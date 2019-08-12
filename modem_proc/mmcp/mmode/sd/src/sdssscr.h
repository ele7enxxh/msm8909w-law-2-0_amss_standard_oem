#if(!defined(SDSSSCR_H) || defined(FEATURE_SD_ENABLE_DEBUG_STRINGS))

#ifndef SDSSSCR_H
#define SDSSSCR_H
#endif

/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

   S Y S T E M   S E L E C T I O N   S C R I P T   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with SDSSSCR.C (i.e. the System Selection Script component of the SD).

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdssscr.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/19/13   jvo     Remove all MDM/MSM Fusion features
04/28/13   xs      Fix tele bsr timeline during HDR traffic
04/28/13   xs      LTE avail and tele bsr for non-SVLTE
11/30/12   xs      SD send acq fail to CM during HDR silent redial
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/14/13   cl      Recursive BSR
11/02/12   skk      Updated SD construct to consider internal service status
                   (i.e. SYS_SRV_STATUS_NO_SRV_INTERNAL).
10/01/12   vm      Make Sprint TELE BSR timer index same across all PLs
09/01/12   xs      LTE telescopic BSR in SVLTE support
07/23/12   xs      Added support of location based BSR
07/17/12   jh      Added LTE Available File BSR support
05/14/12   ak      Do not reset OOS after camping on Limited regional service.
03/25/12   cl      Two BSR timer for LTE reselection requirements support
03/14/12   cl      GW/L limited reselection to LTE/DO enhancement
02/29/12   mj      Remove obsolete script conditions, rename GWL script
                   conditions to 3GPP
02/12/12   aj      ensure MAP avoid systems are scanned during service loss
01/11/12   aj      update emerg call handling on hybr2
01/06/12   ak      Make OOS timeline on main stack configurable.
01/01/12   cl      n_min_voice feature for SVLTE out of service algorithm
11/28/11   ak      Change BSR behaviour when it involves 3GPP
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/15/11   ak      Enabled background PLMN search for multimode configuration.
06/02/11   rk      Fix for emergency call origination when UE camped on LTE
05/20/11   fc      Use NV config to enable/disable ALT_EVDO_PWRUP_ALG_1.
03/25/11   gm      LTE (Lmtd) to DO resel support
03/10/11   gm      Add QXDM NV Browser support for ECBM timer.
01/31/11   gm      Extend acquisition while in ECBM & sys loss
02/07/11   xs      Dynamic control support of FEATURE_JCDMA
01/19/11   ak      Added support for periodic GWL scans in DO only coverage.
10/25/10   aj      BSR after IRAT from LTE->DO should be triggered by
                   TMR_CDMA_RESEL
10/01/10   ak      Merged the Fusion code from Dev branch.
08/20/10   dq      Added enhanced F3 messages for activated constructs.
08/04/10   ay      Added a helper script
07/08/10   sv      Removed lte_forever script
                   Added Initial functionality for Fusion LTE BSR
06/10/10   ay      Added support for EOOS
06/21/10   rk      Redesign of WRLF feature.
05/26/10   xs      Fixed number comments in sdssscr_sched_e_type
04/01/10   xs      Make RESEL_TIMEOUT_SS_PREF NV configurable
03/19/10   aj      MMSS support
03/08/10   rp      Added SDSSSCR_CON_CHK_MODE_PREF_SET_SRV_STATUS to Misc SS
                   script construct
02/24/10   aj      Support PS optimized silent redial
02/16/10   sg      Changes in SD to support eCall feature requirement.
02/08/10   jqi     ESS support.
01/15/10   ay      Added LTE Connected mode support
12/21/09   ak      Removing UOOS
10/16/09   ay      Added code to support GWL
06/10/09   fj      Added support for MMSS option 1.1.
07/10/09   np      Adding LTE Code
06/29/09   rk      Handling new pref_reason SD_SS_PREF_REAS_MULTIMODE_SCAN
06/10/09   sv      Added SDSSSCR_CDMA_ACQ_MICRO to do a Micro CDMA Scan
04/03/09   aj      Added support for emergency callback exit on sys lost
02/13/09   sn      For user pwr save exit event, in pwr save, continue with
                          earlier  4 + 1 GW  band group scan sequence.
01/21/09   rk      Added new construct IS_BAND_PREF_CHGD.
11/28/08   rm      In UOOS report Limited service to NAS if sim state is
                   CS_PS_Invalid. Reverting part of the fix for CR-124247
11/05/08   sv      Moved sdssscr_flag_e_type to sdss_flag_e_type
10/23/08  ak       Fixing the no firmware scenario
09/15/08   pk      Added support for WLAN UOoS
08/29/08   rm      Searching for GW systems in Automatic mode when SIM
                   is not avilable
08/16/08   np      Added NV timer for deep sleep power save
07/30/08   sv      Added pref reason SDSSSCR_PREF_REAS_VAL_STACK_SYNC_UP and
                   flag SDSSSCR_FLAG_SYNC_UP_IN_PROGRESS
06/25/08   sn      Added new enum SDSSSCR_COND_SIM_DOMAIN_VALID in
                   sdssscr_cond_e_type
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
03/20/08   pk/aj   Added support for Time based hybrid HDR UOOS
03/14/08   rm      CM-NAS interface for new service req type
01/22/08   rm      Reverted the changes done for- support for SIM Invalid
                   and UOOS
09/18/07   pk      Added support for non-Hybrid UOOS
06/15/07   rp      Support for handing no service in connected mode
06/06/07   pk      UOOS enhancements cont...
06/06/07   pk      UOOS enhancements...
05/29/07   pk      UMTS UOOS enhancements
05/29/07   pk      FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH
05/29/07   pk      Added support for SIM Invalid and UOOS
05/02/07   jqi     DO not turn off the redirection afer IHO if the expected
                   SID/NID of the redirected system is WILDCARD.
                   Expanded 1x system reselection timer value from 16-bit to
                   32-bit.
03/28/07   jqi     1x 100% PSIS avoidance requirement.
02/12/07   jqi     Kept srv off until OH after failed EGSRDM.
01/04/07   pk      Added support for VoIP -> 1X Handoffs
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS
                   builds
11/29/06   ka      Limited service resel timer expiry results in sending
                   MEAS_BACK action instead of full service req. Satisfies
                   GCF 6.1.2.6.
11/15/06   pk      Added support for HYBR_GW
10/02/06   sk      Renamed AVOID_CANCEL to AVOID_CANCEL_COND.
09/08/06   sk      Added SDSSSCR_NV_ITEM_WLAN_PI_THR.
08/24/06   pk      Added support for IF construct and GCF test fixes
08/23/06   jqi     Added support for system reselection during BCMCS flow.
08/15/06   pk      Added support for GW UOOS
08/07/06   pk      Added support for 1X UOOS
07/13/06   jqi     Added the system lost reason for HDR system lost event.
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
03/27/06   dk      Added wlan power save mode pref script.
03/20/06   sk      Added acquisition type SDSSSCR_ACQ_NOT_LESS_PREF_OR_HOME.
03/09/06   jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
01/09/06   sk      Added SDSSSCR_ACQ_MORE_PREF_THAN_SRV_SYS.
10/05/05   sk      Added support for configuring SD values from NV.
09/10/05   pk      Added DEL_CNT( LIST, START_POS, CNT ),
                   IS_TIME_GR(INSTANCE, SRV_VAL, TIME) constructs and
                   SDSSSCR_HDR_ACQ_ROTATE, SDSSSCR_HDR_ACQ_ROTATE_SRV_LOST
                   called scripts
09/01/05   jqi     DO system reselection optimization.
08/16/05   sk      Fixed numbering comments in sdssscr_con_e_type.
                   KDDI 1x reselection and GSRDM requirements.
08/05/05   jqi     Added support for Best preferred acquisition for DO.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/11/05   jqi     Added avoidance timer for access failures and fades.
07/08/05   sk      Added SDSSSCR_CON_IS_EVENT_CNT to Condition script
                   constructs.
05/16/05   pk      Added SDSSSCR_CON_CHK_SET_SRV_STATUS to Misc SS script
                   construct
05/16/05   pk      Defined SDSSSCR_MAX_LOOP_BACK_CNT, to control the number
                   of times the loop is executed
01/30/05   sj      Added support for WLAN.
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
09/08/04   jqi     Added ps dormant optimization.
07/29/04   jqi     Quadband optimizatioin - eliminate scheduling.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/21/04   RC      Improve association (collocation) determination algorithm
                   for HDR systems while in hybrid operation - CR 44638.
06/21/04   jqi     Use sd nam selection indication when there is a
                   subscription change.
06/07/04   jqi     More quad-band enhancement for automatic mode.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
01/22/04   SJ      Added support for new script construct is_pref_chgd().
12/08/03   SJ      Added support for multistage full srv reselection.
11/17/03   SJ      Updates for HDR usability and new hdr power save.
11/10/03   RC      Added support for SDSSSCR_CON_IS_NET_SEL script construct.
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
06/10/03   RC      Added counter to track CDMA available indications.
04/23/03   SJ      Added early power scan support for GSM/WCDMA.
02/05/03   SJ      Added power save script for manual mode.
12/09/02   RC      Added support for FEATURE_JCDMA.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
01/18/02   az      Added 2 new enum elements in sdssscr_e_type.
10/30/01   RC      Added support for IS-683C collcated systems.
09/07/01   RC      Added support for T53 and JCDMA features.
04/26/01   RC      Modified reacquisition schedules.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
02/01/01   RC      Changes per 8th week of I&T.
01/12/01   RC      Changes per 6th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
12/14/00   RC      Changes per 3rd week of I&T.
12/04/00   RC      Changes per 2nd week of I&T.
11/27/00   RC      Changes per 1st week of I&T.
11/17/00   RC      Initial release.

===========================================================================*/


#include "mmcp_variation.h"
#include "sd.h"        /* External interface to SD */
#include "sd_v.h"
#include "sdss.h"      /* External interface to sdss.c */
#include "comdef.h"    /* Definition for basic types and macros */
#include "sdenumdbg.h" /* Contains all macros that describe the
                       ** enumerations enhanced with string generation.
                       */

/* <EJECT> */
/*---------------------------------------------------------------------------
                           SS-Script Constructs
---------------------------------------------------------------------------*/

/* Enumeration SS-Script Constructs.
*/
SD_BEGIN_ENUM(sdssscr_con_e) {

  /* List Manipulator SS-Script-Constructs.
  */
  SD_CONSTR_ELEMENT(MANIPULATOR)   SD_ENUM_ELEMENT_INDEX(= 0),
                                                /* FOR RANGE CHECKING */

  SD_CONSTR_ELEMENT(CLR),
                            /* CLR( LIST ) - Clears the system record list/
                            ** item LIST */

  SD_CONSTR_ELEMENT(CPY),
                            /* CPY( LIST_TO, LIST_FROM ) - Clear the system
                            ** record list LIST_TO and copy all the elements
                            ** from the system record list LIST_FROM into the
                            ** system record list LIST_TO */

  SD_CONSTR_ELEMENT(DEL),
                            /* DEL( LIST, LIST_REF ) - Delete the system
                            ** records that are referenced by the system
                            ** record list LIST_REF from the system record
                            ** list LIST */

                            /* 4 */
  SD_CONSTR_ELEMENT(DEL_SYS),
                            /* DEL_SYS( LIST, DEL_CAT ) - Delete system
                            ** records from LIST according to DEL_CAT */


  SD_CONSTR_ELEMENT(DEL_BAND),
                            /* DEL_BAND( LIST, SYS_REF ) - Delete system
                            ** records from LIST which does not have same
                            ** band as that in reference system   */

  SD_CONSTR_ELEMENT(APP),
                            /* APP( LIST_TO, LIST_FROM ) - Append the
                            ** elements from the system record list/item
                            ** LIST_FROM to the end of system record list
                            ** LIST_TO. Duplicate elements are not
                            ** appended */

  SD_CONSTR_ELEMENT(ORDER),
                            /* ORDER( LIST, LIST_REF ) - Order the elements
                            ** of the system record list LIST according to
                            ** their order in the reference list LIST_REF */

                            /* 8 */
  SD_CONSTR_ELEMENT(ORDER2),
                            /* ORDER2( LIST, LIST_REF, ORDER_CAT ) - Order
                            ** the elements of the system record list LIST
                            ** according to the order_cat field of reference
                            ** system in the list LIST_REF.*/

  SD_CONSTR_ELEMENT(SORT),
                            /* SORT( LIST, COMP_CAT ) - Sort the system
                            ** record list LIST according to the COMP_CAT */

  SD_CONSTR_ELEMENT(GEO_EXT),
                            /* GEO_EXT( LIST, LIST_REF, EXT_CAT ) - Extract
                            ** systems from the GEO of LIST_REF[0] (i.e.), 1st
                            ** system on LIST_REF) into LIST according to the
                            ** EXT_CAT category */

  SD_CONSTR_ELEMENT(CREATE_GW_LTD),
                            /* CREATE_GW_LTD( LIST ) - create a conterpart
                            ** GW limited list from the system record list/
                            ** item LIST and store in the system record
                            ** database */

                            /* 12 */
  SD_CONSTR_ELEMENT(SET_MAPE), /* set the marked system as MAPE */
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
                            /* 16 */
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* List Operator SS-Script-Constructs.
  */
  SD_CONSTR_ELEMENT(OPERATOR)      SD_ENUM_ELEMENT_INDEX(= 20),
                                                /* FOR RANGE CHECKING */

  SD_CONSTR_ELEMENT(MEAS),
                            /* MEAS( LIST, MODE, TYPE ) - Request a channel
                            ** strength measurement of mode MODE and type
                            ** TYPE for all the systems that are referenced
                            ** by the system record list/item LIST */

  SD_CONSTR_ELEMENT(GET_NET),
                            /* GET_NET - Request to get the available
                            ** networks. */

  SD_CONSTR_ELEMENT(ACQ),
                            /* ACQ( LIST, ACQ_TYPE ) - Step through the
                            ** system record list/item LIST (first to last)
                            ** and try to acquire systems that are referenced
                            ** by LIST and that were not already attempted
                            ** acquisition since the last time the NEW_ACQ
                            ** construct was encountered (and in accordance
                            ** with ACQ_TYPE and the current SS-Preference */

                            /* 24 */
  SD_CONSTR_ELEMENT(ACQ_TRUE_MODE),
                            /* ACQ_TRUE_MODE( LIST, ACQ_TYPE ) - Step through the
                            ** system record list/item LIST (first to last)
                            ** and try to acquire systems that are referenced
                            ** by LIST and that were not already attempted
                            ** acquisition since the last time the NEW_ACQ
                            ** construct was encountered (and in accordance
                            ** with ACQ_TYPE and the current SS-Preference */

  SD_CONSTR_ELEMENT(ACQ_MODE),
                            /* ACQ_MODE - Set the CDMA acquisition mode */

  SD_CONSTR_ELEMENT(NEW_ACQ),
                            /* NEW_ACQ - Start a new acquisition by
                            ** incrementing the new-acquisition counter */

  SD_CONSTR_ELEMENT(START_TIME),
                            /* START_ACQ_TIME - Starts the acquisition
                            ** timer */

                            /* 28 */
  SD_CONSTR_ELEMENT(ACQ_ALL),
                            /* ACQ_ALL( LIST, ACQ_TYPE ) - Equivalent to
                            ** the ACQ construct except that also systems
                            ** that were already attempted acquisition since
                            ** the last time the NEW_ACQ construct was
                            ** encountered are being attempted acquisition */

  SD_CONSTR_ELEMENT(ACQ_CNT),
                            /* ACQ_CNT( LIST, ACQ_TYPE, START_POS, CNT ) -
                            ** Equivalent to the ACQ construct except that
                            ** LIST is traversed START_POS to START_POS-1 and
                            ** acquisition is bounded to CNT elements */


  SD_CONSTR_ELEMENT(ACQ_AVOID_CNT),
                            /* ACQ_AVOID_CNT( LIST, ACQ_TYPE, START_POS, CNT)
                            ** - Equivalent to the ACQ_CNT construct except
                            ** that it allows to attempt the avoid system.
                            */

  SD_CONSTR_ELEMENT(ACQ_ABSR_CNT),
                            /* ACQ_ABSR_CNT( LIST, ACQ_TYPE, START_POS, CNT)
                            ** - Equivalent to the ACQ construct except that
                            ** LIST is traversed START_POS to START_POS-1 and
                            ** acquisition is bounded to CNT elements */

                            /* 32 */
  SD_CONSTR_ELEMENT(ACQ_TIME),
                            /* ACQ_TIME( LIST, ACQ_TYPE, START_POS, TIME ) -
                            ** Equivalent to the ACQ construct except that
                            ** LIST is traversed START_POS to START_POS-1 and
                            ** acquisition is bounded to TIME seconds since
                            ** the START_TIME script construct was last
                            ** encountered */
  SD_CONSTR_ELEMENT(ACQ_TIME_EOOS),
                            /* ACQ_TIME( LIST, ACQ_TYPE, START_POS, TIME ) -
                            ** Equivalent to the ACQ construct except that
                            ** LIST is traversed START_POS to START_POS-1 and
                            ** acquisition is bounded to TIME seconds since
                            ** the START_TIME script construct was last
                            ** encountered */

  SD_CONSTR_ELEMENT(ACQ_CNT_OR_TIME_EOOS),
                            /* ACQ_CNT_OR_TIME( LIST, ACQ_TYPE, START_POS), CNT),
                            ** TIME ) -
                            ** Equivalent to the ACQ construct except that
                            ** LIST is traversed START_POS to START_POS-1 and
                            ** acquisition is bounded to TIME seconds since
                            ** the START_TIME script construct was last
                            ** encountered or CNT entries*/

  SD_CONSTR_ELEMENT(ACQ_RX),
                            /* ACQ_RX( LIST, ACQ_TYPE, RX ) - Equivalent to
                            ** the ACQ construct except that only systems
                            ** with Rx-power measurement value >= RX are
                            ** being attempted acquisition */

                            /* 36 */
  SD_CONSTR_ELEMENT(ACQ_PI),
                            /* ACQ_PI( LIST, ACQ_TYPE, PI ) - Equivalent to
                            ** the ACQ construct except that only systems
                            ** with pilot strength measurement value >= PI
                            ** are being attempted acquisition */

  SD_CONSTR_ELEMENT(SCHED),
                            /* SCHED( LIST, SCHED, ACQ_TYPE ) - Schedule the
                            ** system records that are referenced by the
                            ** system record list/item LIST to be attempted
                            ** acquisition of type ACQ_TYPE at time points
                            ** that are specified by SCHED */

  SD_CONSTR_ELEMENT(SCHED_COND),
                            /* SCHED_COND( LIST, SCHED, ACQ_TYPE ) -
                            ** Identical to SCHED(), except that only
                            ** schedule systems that are not already on equal
                            ** or higher priority reacquisition schedule */

  SD_CONSTR_ELEMENT(AVOID),
                            /* AVOID( LIST, TIME ) - Mark the system records
                            ** that are referenced by the system record
                            ** list/item LIST to be avoided from any
                            ** acquisition attempts for TIME seconds */

                            /* 40 */
  SD_CONSTR_ELEMENT(AVOID_COND2),
                            /* AVOID( LIST, NV_ITEM, COND ) - Mark the system
                            ** records that are referenced by the system
                            ** record list/item LIST to be avoided from any
                            ** acquisition attempts for TIME seconds or upon
                            ** a successful access.
                            ** Where TIME is the value stored in the
                            ** configurable items table at index
                            ** SDSS_NV_ITEM_"NV_ITEM". */

  SD_CONSTR_ELEMENT(AVOID_CANCEL_COND),
                            /* AVOID_CANCEL_COND( LIST, COND ) - Remove the
                            ** avoidance placed on systems that are
                            ** referenced by LIST and have been avoided till
                            ** condition COND is met.
                            ** AVOID_CANCEL( LIST ) - Same as above, with
                            ** COND equal to CANCEL_ALL.
                            */

  SD_CONSTR_ELEMENT(AVOID_COND),
                            /* AVOID_COND( LIST, TIME, COND ) - Mark the
                            ** system records that are referenced by the
                            ** system record list/item LIST to be avoided
                            ** from any acquisition attempts for TIME seconds
                            ** or until the condition is met whichever comes
                            ** first */

                            /* 43 */
  SD_CONSTR_ELEMENT(SCHED_COND2),
                            /* SCHED_COND2( LIST, SCHED, ACQ_TYPE, CMP_SCHED )
                            ** Identical to SCHED(), except that only schedule
                            ** systems that are not already on equal or higher
                            ** priority reacquisition schedule than CMP_SCHED.
                            */

  SD_CONSTR_ELEMENT(ACQ_PI_X),
                            /* ACQ_PI_X( LIST, ACQ_TYPE, X, PI ) - Equivalent to
                            ** the ACQ construct except that only X technology systems
                            ** with pilot strength measurement value >= PI and others irrespective
                            ** of signal are being attempted acquisition */

  SD_CONSTR_ELEMENT(ACQ_NETLIST),
                            /* ACQ_NETLIST( LIST, ACQ_TYPE, NETLIST ) -
                            ** Equivalent to the ACQ  construct except
                            ** that acquistion is issued only when
                            ** background service search yields >=1 PLMN.
                            */

  SD_CONSTR_ELEMENT(RESET_GWL_EOOS_PARAMS),
                            /* SDSSSCR_CON_RESET_GWL_EOOS_PARAMS
                            ** resets the EOOS UMTS parameters
                            */

                            /* 47 */
  SD_CONSTR_ELEMENT(RESET_EOOS_TIMER),
                            /* SDSSSCR_CON_RESET_GW_UOOS_ACQ_PARA
                            ** resets the GW EOOS parameters
                            */

  SD_CONSTR_ELEMENT(SRV_REQ),
                            /* SRV_REQ_TYPE (VAL) -
                            **  Sets the request type to VAL
                            */

  SD_CONSTR_ELEMENT(SET_GW_PWR_SAVE_ACQ_STAGE),
                            /* GW_PWR_SAVE_ACQ_STAGE(VAL) -
                            **  Sets GW pwr save acq stage to val
                            */

  SD_CONSTR_ELEMENT(EOOS_PHASE),
                            /* EOOS_PHASE( VAL ) -
                            ** Sets EOOS phase value to VAL.
                            */

                            /* 51 */
  SD_CONSTR_ELEMENT(ACTIVATE_EOOS),

  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Script Control Constructs.
  */
  SD_CONSTR_ELEMENT(CTRL)          SD_ENUM_ELEMENT_INDEX(= 55),
                                          /* FOR RANGE CHECKING */

                            /* 56 */
  SD_CONSTR_ELEMENT(LOOP_TIME),
                            /* LOOP_TIME( TIME ) - Repeat executing the
                            ** script between this construct and the matching
                            ** LOOP construct for TIME seconds since this
                            ** construct was first encountered */

  SD_CONSTR_ELEMENT(LOOP_NUM),
                            /* LOOP_NUM( NUM ) - Repeat executing the script
                            ** between this construct and the matching LOOP
                            ** construct for NUM times */

  SD_CONSTR_ELEMENT(LOOP_LIST),
                            /* LOOP_LIST( LIST ) - Repeat executing the
                            ** script between this construct and the matching
                            ** LOOP construct for count(LIST) times */

  SD_CONSTR_ELEMENT(LOOP_COND),
                            /* LOOP_COND - Repeat executing the script
                            ** between this construct and the matching LOOP
                            ** construct until the COND scan is TRUE */

                            /* 60 */
  SD_CONSTR_ELEMENT(LOOP_FOREVER),
                            /* LOOP_FOREVER - Repeat executing the script
                            ** between this construct and the matching LOOP
                            ** construct forever */

  SD_CONSTR_ELEMENT(LOOP_TIME_EOOS),
                            /* LOOP_TIME_EOOS( PHASE ) - Repeat executing the
                            ** script between this construct and the matching
                            ** LOOP construct for num seconds specified
                            ** by the EOOS phase duration parameter
                            */

  SD_CONSTR_ELEMENT(LOOP),
                            /* LOOP - Loop-back to the matching LOOP_XXX
                            ** construct */

  SD_CONSTR_ELEMENT(PREV_SCR),
                            /* PREV_SCR - Return to perviously active
                            ** SS-Script and continue with that script
                            ** execution */

                            /* 64 */
  SD_CONSTR_ELEMENT(PROC_EVT),
                            /* PROC_EVT( SS_EVENT ) - Process the SS_EVENT
                            ** SS-Event */

  SD_CONSTR_ELEMENT(ACT_SCR),
                            /* ACT_SCR( SCRIPT ) - Make the script that is
                            ** referenced SCRIPT the active SS-Script */

  SD_CONSTR_ELEMENT(ACT_SCR_RX),
                            /* ACT_SCR_RX( SCRIPT, LIST, RX ) - Make SCRIPT
                            ** the active System Selection Script only if at
                            ** least one of the systems that are referenced
                            ** by the system record list LIST has an Rx-power
                            ** measurement value >= RX */

  SD_CONSTR_ELEMENT(ACT_SCR_PI),
                            /* ACT_SCR_PI( SCRIPT, LIST, PI ) - Make SCRIPT
                            ** the active System Selection Script only if at
                            ** least one of the systems that are referenced
                            ** by system record list LIST has a pilot
                            ** measurement value >= PI */

                            /* 68 */
  SD_CONSTR_ELEMENT(IF),
                            /* IF( IF_COND, PARA1, PARA2, PARA3 ) -
                            ** conditional execution. If the IF_COND is true
                            ** execute the scripts in the IF section
                            ** If the condition is false, and a else portion
                            ** is present, execute the ELSE section of the
                            ** script
                            */

  SD_CONSTR_ELEMENT(ELSE),
                            /* ELSE - Marks the beginning of the else block
                            */

  SD_CONSTR_ELEMENT(ENDIF),
                            /* ENDIF - Marks the end of the IF block
                            */

  SD_CONSTR_ELEMENT(END),
                            /* END - Mark the end of an SS-Script.
                            ** Under normal situation this script construct
                            ** should never be reached */

                            /* 72 */
  SD_CONSTR_ELEMENT(NEST_IF),
                            /* NEST_IF( IF_COND, PARA1, PARA2, PARA3 ) -
                            ** conditional execution. If the IF_COND is true
                            ** execute the scripts in the IF section
                            ** If the condition is false, and a else portion
                            ** is present, execute the ELSE section of the
                            ** script
                            */

  SD_CONSTR_ELEMENT(NEST_ELSE),
                            /* NEST_ELSE - Marks the beginning of the else block
                            */

  SD_CONSTR_ELEMENT(NEST_ENDIF),
                            /* NEST_ENDIF - Marks the end of the IF block
                            */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Miscellaneous SS-Script-Constructs.
  */
  SD_CONSTR_ELEMENT(MISC)          SD_ENUM_ELEMENT_INDEX(= 75),
                                          /* FOR RANGE CHECKING */

  SD_CONSTR_ELEMENT(CONT),
                            /* CONT( TIME ) - Instructs the SS-Client to
                            ** continue with its current activity while
                            ** setting-up a TIME timer. When this timer
                            ** expires the SS-Client shall call on the
                            ** SD_SS_IND_MISC_TIMER SS-Indication function */

  SD_CONSTR_ELEMENT(CONT_REDIR),
                            /* CONT_REDIR( TIME ) - Conditionally stops
                            ** script execution and instructs the SS-Client
                            ** to continue until TIME seconds passed since
                            ** the most recent redirection indication was
                            ** received */

  SD_CONSTR_ELEMENT(CONT2),
                            /* CONT2( NV_ITEM ) - Instructs the SS-Client to
                            ** continue with its current activity while
                            ** setting-up a TIME timer. When this timer
                            ** expires the SS-Client shall call on the
                            ** SD_SS_IND_MISC_TIMER SS-Indication function.
                            ** Where TIME is the value stored in the
                            ** configurable items table at index
                            ** SDSS_NV_ITEM_"NV_ITEM". */

                            /* 79 */
  SD_CONSTR_ELEMENT(CONT2_REDIR),
                            /* CONT2_REDIR( NV_ITEM ) - Conditionally stops
                            ** script execution and instructs the SS-Client
                            ** to continue until TIME seconds passed since
                            ** the most recent redirection indication was
                            ** received. Where TIME is the value stored in
                            ** the configurable items table at index
                            ** SDSS_NV_ITEM_"NV_ITEM". */

  SD_CONSTR_ELEMENT(CONT3),
                            /* CONT3( EFS_ITEM ) - Instructs the SS-Client to
                            ** continue with its current activity while
                            ** setting-up a TIME timer. When this timer
                            ** expires the SS-Client shall call on the
                            ** SD_SS_IND_MISC_TIMER SS-Indication function.
                            ** Where TIME is the value stored in the
                            ** configurable items table at index
                            ** SDSS_EFS_ITEM_"EFS_ITEM". */

  SD_CONSTR_ELEMENT(CONT2_WAIT),
                            /* CONT2_WAIT( TYPE, NV_ITEM ) - Instructs the
                            ** SS-Client to continue with its current
                            ** activity if a TIME  timer does not expired.
                            ** When this timer expires the SS-Client shall
                            ** call on the SD_SS_IND_MISC_TIMER SS-Indication
                            ** function. Where TIME is the value stored in
                            ** the configurable items table at index
                            ** SDSS_NV_ITEM_"NV_ITEM". */

  SD_CONSTR_ELEMENT(CONT3_WAIT),
                            /* CONT3_WAIT( TYPE1, NV_ITEM1, TYPE2, NV_ITEM2)
                            ** Instructs the SS-Client to continue with its
                            ** current activity if either a TYPE1 or a TYPE2
                            ** specific timer does not expires. When this
                            ** timer expires the SS-Client shall call on the
                            ** SD_SS_IND_MISC_TIMER SS-Indication function.
                            ** Where TIME is the value stored in the
                            ** configurable items table at index
                            ** SDSS_NV_ITEM_"NV_ITEM". */


  SD_CONSTR_ELEMENT(CONT_ABSR),
                            /* CONT_ABSR( NV_ITEM1, NV_ITEM2  ) - Instructs
                            ** the SS-Client to continue with its current
                            ** activity while setting-up a TIME timer. When
                            ** this timer expires the SS-Client shall call on
                            ** the SD_SS_IND_MISC_TIMER SS-Indication
                            ** function. Where TIME is the value stored in
                            ** the configurable items table at index
                            ** SDSS_NV_ITEM_"NV_ITEM". */

                            /* 83 */
  SD_CONSTR_ELEMENT(CONT_SET),
                            /* CONT_SET( TYPE, VALUE ) - Set the start time
                            ** of a TIME timer. */

  SD_CONSTR_ELEMENT(ACCEPT),
                            /* ACCEPT - Accept BS proposed mode, band and/or
                            ** CDMA channel/AMPS system. This SS-Action is
                            ** only valid for Redirection, Channel Assignment
                            ** or Handoff Direction */

  SD_CONSTR_ELEMENT(REJECT),
                            /* REJECT - Reject BS proposed mode, band and/or
                            ** CDMA channel/AMPS system. This SS-Action is
                            ** only valid for Redirection, Channel Assignment
                            ** or Handoff Direction */

  SD_CONSTR_ELEMENT(PWR_SAVE),
                            /* PWR_SAVE - Instruct the SS-Client to enter
                            ** Power-Save mode of operation. Once entering
                            ** Power-Save mode of operation, the SS-Client
                            ** should call on the
                            ** SD_SS_IND_MISC_PWR_SAVE_ENTER SS-Indication
                            ** function. */

                            /* 87 */
  SD_CONSTR_ELEMENT(REDIR_OFF),
                            /* REDIR_OFF - Set the redirection indication to
                            ** disabled */

  SD_CONSTR_ELEMENT(HO_OFF),
                            /* HO_OFF - Set the handoff indication to
                            ** disabled */

  SD_CONSTR_ELEMENT(ACQ_FAIL),
                            /* ACQ_FAIL - Indicate that acquisition over all
                            ** systems has failed */
  SD_CONSTR_ELEMENT(HYBR_BSR_TO_HDR),
                            /* HYBR_BSR_TO_HDR - Indicate LTE to HDR bsr */

  SD_CONSTR_ELEMENT(SRV_STATUS),
                            /* SRV_STATUS( VAL ) - Set the Service Indication to VAL */

  SD_CONSTR_ELEMENT(KICK_HYBR2),
                            /* KICK_HYBR2 - Indicate kick hybr2 stack */

                            /* 91 */
  SD_CONSTR_ELEMENT(SRV_OFF_UNTIL),
                            /* SRV_OFF_UNTIL( CRIT ) - Set the Service
                            ** Indication to OFF. Do not update the service
                            ** indicators before the specified criteria is
                            ** met */

  SD_CONSTR_ELEMENT(RESEL_STT),
                            /* RESEL_STT( STATE ) - Set the Reselection
                            ** State to STATE */

  SD_CONSTR_ELEMENT(RESEL_MODE),
                            /* RESEL_STT( MODE ) - Set the Reselection
                            ** Mode to MODE */


  SD_CONSTR_ELEMENT(CHK_SET_SRV_STATUS),
                            /* CHK_SRV_STATUS_AND_SET( CHK_VAL, SET_VAL )
                            ** If current Service Indication is set to
                            ** CHK_VAL, set the Service Indication to
                            ** SET_VAL */

                            /* 95 */
  SD_CONSTR_ELEMENT(DEL_CNT),
                            /* DEL_CNT( LIST, START_POS, CNT )
                            ** Delete CNT entries beginning from START_POS
                            ** from LIST list */

  SD_CONSTR_ELEMENT(RESET_GW_ACQ_SCAN_STATUS),
                            /* RESET_GW_ACQ_SCAN_STATUS - Reset GW acq scan
                            ** status */

  SD_CONSTR_ELEMENT(SET_FLAG),
                            /* SET_FLAG ( FLAG, SET_VAL ) Set the
                            ** FLAG to SET_VAL */

  SD_CONSTR_ELEMENT(SET_TIMER),
                            /* SET_TIMER( NV_ITEM, TMR_ADJ ) Set timer */

                            /* 99 */
  SD_CONSTR_ELEMENT(MAINTAIN_TIMER),
                            /* MAINTAIN_TIMER Maintain the timer */

  SD_CONSTR_ELEMENT(MAINTAIN_PREV_SCR_TIMER),
                            /* MAINTAIN_PREV_SCR_TIMER Maintain
                            ** pervious script timer */

  SD_CONSTR_ELEMENT(STACK_SYNC_UP),
                            /* STACK_SYNC_UP Sync up the stacks for UOoS */

  SD_CONSTR_ELEMENT(CONT_BSR),
                            /* Start a BSR timer */

                            /* 103 */
  SD_CONSTR_ELEMENT(CONT2_BSR),
                            /* Start a BSR timer, timer value from NV table */

  SD_CONSTR_ELEMENT(BSR_TIMER_RESET),
                            /* Reset BSR timer uptime */

  SD_CONSTR_ELEMENT(EXIT_EMERG_CB),
                            /* exit from emergency callback mode */

  SD_CONSTR_ELEMENT(CHK_MODE_PREF_SET_SRV_STATUS),
                            /* CHK_MODE_PREF_AND_SET( CHK_VAL, SET_VAL )
                            ** If current mode pref Indication is set to
                            ** CHK_VAL), set the Service Indication to
                            ** SET_VAL */

                            /* 107 */
  SD_CONSTR_ELEMENT(CONT_EOOS_SLEEP), /* EOOS sleep*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_CONSTR_ELEMENT(SET_FLAG2),
                              /* SET_FLAG2 ( SS, FLAG, SET_VAL ) Set the
                              ** FLAG to SET_VAL */

  SD_CONSTR_ELEMENT(CONT_TELE_BSR),  /* Start LTE telescopic BSR timeline*/

  SD_CONSTR_ELEMENT(CONT3_BSR),
                            /* Start a BSR timer, timer value from EFS table */

                            /* 111 */
  SD_CONSTR_ELEMENT(RESET_TELE_BSR_TIMER),   /* reset LTE telescopic BSR parameters */
  SD_CONSTR_ELEMENT(OPT_SR_HDR_ACQ_FAIL),
                            /* OPT_SR_HDR_ACQ_FAIL - Indicate that 1 round of current colloc scan done
                            */
  SD_CONSTR_ELEMENT(SET_SKIP_PREF_SCAN_FLAG), /* set skip more pref scan flag */
  SD_CONSTR_ELEMENT(CONT_LONG_BSR),  /* Start LTE telescopic BSR timeline*/

                             /* 115 */
  SD_CONSTR_ELEMENT(RESET_1X_BSR_STAGE), /* reset 1x bsr to stage 1*/
  SD_CONSTR_ELEMENT(LOOP_TIME_TEMP_GSRDM),
  SD_CONSTR_ELEMENT(RESET_RLF),          /* reset RLF/ connected mode */
  SD_CONSTR_ELEMENT(BSR_EVENT),          /* bsr log events */

                            /* 119 */
  SD_CONSTR_ELEMENT(FULL_SRV_LOST),
  SD_CONSTR_ELEMENT(RESET_EMERG_DUP_BAND), /* reset emergency dup band info */ 
  SD_CONSTR_ELEMENT(RESET_OPERATOR_CAPABILITY), /* Reset Operator Capability */
  SD_CONSTR_ELEMENT(SCAN_SCOPE),
                               /* SDSSSCR_CON_SCAN_SCOPE - set the search scan scope type in SD */

  SD_CONSTR_ELEMENT(TOGGLE_SCAN_SCOPE),
                               /* SDSSSCR_CON_TOGGLE_SCAN_SCOPE - Toggle the search scan scope type in SD */
                            /* 124 */
  SD_CONSTR_ELEMENT(GET_SCAN_SCOPE_RULE),
  SD_CONSTR_ELEMENT(CHK_SET_EXTEND_1X_BSR),  /* check and set flag to extend 1x BSR scan in C2K alt algorithm */
  SD_CONSTR_ELEMENT(CHK_RESET_HYBR2_BSR),  /* check srv status on 1x/HDR and reset hybr2_bsr flag */
  SD_CONSTR_ELEMENT(INC_SIB8_LOOP_NUM),    /* Increment SIB8 based 1x scan loop number */
  SD_CONSTR_ELEMENT(CS_EMERG_SCAN_FAIL), /* used to inform AP when one full scan of GW ACQ fails for e911 call */


                            /* 129 */
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)

                            /* 132 */
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)

                            /* 133 */
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)

                            /* 134 */
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)

                            /* 145 */
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)

                            /* 148 */
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)

                            /* 152 */
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)
  SD_ENUM_UNDEFINED_ELEMENT(UNDEFINED_CONSTRUCT)

  /* Condition Script Constructs.
  */
  SD_CONSTR_ELEMENT(COND)         SD_ENUM_ELEMENT_INDEX(= 154),
                                         /* FOR RANGE CHECKING */

  SD_CONSTR_ELEMENT(1ST_COND),
                            /* To be used for range checking only */

                            /* 156 */
  SD_CONSTR_ELEMENT(NOT),
                            /* NOT - Negate the value of the following
                            ** condition script construct */

  SD_CONSTR_ELEMENT(IS),
                            /* IS( VAL ) - Evaluates to TRUE iff VAL is
                            ** equal to a TRUE boolean condition */

  SD_CONSTR_ELEMENT(IS_NOT),
                            /* IS_NOT( VAL ) - Evaluates to TRUE iff VAL is
                            ** equal to a FALSE boolean condition */

  SD_CONSTR_ELEMENT(IS_EVENT),
                            /* IS_EVENT( VAL ) - Evaluates to TRUE iff VAL is
                            ** equal the current SS-Event */

                            /* 160 */
  SD_CONSTR_ELEMENT(IS_OPR_MODE),
                            /* IS_OPR_MODE( VAL ) - Evaluates to TRUE iff VAL
                            ** includes the current operating mode */

  SD_CONSTR_ELEMENT(IS_FLAG_SET),
                            /* IS_FLAG_SET( VAL ) - Evaluates to TRUE iff VAL
                            ** includes the current operating mode */

  SD_CONSTR_ELEMENT(IS_PREF_REAS),
                            /* IS_PREF_REAS( VAL ) - Evaluates to TRUE iff
                            ** VAL equals the current SS-Preference reason */

  SD_CONSTR_ELEMENT(IS_ORIG_MODE),
                            /* IS_ORIG_MODE( VAL ) - Evaluates to TRUE iff
                            ** VAL includes the current origination mode */

                            /* 164 */
  SD_CONSTR_ELEMENT(IS_STATE),
                            /* IS_STATE( VAL ) - Evaluates to TRUE iff VAL
                            ** equal the current SS-State */

  SD_CONSTR_ELEMENT(IS_ACQ_STATE),
                            /* IS_ACQ_STATE( VAL ) - Evaluates to TRUE iff
                            ** VAL equal the current acquisition state */

  SD_CONSTR_ELEMENT(IS_ACQ_TYPE),
                            /* IS_ACQ_TYPE( VAL ) - Evaluates to TRUE iff VAL
                            ** equal the type of the most recent
                            ** acquisition attempt */

  SD_CONSTR_ELEMENT(IS_REDIR),
                            /* IS_REDIR( VAL ) - Evaluates to TRUE iff VAL
                            ** includes the current/proposed redirection
                            ** type */

                            /* 168 */
  SD_CONSTR_ELEMENT(IS_HANDOFF),
                            /* IS_HANDOFF( VAL ) - Evaluates to TRUE iff VAL
                            ** includes the current/proposed handoff */

  SD_CONSTR_ELEMENT(IS_RESEL_STT),
                            /* IS_RESEL_STT( VAL ) - Evaluates to TRUE iff
                            ** VAL equals the current reselection state */

  SD_CONSTR_ELEMENT(IS_RESEL_MODE),
                            /* IS_RESEL_MODE( VAL ) - Evaluates to TRUE iff
                            ** VAL equals the current reselection mode */

  SD_CONSTR_ELEMENT(IS_CDMA_AVAIL),
                            /* IS_CDMA_AVAIL( CNT ) - Evaluates to TRUE iff
                            ** CDMA-available counter < CNT */

                            /* 172 */
  SD_CONSTR_ELEMENT(IS_SRV_STATUS),
                            /* IS_SRV_STATUS( VAL ) - Evaluates to TRUE iff
                            ** VAL equals the current srv status */

  SD_CONSTR_ELEMENT(IS_ACC_REAS),
                            /* IS_ACC_REAS( VAL ) - Evaluates to TRUE iff VAL
                            ** equals the latest access attempt reason */

  SD_CONSTR_ELEMENT(IS_ACC_TERM),
                            /* IS_ACC_TERM( VAL ) - Evaluates to TRUE iff VAL
                            ** equals the latest access attempt termination
                            ** reason */

  SD_CONSTR_ELEMENT(IS_ACC_FAIL),
                            /* IS_ACC_FAIL( CNT, TIME ) - Evaluates to TRUE
                            ** iff registration access has failed at least
                            ** CNT times and TIME minutes */

                            /* 176 */
  SD_CONSTR_ELEMENT(IS_SID),
                            /* IS_SID( LST, VAL ) - Evaluates to TRUE iff VAL
                            ** describes the SID/NID to expected SID/NID
                            ** relation of at least one of the systems that
                            ** are referenced by the system record list LST*/

  SD_CONSTR_ELEMENT(IS_MODE),
                            /* IS_MODE( LST, VAL ) - Evaluates to TRUE iff
                            ** VAL includes the mode designation of the
                            ** systems that is referenced by LST */

  SD_CONSTR_ELEMENT(IS_BAND),
                            /* IS_BAND( LST, VAL ) - Evaluates to TRUE iff
                            ** VAL includes the band class designation of the
                            ** systems that is referenced by LST */

  SD_CONSTR_ELEMENT(IS_PRL),
                            /* IS_PRL( LST, VAL ) - Evaluates to TRUE iff VAL
                            ** include the PRL designation of the systems
                            ** that is referenced by LST */

                            /* 180 */
  SD_CONSTR_ELEMENT(IS_NET_SEL),
                            /* IS_NET_SEL( LST, VAL ) - Evaluates to TRUE iff
                            ** VAL equals the network selection mode of at
                            ** least one of the systems that are referenced
                            ** by the system record list LST */

  SD_CONSTR_ELEMENT(IS_SCHED),
                            /* IS_SCHED( LST, VAL ) - Evaluates to TRUE iff
                            ** VAL equal the reacquisition schedule of at
                            ** least one of the systems that are referenced
                            ** by the system record list LST */

  SD_CONSTR_ELEMENT(IS_PREF_CONF),
                            /* IS_PREF( LST, VAL ) - Evaluates to TRUE iff
                            ** VAL includes the SS-Preference designation of
                            ** the system that is referenced by LST */

  SD_CONSTR_ELEMENT(IS_PREF_CHGD),
                            /* IS_PREF( VAL ) - Evaluates to TRUE iff
                            ** specified SS preference given by VAL was
                            ** changed with the last preferred system
                            ** changed. */

                            /* 184 */
  SD_CONSTR_ELEMENT(IS_BAND_PREF_CHGD),
                            /* IS_PREF( VAL ) - Evaluates to TRUE iff
                            ** band preference given for a VAL (mode) was
                            ** changed with the last preferred system
                            ** changed. */

  SD_CONSTR_ELEMENT(IS_SAME),
                            /* IS_SAME( LST1), LST2), VAL ) - Evaluates to
                            ** TRUE iff LST1 and LST2 are the same with
                            ** respect to the VAL equality category */

  SD_CONSTR_ELEMENT(IS_CONTAIN),
                            /* IS_CONTAIN( LST1, VAL, LST2 ) - Evaluates to
                            ** TRUE iff LST1 contains VAL elements of LST2 */

  SD_CONSTR_ELEMENT(IS_CNT),
                            /* IS_CNT( LST, CNT, CNT_COND, CNT_INC ) -
                            ** Evaluates to TRUE iff LST contains CNT or
                            ** more/less/exactly elements that comply with
                            ** the CNT_INC count inclusion criteria */

                            /* 188 */
  SD_CONSTR_ELEMENT(IS_SRV_DOMAIN),
                            /* IS_SRV_DOMAIN( VAL ) - Evaluates to TRUE iff
                            ** VAL equals the current srv domain */

  SD_CONSTR_ELEMENT(IS_FADE),
                            /* IS_FADE( SYS, VAL ) - Evaluates to TRUE if
                            ** there are at lease CNT number of fades on
                            ** the reference system. */

  SD_CONSTR_ELEMENT(IS_EVENT_CNT),
                            /* IS_EVENT_CNT (EVENT, CNT, TIME ) -
                            ** Evaluates to TRUE iff EVENT has occurred
                            ** CNT times within TIME minutes */


  SD_CONSTR_ELEMENT(IS_TIME),
                            /* IS_TIME_GR(INSTANCE, SRV_VAL, TIME) -
                            ** Evaluates to TRUE iff INSTANCE has SRV_VAL for
                            ** atleast TIME seconds */

                            /* 192 */
  SD_CONSTR_ELEMENT(IS_SYS_LOST),
                            /* IS_SYS_LOST( VAL )- Evaluates to TRUE iff
                            ** VALUE equals the latest system lost reason. */


  SD_CONSTR_ELEMENT(IS_MODE_PREF),
                            /* IS_MODE_PREF( LST, VAL ) - Evaluates to TRUE iff
                            ** VAL includes the mode designation of the
                            ** systems that is referenced by LST */

  SD_CONSTR_ELEMENT(IS_SIM_STATE),
                            /* IS_NOT_SIM_STATE( SIM_SATE )
                            ** Evaluates to TRUE iff the phone's SIM state
                            ** is not equal the given sim_state */

  SD_CONSTR_ELEMENT(IS_PREF_UPDATE),
                            /* IS_PREF_UPDATE(  VAL ) - Evaluates to TRUE
                            ** iff specified SS preference given by VAL was
                            ** the update status with the last preferred
                            ** system changed. */

                            /* 196 */
  SD_CONSTR_ELEMENT(IS_REG_STATUS),/* IS_REG_STATUS ( CURRENT)  -
                            ** Evaluates to TRUE iff the registration
                            ** status is current
                            */

   SD_CONSTR_ELEMENT(IS_ABSR_CNT),
                            /* IS_ABSR_CNT( LESS_THAN_NUM_ABSR_PER_BSR )
                            ** Evaluates to TRUE iff the ABSR count is
                            ** less than the num ABSR per BSR interval.
                            */

  SD_CONSTR_ELEMENT(IS_ECALL_MODE),
                            /* IS_ECALL_MODE () - Evaluates to TRUE iff
                            ** eCall API return ECALL_MODE_ECALL_ONLY */

  SD_CONSTR_ELEMENT(IS_GWL_SCAN_STATUS),
                            /* IS_GWL_SCAN_STATUS( VAL ) -
                            ** Evaluates to TRUE iff the GW scan status
                            ** is set to VAL. */

                            /* 200 */
  SD_CONSTR_ELEMENT(IS_SRV_STATUS2),
                            /* IS_SRV_STATUS2( SS), VAL ) - Evaluates to TRUE iff
                            ** VAL equals the current srv status for SS */

  SD_CONSTR_ELEMENT(IS_EOOS_PHASE),
                            /* IS_EOOS_PHASE( VAL ) - Evaluates to true if the
                            ** EOOS algorithm is in phase indicated by VAL.
                            */

                             /* 202 */
  SD_CONSTR_ELEMENT(IS_LAST_ACT_NET_SEL),   
                            /* IS_LAST_ACT_NET_SEL( VAL ) - Evaluates to TRUE iff
                            ** VAL equals the network selection mode sent to NAS
                            **/

  SD_CONSTR_ELEMENT(LOOP_TIME_CFG),
                            /* LOOP_TIME_CFG (CFG_ITEM) - Repeat executing the
                            ** script between this construct and the matching
                            ** LOOP construct for CFG_ITEM config duration */

  SD_CONSTR_ELEMENT(IS_SRV_MODE),
                            /* IS_SRV_MODE (VAL) -   Evaluates to TRUE iff
                            ** VAL is the current serving system mode
                            */

  SD_CONSTR_ELEMENT(LOOP_NUM_CFG),
                            /* LOOP_NUM_CFG( CFG_ITEM ) - Repeat executing the script
                            ** between this construct and the matching LOOP
                            ** construct for NUM times stored in CFG_ITEM */

                             /* 206 */
  SD_CONSTR_ELEMENT(LOOP_TIME2),
                              /* LOOP_TIME2 (NV_ITEM) - Repeat executing the
                              ** script between this construct and the matching
                              ** LOOP construct for NV_ITEM config duration */


  SD_CONSTR_ELEMENT(IS_LTE_AVAIL_1X),
                              /*  IS_LTE_AVAIL_1X(VAL) - Evaluates to true if
                              ** the VAL equalS to the current lte_avail_1x status */


  SD_CONSTR_ELEMENT(IS_SRV_SYS_PREF_CONF),
                            /* IS_SRV_SYS_PREF_CONF( Val )- Evaluates to TRUE iff
                            ** VAL includes the SS-Preference designation of
                            ** the current serving system */



  SD_CONSTR_ELEMENT(IS_CSFB_ACC_TERM_COMB), 
                              /* IS_CSFB_ACC_TERM_COMB - Evaluates to TRUE if 
                              ** acc_term & acc_reas combinations are valid*/
                                  /* 211 */
  SD_CONSTR_ELEMENT(IS_PREV_ORIG_MODE), 
                              /* IS_PREV_ORIG_MODE - Evaluates to TRUE if 
                              ** acc_term & acc_reas combinations are valid*/

  SD_CONSTR_ELEMENT(IS_ALL_SYS_PERM_AVOID_IN_LST),
                      	  /*IS_ALL_SYS_PERM_AVOID_IN_LST( REDIR_LST ) - Evaluates to
                      	  ** TRUE iff all the systems present in the REDIR_LST 
                      	  ** are under permanent avoidance */

  SD_CONSTR_ELEMENT(IS_PREF_UPDATE_REAS),
                            /* IS_PREF_UPDATE_REAS(  VAL ) - Evaluates to TRUE
                            ** iff VAL equals the pref update reason 
                            ** of the last preferred system changed. */
  SD_CONSTR_ELEMENT(IS_SS),
                            /* IS_SS(  VAL ) - Evaluates to TRUE
                            ** if VAL equals SS type. */

  SD_CONSTR_ELEMENT(IS_ACQ_SCAN_SCOPE_TYPE),
                            /* IS_SCAN_SCOPE_TYPE(  VAL ) - Evaluates to TRUE
                            ** if VAL equals SCAN_SCOPE_TYPE type. */

  SD_CONSTR_ELEMENT(MAX)

} SD_END_ENUM(sdssscr_con_e_type)

/* Everything other than the tagged enumeration will not be compiled
** if the default enum macro is disabled (aka generate string). */
#ifndef FEATURE_SD_ENABLE_DEBUG_STRINGS


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enums for ABSR CNT.
*/

typedef enum
{

  SDSSSCR_ABSR_CNT_NONE,
  /* Internal range check.
  */

  SDSSSCR_ABSR_CNT_LESS_THAN_NUM_ABSR_PER_BSR,
  /* Less than num ABSR per BSR.
  */

  SDSSSCR_ABSR_CNT_SAME_AS_NUM_ABSR_PER_BSR,
  /* Same as num ABSR per BSR.
  */

  SDSSSCR_ABSR_CNT_MAX
  /* Internal range check.
  */


} sdssscr_absr_cnt_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given script construct is a condition construct.
*/
#define SDSSSCR_CON_IS_COND( con ) \
                        BETWEEN( con, SDSSSCR_CON_1ST_COND, SDSSSCR_CON_MAX )


/* <EJECT> */
/*---------------------------------------------------------------------------
                       SS-Script Constructs' Parameters
---------------------------------------------------------------------------*/

/* Enumeration of acquisition types.
*/
typedef enum {

                            /* 0 */
  SDSSSCR_ACQ_ANY,          /* Try to fully acquire any system without regard
                            ** to PRL designation */

  SDSSSCR_ACQ_ALLOWED,      /* Try to fully acquire any system that is
                            ** allowed by the PRL and complies with the
                            ** current SS-Preference */

  SDSSSCR_ACQ_PREFERRED,    /* Try to fully acquire only a preferred PRL
                            ** system that complies with the current
                            ** SS-Preference */

  SDSSSCR_ACQ_BEST_PREF,    /* Try to fully acquire only a preferred PRL
                            ** system that complies with the current
                            ** SS-Preference in a two step process, as
                            ** follows:
                            ** 1. Step through the system record list/item
                            **    LIST (first to last) and try to acquire
                            **    systems that are referenced by LIST.
                            **    Whenever a system from the PRL is acquired,
                            **    continue with step 2.
                            **
                            ** 2. Try to acquire more preferred systems from
                            **    the GEO of the system that was acquired in
                            **    step 1 (most to acquired) */

                            /* 4 */
  SDSSSCR_ACQ_REG_BEST_PREF,  /* Try to fully acquire only a preferred PRL
                            ** system that complies with the current
                            ** SS-Preference in a three step process, as
                            ** follows:
                            ** 1. Step through the system record list/item
                            **    LIST (first to last) and try to acquire
                            **    systems that are referenced by LIST. If a
                            **    system is 3gpp, search for it without
                            **    attempting registration.
                            **    Whenever a system from the PRL is acquired,
                            **    continue with step 2.
                            **
                            ** 2. Try to acquire more preferred systems from
                            **    the GEO of the system that was acquired in
                            **    step 1 (most to acquired)

                               3. Register on most pref sys, if it is a 3gpp
                               ** system . For 3gpp2 registration is done with
                               ** acquisition
                               */

  SDSSSCR_ACQ_MORE_PREF,    /* Try to fully acquire only a system that is
                            ** more preferred than the idle system (i.e.,
                            ** SDSR_IDL_SYS) and complies with the current
                            ** SS-Preference */

  SDSSSCR_ACQ_MORE_PREF_THAN_SRV_SYS,
                            /* Try to fully acquire only a system that is
                            ** more preferred than the serving system (i.e.,
                            ** SDSR_SRV_SYS) and complies with the current
                            ** SS-Preference */

  SDSSSCR_ACQ_MORE_PREF_RECURSIVE,
                            /* Perform more-pref scan would take into account
                            ** the time restriction, etc. Also, the scan will
                            ** be recursive scan to the best preferred system.
                            **
                            ** This type is equivalent to BEST_PREF, except
                            ** that:
                            ** 1. it's triggered from BSR rather than OOS.
                            ** 2. it does not allow duplicated system scanned
                            **    during BSR, while BEST PREF allow duplicated
                            **    scan during OOS.
                            ** 3. it consider the time used in construct ACQ_TIME
                            **    while BEST_PREF does not.
                            */

  SDSSSCR_ACQ_NOT_LESS_PREF,/* Try to fully acquire only a system that is not
                            ** less preferred (upto roaming indicator) than
                            ** the idle system (i.e., SDSR_IDL_SYS) and
                            ** complies with the current SS-Preference
                            */

                            /* 9 */
  SDSSSCR_ACQ_NOT_LESS_PREF_OR_HOME,
                            /* Try to fully acquire only a system that is not
                            ** less preferred (upto roaming indicator) than
                            ** the idle system (i.e., SDSR_IDL_SYS) or is a
                            ** home system, and complies with the current
                            ** SS-Preference
                            */

  SDSSSCR_ACQ_SPECIFIC,     /* Try to fully acquire a specific system for
                            ** which the SID/NID is known (for example a
                            ** system that is included in a redirection)
                            ** without regard to the current SS-Preference or
                            ** PRL designation */

  SDSSSCR_ACQ_SPECIFIC_PRL, /* Try to fully acquire a specific system for
                            ** which the SID/NID is known (for example a
                            ** system that is included in a channel
                            ** assignment) without regard to the current
                            ** SS-Preference. The system must be allowed by
                            ** the PRL */

  SDSSSCR_ACQ_SPECIFIC_PREF,/* Try to fully acquire a specific system for
                            ** which the SID/NID is known (for example a
                            ** system that is included in a redirection)
                            ** and complies with the current SS-Preference */

                            /* 13 */
  SDSSSCR_ACQ_COLLOC_PREF,  /* Try to fully acquire a system that is
                            ** associated (collocated) with the SDSR_IDL_SYS
                            ** system and complies with the current
                            ** SS-Preference */

  SDSSSCR_ACQ_COLLOC_MORE_PREF,
                            /* Try to fully acquire only a system that is
                            ** associated (collocated) with the SDSR_IDL_SYS
                            ** system and complies with the current
                            ** SS-Preference and not less preferred than the
                            ** serving system */

  SDSSSCR_ACQ_COLLOC_MORE_PREF_RECURSIVE,
                            /* Perform more-pref scan would take into account
                            ** the time restriction, etc. Also, the scan will
                            ** be recursive scan to the best preferred system.
                            **
                            ** This type is equivalent to COLLOC_BEST_PREF,
                            ** except that:
                            ** 1. it's triggered from BSR rather than OOS.
                            ** 2. it does not allow duplicated system scanned
                            **    during BSR, while COLLOC_BEST PREF allow
                            **    duplicated scan during OOS.
                            ** 3. it consider the time used in construct ACQ_TIME
                            **    while COLLOC_BEST_PREF does not.
                            */

  SDSSSCR_ACQ_COLLOC_BEST_PREF,
                            /* Try to fully acquire only a preferred PRL
                            ** system that is associated (collocated) with
                            ** the SDSR_IDL_SYS system and complies with the
                            ** current SS-Preference in a two step process,as
                            ** follows:
                            ** 1. Step through the system record list/item
                            **    LIST (first to last) and try to acquire
                            **    systems that are referenced by LIST.
                            **    Whenever a system from the PRL is acquired,
                            **    continue with step 2.
                            **
                            ** 2. Try to acquire more preferred systems from
                            **    the GEO of the system that was acquired in
                            **    step 1 (most to acquired) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_ACQ_SAME_AS_LAST, /* Same as last acquisition attempt */

  /* 18 */
  SDSSSCR_ACQ_MAX
} sdssscr_acq_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of SS-Scripts that are being referenced by other SS-Scripts
** (as opposed to SS-Scripts that are being referenced by the default or
** exception scripts tables).
*/
typedef enum {

                              /* 0 */
  SDSSSCR_CLR_ACQ_LSTS,       /* Clear acq process related lists           */
  SDSSSCR_CLR_ACQ_SCHED_LSTS, /* Clear acq process and sched related lists */
  SDSSSCR_CLR_AVOID_LSTS,     /* Clear avoided lists */
  SDSSSCR_SRV_LOST_NORM,      /* Normal service lost script                */

                              /* 4 */
  SDSSSCR_SRV_LOST_NORM_SLNT, /* Normal service lost script - silent       */
  SDSSSCR_SRV_LOST_NORM_SRV_OFF_UNTIL_OH,
                              /* Normal service lost script - Keep srv off
                              ** until OH
                              */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                              /* 6 */
  SDSSSCR_SRV_LOST_SKIP,      /* Optimized service lost script             */
  SDSSSCR_SRV_LOST_SKIP_SLNT, /* Optimized service lost script - silent    */
  SDSSSCR_SRV_LOST_SKIP_ACQ,  /* Optimized service lost script - skip idle */
  SDSSSCR_SRV_LOST_SKIP_IDL,  /* Optimized service lost script - skip idle */

                              /* 10 */
  SDSSSCR_SRV_LOST_SKIP_SRV_OFF_UNTIL_OH,
                              /* Optimized service lost script - Keep srv off
                              ** until OH
                              */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SDSSSCR_SYS_ACC_FAIL_PRL,   /* Access failure (PRL system)               */
  SDSSSCR_SYS_ACC_FAIL_PRL_AMPS,
                              /*Access failure (PRL system, AMPS priority  */

                              /* 13 */
  SDSSSCR_SYS_ACC_FAIL_AVAIL, /* Access failure (Avail sys)                */
  SDSSSCR_SYS_ACC_FAIL_AVAIL_AMPS,
                              /* Access failure (Avail sys,AMPS priority   */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SDSSSCR_RESEL_JCDMA,        /* Dual-mode reselection script              */
  SDSSSCR_RESEL_DM,           /* Dual-mode reselection script              */
                              /* 17 */
  SDSSSCR_RESEL_NONE,         /* Reselection enter script */
  SDSSSCR_RESEL_ABSR,         /* ABSR reselection script */
  SDSSSCR_RESEL_INIT,         /* Initial reselection script                */
  SDSSSCR_RESEL_INIT_TO_NORM, /* Switching to normal reselection script */
                              /* 21 */
  SDSSSCR_RESEL_NORM,         /* Normal reselection script                 */
  SDSSSCR_RESEL_FULL_SRV,     /* Full service reselection script           */
  SDSSSCR_RESEL_HYBR,         /* SDSSSSCR_RESEL_HYBR                       */

  SDSSSCR_RESEL_GW_FULL_SRV,  /* GW only Full service reselection script   */
                              /* 25 */
  SDSSSCR_RESEL_FULL_SRV_MANUAL,
                              /* Full service reselection script  for manual
                              ** PLMN selection mode                       */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SDSSSCR_PWR_SAVE,           /* Power-save script                         */
  SDSSSCR_PWR_SAVE_JCDMA,     /* Power-save script for JCDMA               */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_PWR_SAVE_COLLOC_1SYS,
                              /* Power-save script for colloc orig, 1 sys  */
  SDSSSCR_PWR_SAVE_COLLOC,    /* Power-save script for collocted orig mode */
                              /* 30 */
  SDSSSCR_PWR_SAVE_COLLOC_DEEP,
                              /* Power-save script for collocted orig mode */
  SDSSSCR_PWR_SAVE_COLLOC_SHALLOW,
                              /* Power-save script for collocted mode      */
  SDSSSCR_PWR_SAVE_RX_GT_99,  /* Rx-power > -99dB during power-save script */

  SDSSSCR_PWR_SAVE_JCDMA_1_2_B,
                              /* Power-save script for JCDMA proc. 1-2, B  */
                              /* 34 */
  SDSSSCR_PWR_SAVE_JCDMA_1_3_B,
                              /* Power-save script for JCDMA proc. 1-3, B  */
  SDSSSCR_PWR_SAVE_OVERREACH_2_2,
                              /* Power-save for over-reach proc. 2-2       */
  SDSSSCR_PWR_SAVE_OVERREACH_2_3,
                              /* Power-save for over-reach proc. 2-3       */
  SDSSSCR_PWR_SAVE_GW_PWRUP,  /* Power-save after power up for GW target   */

                              /* 38 */
  SDSSSCR_PWR_SAVE_GW,        /* Power-save other than power up for GW
                              ** target                                    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_SRV_LOST_GW_EMERG_CALL_ORIG_PWRUP,
                              /* Service lost at pwr up during a E911 call
                              ** origination                               */
  SDSSSCR_SRV_LOST_GW_ORIG_PWRUP,
                              /* Srv lost after power up during call orig
                              ** for GW target                             */
  SDSSSCR_SRV_LOST_GW_EMERG_CALL_ORIG,
                              /* Service lost during a E911 Call orig.     */

                              /* 42 */
  SDSSSCR_SRV_LOST_GW_ORIG,   /* Srv lost during call orig for GW target   */
  SDSSSCR_SRV_LOST_GW_PWRUP,  /* Srv lost after power up for GW target     */
  SDSSSCR_SRV_LOST_GW,        /* Srv lost other than power up for GW target*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_HDR_ACQ_ROTATE,     /* HDR acquisitions, FULL followed by Micro  */
  SDSSSCR_HDR_ACQ_ROTATE_SRV_LOST,
                              /* SRV_LOST                                  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                              /* 47 */
  SDSSSCR_CDMA_ACQ_MICRO,     /* One CDMA Micro Acq  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_EOOS_UMTS
  SDSSSCR_GW_SET_PWR_SAVE_TIMER,
  #else
  SDSSSCR_RESERVED_9,         /* Reserved */
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  SDSSSCR_SRV_LOST_HDR_CDMA_HO,
                              /* Service lost while in HDR to CDMA Handoff */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_CHK_AND_CLR_ACQ_SYS,/* Check and clear acq sys                   */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_EOOS
  #error code not present
#else
  SDSSSCR_CALLED_RESERVED_12,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_13,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_14,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_15,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_16,              /* RESERVED */
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_EOOS
  #error code not present
#else
  SDSSSCR_CALLED_RESERVED_17,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_18,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_19,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_20,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_21,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_22,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_23,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_24,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_25,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_26,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_27,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_28,              /* RESERVED */
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SDSSSCR_SRV_LOST_LTE_CONN,

  #if defined (FEATURE_WRLF_SYSTEM_SEL)
  SDSSSCR_SRV_LOST_WCDMA_CONN_STAGE1,
  SDSSSCR_SRV_LOST_WCDMA_CONN_STAGE2,
  #else
  SDSSSCR_CALLED_RESERVED_29,              /* RESERVED */
  SDSSSCR_CALLED_RESERVED_30,              /* RESERVED */
  #endif /* FEATURE_WRLF_SYSTEM_SEL */


  SDSSSCR_ACQ_GW_LTD,             /* 71 */

  SDSSSCR_SRV_LOST_EMERG_MODE,    /* Service lost in emerg mode script */

  SDSSSCR_RESEL_LMTD_TO_HYBR_BSR, /* Script for Limited to HYBR BSR resel */

  SDSSSCR_RESEL_LMTD_TO_FULL,     /* Script for Limited to FULL resel */

                                  /* 75 */
  SDSSSCR_RESEL_NORM_SCAN,        /* BSR scan behavior */
  SDSSSCR_SCAN_MAP_SYSTEMS,       /* Scan MAP systems on service loss */
  SDSSSCR_BSR_START,              /* BSR timer behavior */
  SDSSSCR_LOC_BSR,                /* location based bsr */
  SDSSSCR_PWR_SAVE_BSR,           /* Go to PWR Save upon BSR fail */

                                  /* 80 */   
  SDSSSCR_GET_NET_BPLMN,          /* Get Net BPLMN */
  SDSSSCR_USER_GET_NET,           /* Called script User get net */

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  SDSSSCR_PWR_SAVE_HYBR2,
  SDSSSCR_SRV_LOST_HYBR2_PWRUP,   /* HYBR2 power-up script */
  SDSSSCR_PWR_SAVE_HYBR2_PWRUP,   /* HYBR2 power-save script */
  #else
  SDSSSCR_CALLED_RESERVED_32,
  SDSSSCR_CALLED_RESERVED_33,
  SDSSSCR_CALLED_RESERVED_34,
  #endif

  SDSSSCR_GWL_OPR_SYS_LOST_DSDS, /* gwl srv lost processing for DSDS */

  SDSSSCR_GWL_NO_SRV_LMTD_SYS_SCAN, /* no srv on gwl and attempt lmtd sys */

  SDSSSCR_GWL_NO_SRV_LMTD_SCAN, /* no gwl srv and attempt all lmtd systems */

  SDSSSCR_GWL_NO_SRV_LMTD_TRUE_SCAN, /* no gwl srv & attempt lmtd based on
                                        actual mode pref */
  SDSSSCR_OPERATOR_CAPABILITY_CHANGED,  /* Script for CDMA Operator Capability Changed*/
  
  SDSSSCR_ROAMING_PLMN_SCAN,
  
  SDSSSCR_ACQ_DB_SCAN_FIRST_CYCLE,            /* Script for ACQ DB Scan for 30s */

  SDSSSCR_ACQ_DB_SCAN_SECOND_CYCLE,            /* Script for ACQ DB Scan for 45s */

  SDSSSCR_ACQ_DB_SCAN_DEEP_SLEEP_CYCLE,            /* Script for ACQ DB Scan 60s */
  
  SDSSSCR_MORE_PREFERRED_DO_SCAN,  /* More preferred DO  scan */
  
  SDSSSCR_NON_SV_ACQ_DB_SCAN_MRU,  /* ACQ DB scans in Non SV configuration on MRU */

  SDSSSCR_CSG_APP_SERVICE_REQ,          /* Script to send csg app selection service req */

  SDSSSCR_HDR_OPTI_REDIAL_START,   /* Script to update the redial list during optimized
                                      silent redial */  

  SDSSSCR_SRV_LOST_NORM_SLNT_NON_MULTI_SIM, /* Srv Lost script non multi sim case */

  SDSSSCR_GWL_NO_SRV_PLMN_FOUND_LMTD_SYS_SCAN, /* no gwl srv , but plmn was found so trigger lmtd scan without E911 flag set */
  
  SDSSSCR_ACQ_DB_SCAN_PWR_UP,  /* ACQ DB scans at power up */
  
  SDSSSCR_ACQ_DB_SCAN_PWR_UP_3GPP,  /* ACQ DB scans at power up for 3GPP only SUB*/

  SDSSSCR_STACK_SYNC_UP_BSR_ACQ_ALL, /* Acq all for SRLTE or SVLTE cases */

  SDSSSCR_SRV_LOST_NORM_ACQ_MODE,    /* SRV_LOST_NORM script without NEW_ACQ_CNT*/
  
  SDSSSCR_SRLTE_NO_TRM_SCOPE_ACQDB, /* Set scan scope as Acqdb in SRLTE & srv lost due to no trm */

  SDSSSCR_SRLTE_SCOPE_FULL,           /* Set scan scope as Full in SRLTE */

  SDSSSCR_START_1XSIB8_SCAN,        /*  Acq 1x systems from SIB8_1X_NBR_LST */

  SDSSSCR_OPR_GW_EMERG_CALL_ORIG_LTD, /* Script for GW Em call orig in ltd */

  SDSSSCR_NO_SUBS_EMERG_MODE, /* emerg mode no subscription script */
  SDSSSCR_MAX

} sdssscr_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of schedule conditions
*/

typedef enum {

  SDSSSR_SCHED_COND_NONE = -1,         /* For range checking only          */

  SDSSSR_SCHED_COND_UNCONDITIONAL,     /* Un-conditional schedule          */

  SDSSSR_SCHED_COND_NOT_ON_HIGH_SCHED, /* Schedule only if Not on a high
                                       ** priority schedule                */

  SDSSSR_SCHED_COND_NOT_ON_SAME_SCHED, /* Schedule only if Not already on
                                       ** the same schedule                */

  SDSSSR_SCHED_COND_NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID,
                                       /* Schedule only if Not on equal or
                                       ** higher priority schedule         */

  SDSSSR_SCHED_COND_NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID_OR_FORBID,
                                       /* Schedule only if Not ( on equal or
                                       ** higher priority schedule or
                                       ** avoided or forbidden )*/

  SDSSSR_SCHED_COND_MAX                /* For range checking only          */

} sdsssr_sched_cond_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of reacquisition schedules.
*/
typedef enum {

  /* ------------------------------------
  ** Time based schedules (low priority).
  ** ------------------------------------
  */                                      /* 0 */
  SDSSSCR_SCHED_REACQ_TIME_LOW,           /* For range checking only */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Power-up.
  */
  SDSSSCR_SCHED_REACQ_PWR_UP,
  SDSSSCR_SCHED_REACQ_PWR_UP_1,
  SDSSSCR_SCHED_REACQ_PWR_UP_2,
  SDSSSCR_SCHED_REACQ_PWR_UP_3,           /* 4 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------
  ** Time based schedules (high priority).
  ** ------------------------------------
  */
  SDSSSCR_SCHED_REACQ_TIME_HIGH,          /* For range checking only */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire 1 time.
  */
  SDSSSCR_SCHED_REACQ_1_TIME,
  SDSSSCR_SCHED_REACQ_1_TIME_1,
  SDSSSCR_SCHED_REACQ_1_TIME_2,           /* 8 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire 2 times (consecutively).
  */
  SDSSSCR_SCHED_REACQ_2_TIMES,
  SDSSSCR_SCHED_REACQ_2_TIMES_1,
  SDSSSCR_SCHED_REACQ_2_TIMES_2,
  SDSSSCR_SCHED_REACQ_2_TIMES_3,          /* 12 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire every 1s.
  */
  SDSSSCR_SCHED_REACQ_EVERY_1S,
  SDSSSCR_SCHED_REACQ_EVERY_1S_1,
  SDSSSCR_SCHED_REACQ_EVERY_1S_2,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire every 2s.
  */
  SDSSSCR_SCHED_REACQ_EVERY_2S,           /* 16 */
  SDSSSCR_SCHED_REACQ_EVERY_2S_1,
  SDSSSCR_SCHED_REACQ_EVERY_2S_2,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire every 4s.
  */
  SDSSSCR_SCHED_REACQ_EVERY_4S,
  SDSSSCR_SCHED_REACQ_EVERY_4S_1,         /* 20 */
  SDSSSCR_SCHED_REACQ_EVERY_4S_2,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire every 8s.
  */
  SDSSSCR_SCHED_REACQ_EVERY_8S,
  SDSSSCR_SCHED_REACQ_EVERY_8S_1,
  SDSSSCR_SCHED_REACQ_EVERY_8S_2,         /* 24 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* System lost during operation.
  */
  SDSSSCR_SCHED_REACQ_SYS_LOST,
  SDSSSCR_SCHED_REACQ_SYS_LOST_1,
  SDSSSCR_SCHED_REACQ_SYS_LOST_2,
  SDSSSCR_SCHED_REACQ_SYS_LOST_3,         /* 28 */
  SDSSSCR_SCHED_REACQ_SYS_LOST_4,
  SDSSSCR_SCHED_REACQ_SYS_LOST_5,
  SDSSSCR_SCHED_REACQ_SYS_LOST_6,
  SDSSSCR_SCHED_REACQ_SYS_LOST_7,         /* 32 */
  SDSSSCR_SCHED_REACQ_SYS_LOST_8,
  SDSSSCR_SCHED_REACQ_SYS_LOST_9,
  SDSSSCR_SCHED_REACQ_SYS_LOST_10,
  SDSSSCR_SCHED_REACQ_SYS_LOST_11,        /* 36 */
  SDSSSCR_SCHED_REACQ_SYS_LOST_12,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* System lost during acquisition.
  */
  SDSSSCR_SCHED_REACQ_ACQ_LOST,
  SDSSSCR_SCHED_REACQ_ACQ_LOST_1,
  SDSSSCR_SCHED_REACQ_ACQ_LOST_2,         /* 40 */
  SDSSSCR_SCHED_REACQ_ACQ_LOST_3,
  SDSSSCR_SCHED_REACQ_ACQ_LOST_4,
  SDSSSCR_SCHED_REACQ_ACQ_LOST_5,
  SDSSSCR_SCHED_REACQ_ACQ_LOST_6,         /* 44 */
  SDSSSCR_SCHED_REACQ_ACQ_LOST_7,
  SDSSSCR_SCHED_REACQ_ACQ_LOST_8,
  SDSSSCR_SCHED_REACQ_ACQ_LOST_9,
  SDSSSCR_SCHED_REACQ_ACQ_LOST_10,        /* 48 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* AMPS system lost during operation.
  */
  SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST,
  SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_1,
  SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_2,
  SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_3,    /* 52 */
  SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_4,
  SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_5,
  SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_6,
  SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_7,    /* 56 */
  SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_8,
  SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_9,
  SDSSSCR_SCHED_REACQ_AMPS_SYS_LOST_10,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* AMPS system lost during acquisition.
  */
  SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST,      /* 60 */
  SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_1,
  SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_2,
  SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_3,
  SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_4,    /* 64 */
  SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_5,
  SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_6,
  SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_7,
  SDSSSCR_SCHED_REACQ_AMPS_ACQ_LOST_8,    /* 68 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -------------------------------------
  ** Count based schedules (low priority).
  ** -------------------------------------
  */
  SDSSSCR_SCHED_REACQ_CNT_LOW,            /* For range checking only */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -------------------------------------
  ** Count based schedules (high priority).
  ** -------------------------------------
  */
  SDSSSCR_SCHED_REACQ_CNT_HIGH,           /* For range checking only */


  /* Reacquire immidiately and alternate with 1 system there after.
  */
  SDSSSCR_SCHED_REACQ_EVERY_1,
  SDSSSCR_SCHED_REACQ_EVERY_1_1,          /* 72 */
  SDSSSCR_SCHED_REACQ_EVERY_1_2,
  SDSSSCR_SCHED_REACQ_EVERY_1_3,
  SDSSSCR_SCHED_REACQ_EVERY_1_4,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire immidiately and alternate with 2 systems there after.
  */
  SDSSSCR_SCHED_REACQ_EVERY_2,            /* 76 */
  SDSSSCR_SCHED_REACQ_EVERY_2_1,
  SDSSSCR_SCHED_REACQ_EVERY_2_2,
  SDSSSCR_SCHED_REACQ_EVERY_2_3,
  SDSSSCR_SCHED_REACQ_EVERY_2_4,          /* 80 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire immidiately and alternate with 3 systems there after.
  */
  SDSSSCR_SCHED_REACQ_EVERY_3,
  SDSSSCR_SCHED_REACQ_EVERY_3_1,
  SDSSSCR_SCHED_REACQ_EVERY_3_2,
  SDSSSCR_SCHED_REACQ_EVERY_3_3,          /* 84 */
  SDSSSCR_SCHED_REACQ_EVERY_3_4,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire immidiately and alternate with 4 systems there after.
  */
  SDSSSCR_SCHED_REACQ_EVERY_4,
  SDSSSCR_SCHED_REACQ_EVERY_4_1,
  SDSSSCR_SCHED_REACQ_EVERY_4_2,          /* 88 */
  SDSSSCR_SCHED_REACQ_EVERY_4_3,
  SDSSSCR_SCHED_REACQ_EVERY_4_4,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reacquire immidiately and alternate with 8 systems there after.
  */
  SDSSSCR_SCHED_REACQ_EVERY_8,
  SDSSSCR_SCHED_REACQ_EVERY_8_1,          /* 92 */
  SDSSSCR_SCHED_REACQ_EVERY_8_2,
  SDSSSCR_SCHED_REACQ_EVERY_8_3,
  SDSSSCR_SCHED_REACQ_EVERY_8_4,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Emergency system lost/access failure.
  */
  SDSSSCR_SCHED_REACQ_EMERG_LOST,         /* 96 */
  SDSSSCR_SCHED_REACQ_EMERG_LOST_1,
  SDSSSCR_SCHED_REACQ_EMERG_LOST_2,
  SDSSSCR_SCHED_REACQ_EMERG_LOST_3,
  SDSSSCR_SCHED_REACQ_EMERG_LOST_4,       /* 100 */
  SDSSSCR_SCHED_REACQ_EMERG_LOST_5,
  SDSSSCR_SCHED_REACQ_EMERG_LOST_6,
  SDSSSCR_SCHED_REACQ_EMERG_LOST_7,
  SDSSSCR_SCHED_REACQ_EMERG_LOST_8,       /* 104 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reaquire immediately and
  ** alternate with one system for 3 times followed by three systems for 1
  ** time there after.
  */
  SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST,
  SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_1,
  SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_2,
  SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_3, /* 108 */
  SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_4,
  SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_5,
  SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_6,
  SDSSSCR_SCHED_REACQ_GW_FULL_ACQ_LOST_7, /* 112 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reaquire after one system and
  ** alternate with one system for 3 times followed by three systems for 1
  ** time there after.
  */
  SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST,
  SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_1,
  SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_2,
  SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_3,  /* 116 */
  SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_4,
  SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_5,
  SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_6,
  SDSSSCR_SCHED_REACQ_GW_LTD_ACQ_LOST_7,  /* 120 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reaquire one system 4 times and alternate with one system there after.
  */
  SDSSSCR_SCHED_REACQ_GW_FULL_ORIG,
  SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_1,
  SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_2,
  SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_3,     /* 124 */
  SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_4,
  SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_5,
  SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_6,
  SDSSSCR_SCHED_REACQ_GW_FULL_ORIG_7,     /* 128 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_SCHED_REACQ_LAST,               /* To be used for range checking only */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -------------------------------------
  ** All the avoid construct should start
  ** after this.
  ** -------------------------------------
  */
  SDSSSCR_SCHED_AVOID,                     /* Avoid system for a specified time */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_SCHED_AVOID_UNTIL_ACQ_FAIL,     /* Avoid system for a specified time or
                                          ** ACQ_FAIL is encountered.
                                          */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_SCHED_AVOID_UNTIL_ACQ_FAIL_DUR_CALL_ORIG, /* 132 */
                                          /* Avoid system for a specified time or
                                          ** ACQ_FAIL is encountered during call
                                          ** origination.
                                          */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_SCHED_AVOID_MAP_UNTIL_ACC_SUCCESS,
                                          /* Avoid system for a specified time or
                                          ** a successful access is encountered.
                                          */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_SCHED_AVOID_UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL,
                                          /* Avoid system for a specified time or
                                          ** ACQ_FAIL is encountered during
                                          ** optimized silent redial
                                          */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_SCHED_AVOID_CANCEL_ALL,         /* Cancel all the avoidances listed
                                          ** above
                                          */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                          /* 136 */
  SDSSSCR_SCHED_AVOID_LAST,               /* To be used for range checking only */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_SCHED_MAX
} sdssscr_sched_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given schedule is time based, low priority.
*/
#define SDSSSCR_SCHED_IS_TIME_LOW( sched ) \
                        /*lint -e641 */    \
                        ( ((sched) > SDSSSCR_SCHED_REACQ_TIME_LOW) && \
                          ((sched) < SDSSSCR_SCHED_REACQ_TIME_HIGH) ) \
                        /*lint +e641 */ /* Convertion enum to int */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given schedule is time based, high priority.
*/
#define SDSSSCR_SCHED_IS_TIME_HIGH( sched ) \
                        /*lint -e641 */    \
                        ( ((sched) > SDSSSCR_SCHED_REACQ_TIME_HIGH) && \
                          ((sched) < SDSSSCR_SCHED_REACQ_CNT_LOW) )
                        /*lint +e641 */ /* Convertion enum to int */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given schedule is time based.
*/
#define SDSSSCR_SCHED_IS_TIME( sched ) \
                        ( SDSSSCR_SCHED_IS_TIME_LOW( sched ) || \
                          SDSSSCR_SCHED_IS_TIME_HIGH( sched ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given schedule is count based, low priority.
*/
#define SDSSSCR_SCHED_IS_CNT_LOW( sched ) \
                        /*lint -e641 */    \
                        ( ((sched) > SDSSSCR_SCHED_REACQ_CNT_LOW) && \
                          ((sched) < SDSSSCR_SCHED_REACQ_CNT_HIGH) )
                        /*lint +e641 */ /* Convertion enum to int */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given schedule is count based, high priority.
*/
#define SDSSSCR_SCHED_IS_CNT_HIGH( sched ) \
                        /*lint -e641 */    \
                        ( ((sched) > SDSSSCR_SCHED_REACQ_CNT_HIGH) && \
                          ((sched) < SDSSSCR_SCHED_REACQ_LAST) )
                        /*lint +e641 */ /* Convertion enum to int */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given schedule is count based.
*/
#define SDSSSCR_SCHED_IS_CNT( sched ) \
                        ( SDSSSCR_SCHED_IS_CNT_LOW( sched ) || \
                          SDSSSCR_SCHED_IS_CNT_HIGH( sched ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given schedule is low priority.
*/
#define SDSSSCR_SCHED_IS_LOW( sched ) \
                        ( SDSSSCR_SCHED_IS_TIME_LOW( sched ) || \
                          SDSSSCR_SCHED_IS_CNT_LOW( sched ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given schedule is high priority.
*/
#define SDSSSCR_SCHED_IS_HIGH( sched ) \
                        ( SDSSSCR_SCHED_IS_TIME_HIGH( sched ) || \
                          SDSSSCR_SCHED_IS_CNT_HIGH( sched ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define SDSSSCR_SCHED_IS_AVOID( sched ) \
                        /*lint -e641 */    \
                         (( (sched) >= SDSSSCR_SCHED_AVOID ) &&  \
                          ( (sched) < SDSSSCR_SCHED_AVOID_LAST ))
                        /*lint +e641 */ /* Convertion enum to int */



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Definition of magic values to be used in conjunction with the START_POS
** parameter of the ACQ_CNT() and ACQ_TIME() acquisition constructs.
*/
#define SDSSSCR_FULL_SYS  ((uint16) (-1))  /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_ACQ_FULL_SYS */

#define SDSSSCR_MINI_SYS  ((byte) (-2))  /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_ACQ_MINI_SYS */

#define SDSSSCR_MICRO_SYS ((uint16) (-3))  /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_ACQ_MICRO_SYS */

#define SDSSSCR_FULL2_SYS ((uint16) (-4))  /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_ACQ_FULL2_SYS */

#define SDSSSCR_MINI2_SYS ((uint16) (-5))  /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_ACQ_MINI2_SYS */

#define SDSSSCR_DEEP_SYS  ((uint16) (-6))  /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_DEEP_SYS */

#define SDSSSCR_DEEP2_SYS  ((byte) (-7)) /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_DEEP2_SYS */

#define SDSSSCR_SHALLOW_SYS ((uint16)(-8)) /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_SHALLOW_SYS */

#define SDSSSCR_SHALLOW2_SYS ((byte)(-9))/* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_SHALLOW2_SYS */

#define SDSSSCR_EMERG_SYS ((uint16) (-10)) /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_EMERG_SYS */

#define SDSSSCR_EOOS_FULL_SYS ((uint16) (-11))  /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_ACQ_EOOS_FULL_SYS */

#define SDSSSCR_MICRO2_SYS ((uint16) (-12))  /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_ACQ_MICRO_SYS */

#define SDSSSCR_FULL_OSR_SYS  ((uint16) (-13))
                                         /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_ACQ_FULL_OSR_SYS */

#define SDSSSCR_FULL_AOOS_SYS  ((uint16) (-14))
                                         /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_ACQ_FULL_AOOS_SYS */

#define SDSSSCR_MICRO_ABSR_SYS ((uint16) (-15))
                                         /* Start acq from the system that is
                                         ** one after the system that equal
                                         ** to SDSR_ACQ_MICRO_ABSR_SYS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Definition of a magic value to be used in conjunction with the TIME
** parameter of the AVOID() constructs.
*/
#define SDSSSCR_AVOID_UNTIL_NEW_ACQ   ((word) (-1))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Definition of a magic value to be used in conjunction with the TIME
** parameter of the CONT() constructs.
*/
#define SDSSSCR_UNTIL_NEXT_EVENT                       ((word) (-1))

#define SDSSSCR_UNTIL_NEXT_EVENT_WITH_CURR_TIMER       ((word) (-2))

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_EOOS
#error code not present
#endif
/* Enumeration of continue timer type
*/
typedef enum {

  SDSSSCR_CONT_TYPE_NONE,        /* For internal use only */

  SDSSSCR_CONT_TYPE_LESS_PREF,
                                 /* A timer indicates that the system
                                 ** reselection should be started after
                                 ** TIME passed since this timer is started.
                                 ** The TIME is a NV item,
                                 ** SDSSSCR_NV_ITEM_TMR_HDR_RESEL.
                                 **
                                 ** Initialization: reset at power up.
                                 **
                                 ** Start condition: controlled by scripts.
                                 **
                                 **   Update with uptime when it acquires
                                 **   the less preferred channel at powerup,
                                 **   or when it just moves from a more
                                 **   preferred to a less preferred system.
                                 **
                                 ** Reset condition: controlled by scripts.
                                 **
                                 **   Whenever the phone acquires the more
                                 **   preferred channel. or after the system
                                 **   reselection is started.
                                 **
                                 **
                                 */

  SDSSSCR_CONT_TYPE_HOLD_RESEL,  /* A timer indicates that the system
                                 ** reselection should be hold until TIME
                                 ** passed since this timer is started.
                                 ** The TIME is a NV item,
                                 ** SDSSSCR_NV_ITEM_TMR_HDR_RESEL_HOLD_RESEL.
                                 **
                                 ** Initialization: reset at power up
                                 **
                                 ** Start condition: controlled by scripts
                                 **
                                 **   Start when it exits HDR traffic channel
                                 **
                                 ** Reset condition: controlled by scripts
                                 **
                                 **   After the system reselection is started
                                 **
                                 */

  SDSSSCR_CONT_TYPE_CALL_END,    /* A timer indicates that the system
                                 ** reselection should be started after TIME
                                 ** passed since this timer is started.
                                 ** The TIME is a NV item
                                 ** SDSSSCR_NV_ITEM_TMR_HDR_RESEL_HOLD_CALL_END
                                 **
                                 ** Initialization: reset at power up
                                 **
                                 ** Start condition: controlled by scripts
                                 **
                                 **   Start when the MO call is ended
                                 **
                                 ** Reset condition: controlled by scripts
                                 **
                                 **   After the system reselection is started
                                 **
                                 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_CONT_TYPE_MAX

}sdssscr_cont_type_e_type;


/* Enumeration of continue sleep timer type per RAT (telescopic)
*/
typedef enum {

  SDSSSCR_CONT_TYPE_RAT_SLEEP_NONE,        /* For internal use only */

  SDSSSCR_CONT_RAT_GW_SLEEP_TYPE,

  SDSSSCR_CONT_RAT_1X_SLEEP_TYPE,

  SDSSSCR_CONT_RAT_AUTO_SLEEP_TYPE,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_CONT_TYPE_RAT_SLEEP_MAX

}sdssscr_cont_type_rat_type;

/* Enumeration of continue timer value
*/
typedef enum {

  SDSSSCR_CONT_VAL_ZERO,  /* Value 0 */

  SDSSSCR_CONT_VAL_UPTIME_IF_ZERO,
                          /* Value is uptime if the current value is zero.*/

  SDSSSCR_CONT_VAL_UPTIME,/* Value is uptime */


  SDSSSCR_CONT_VAL_INC_BY_BCMCS_LEN,
                          /* Pause the timer till BCMCS monitoring is end.*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_CONT_VAL_MAX

}sdssscr_cont_val_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Definition of the maximum loop back count
** The count is 1, because the condition checks for greater than so
** in effect the loop will be executed atmost twice.
*/

#define SDSSSCR_MAX_LOOP_BACK_CNT               1



/* <EJECT> */
/*---------------------------------------------------------------------------
                 Condition SS-Script Constructs' Parameters
---------------------------------------------------------------------------*/

/* Enumeration of Operating Mode combinations.
*/
typedef enum {

  SDSSSCR_OPR_MODE_VAL_ONLINE       = BM( SDSS_OPR_MODE_ONLINE ),
                                      /* Online (CDMA/AMPS/HDR) */

  SDSSSCR_OPR_MODE_VAL_OFFLINE_CDMA = BM( SDSS_OPR_MODE_OFFLINE_CDMA ),
                                      /* Offline CDMA */

  SDSSSCR_OPR_MODE_VAL_OFFLINE_AMPS = BM( SDSS_OPR_MODE_OFFLINE_AMPS ),
                                      /* Offline AMPS */

  SDSSSCR_OPR_MODE_VAL_OFFLINE_ANY  = BM( SDSS_OPR_MODE_OFFLINE_CDMA ) | \
                                      BM( SDSS_OPR_MODE_OFFLINE_AMPS ),
                                      /* Offline (CDMA or AMPS) */

  SDSSSCR_OPR_MODE_VAL_PWR_DOWN     = BM( SDSS_OPR_MODE_PWR_DOWN ),
                                      /* Power-off, reset or LPM */

  SDSSSCR_OPR_MODE_VAL_MAX
} sdssscr_opr_mode_val_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of pref reason combinations.
*/
typedef enum {

  SDSSSCR_PREF_REAS_VAL_USER            = BM( SD_SS_PREF_REAS_USER ),
  SDSSSCR_PREF_REAS_VAL_ORIG_START_CS   = BM( SD_SS_PREF_REAS_ORIG_START_CS ),
  SDSSSCR_PREF_REAS_VAL_REDIAL_NORM     = BM( SD_SS_PREF_REAS_REDIAL_NORM ),
  SDSSSCR_PREF_REAS_VAL_REDIAL_OPTI     = BM( SD_SS_PREF_REAS_REDIAL_OPTI ),
  SDSSSCR_PREF_REAS_VAL_ORIG_END        = BM( SD_SS_PREF_REAS_ORIG_END ),

  SDSSSCR_PREF_REAS_VAL_AVOID_SYS       = BM( SD_SS_PREF_REAS_AVOID_SYS ),
  SDSSSCR_PREF_REAS_VAL_USER_RESEL      = BM( SD_SS_PREF_REAS_USER_RESEL ),
  SDSSSCR_PREF_REAS_VAL_ORIG_START_PS   = BM( SD_SS_PREF_REAS_ORIG_START_PS ),
  SDSSSCR_PREF_REAS_VAL_ORIG_START      = BM( SD_SS_PREF_REAS_ORIG_START_CS ) |
                                          BM( SD_SS_PREF_REAS_ORIG_START_PS ),
  /* Origination any call, end of any call */
  SDSSSCR_PREF_REAS_VAL_ORIG_START_OR_END
                                        = BM( SD_SS_PREF_REAS_ORIG_START_CS ) |
                                          BM( SD_SS_PREF_REAS_ORIG_START_PS ) |
                                          BM( SD_SS_PREF_REAS_ORIG_END ),

  SDSSSCR_PREF_REAS_VAL_ORIG_RESUME     = BM( SD_SS_PREF_REAS_ORIG_RESUME ),
  SDSSSCR_PREF_REAS_VAL_CALL_ORIG       = BM( SD_SS_PREF_REAS_ORIG_START_CS ) |
                                          BM( SD_SS_PREF_REAS_ORIG_START_PS ) |
                                          BM( SD_SS_PREF_REAS_REDIAL_NORM ) |
                                          BM( SD_SS_PREF_REAS_REDIAL_OPTI)  |
                                          BM( SD_SS_PREF_REAS_ORIG_RESUME),

  SDSSSCR_PREF_REAS_VAL_PSEUDO_ONLINE   = BM( SD_SS_PREF_REAS_PSEUDO_ONLINE ),

  SDSSSCR_PREF_REAS_VAL_HDR_CDMA_HO     = BM( SD_SS_PREF_REAS_HDR_CDMA_HO ),

  SDSSSCR_PREF_REAS_VAL_STACK_SYNC_UP   = BM( SD_SS_PREF_REAS_STACK_SYNC_UP ),

  SDSSSCR_PREF_REAS_VAL_MULTIMODE_SCAN  = BM( SD_SS_PREF_REAS_MULTIMODE_SCAN ),

  SDSSSCR_PREF_REAS_VAL_STACK_SYNC_UP_BSR
                                        = BM(SD_SS_PREF_REAS_STACK_SYNC_UP_BSR),

  SDSSSCR_PREF_REAS_VAL_3GPP_PS_CALL_CONNECT = BM(SD_SS_PREF_REAS_3GPP_PS_CALL_CONNECT),

  SDSSSCR_PREF_REAS_VAL_MAX             = BM(SD_SS_PREF_REAS_MAX)

} sdssscr_pref_reas_val_e_type;


/* Enumeration of srv status combinations.
*/
typedef enum {

  SDSSSCR_SRV_STATUS_VAL_NO_SRV          = BM( SYS_SRV_STATUS_NO_SRV ) | \
                                           BM( SYS_SRV_STATUS_NO_SRV_INTERNAL ),
  SDSSSCR_SRV_STATUS_VAL_PWR_SAVE        = BM( SYS_SRV_STATUS_PWR_SAVE ),
  SDSSSCR_SRV_STATUS_VAL_NO_SRV_ANY      = BM( SYS_SRV_STATUS_PWR_SAVE ) | \
                                           BM( SYS_SRV_STATUS_NO_SRV ) | \
                                           BM( SYS_SRV_STATUS_NO_SRV_INTERNAL ),

  SDSSSCR_SRV_STATUS_VAL_LIMITED         = BM( SYS_SRV_STATUS_LIMITED ),
  SDSSSCR_SRV_STATUS_VAL_LIMITED_REG     = BM( SYS_SRV_STATUS_LIMITED_REGIONAL ),
  SDSSSCR_SRV_STATUS_VAL_LIMITED_ANY     = BM( SYS_SRV_STATUS_LIMITED_REGIONAL ) |
                                           BM( SYS_SRV_STATUS_LIMITED ),

  SDSSSCR_SRV_STATUS_VAL_SRV             = BM( SYS_SRV_STATUS_SRV ),
  SDSSSCR_SRV_STATUS_VAL_SRV_ANY         = BM( SYS_SRV_STATUS_SRV ) | \
                                           BM( SYS_SRV_STATUS_LIMITED ) | \
                                           BM( SYS_SRV_STATUS_LIMITED_REGIONAL ),

  SDSSSCR_SRV_STATUS_VAL_MAX
} sdssscr_srv_status_val_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of srv domain combinations.
*/
typedef enum {

  SDSSSCR_SRV_DOMAIN_VAL_NO_SRV          = BM( SYS_SRV_DOMAIN_NO_SRV ),

  SDSSSCR_SRV_DOMAIN_VAL_CS              = BM( SYS_SRV_DOMAIN_CS_ONLY ) |
                                           BM( SYS_SRV_DOMAIN_CS_PS),

  SDSSSCR_SRV_DOMAIN_VAL_PS              = BM( SYS_SRV_DOMAIN_PS_ONLY ) |
                                           BM( SYS_SRV_DOMAIN_CS_PS),

  SDSSSCR_SRV_DOMAIN_VAL_CS_OR_PS        = BM( SYS_SRV_DOMAIN_CS_ONLY ) |
                                           BM( SYS_SRV_DOMAIN_PS_ONLY ) |
                                           BM( SYS_SRV_DOMAIN_CS_PS),

  SDSSSCR_SRV_DOMAIN_VAL_CS_AND_PS       = BM( SYS_SRV_DOMAIN_CS_PS ),

  SDSSSCR_SRV_DOMAIN_VAL_MAX
} sdssscr_srv_domain_val_e_type;



/* Enumeration of srv mode
*/
typedef enum {

  SDSSSCR_SRV_MODE_VAL_NO_SRV          = 0,

  SDSSSCR_SRV_MODE_VAL_CDMA            = BM( SD_MODE_CDMA ),

  SDSSSCR_SRV_MODE_VAL_GSM             = BM( SD_MODE_GSM ),

  SDSSSCR_SRV_MODE_VAL_HDR             = BM( SD_MODE_HDR ),

  SDSSSCR_SRV_MODE_VAL_WCDMA           = BM( SD_MODE_WCDMA ),

  SDSSSCR_SRV_MODE_VAL_GPS             = BM( SD_MODE_GPS ),

  SDSSSCR_SRV_MODE_VAL_LTE             = BM( SD_MODE_LTE ),


  SDSSSCR_SRV_MODE_VAL_MAX

} sdssscr_srv_mode_val_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of  SIM states.
*/
typedef enum {

  SDSSSCR_SIM_STATE_NONE  = -1, /* Internal SD use */

  SDSSSCR_SIM_STATE_NOT_AVAILABLE, /* SIM state not available */

  SDSSSCR_SIM_STATE_AVAILABLE,  /* SIM state available */

  SDSSSCR_SIM_STATE_CS_INVALID, /* SIM state is invalid for CS call */

  SDSSSCR_SIM_STATE_PS_INVALID, /* SIM state is invalid for PS call */

  SDSSSCR_SIM_STATE_CS_PS_INVALID, /* SIM state is invalid for CS,PS calls */

  SDSSSCR_SIM_STATE_MAX     /* Internal SD use */

} sdssscr_sim_state_e_type;
/* Enumeration of reselection state combinations.
*/

typedef enum {

  SDSSSCR_RESEL_STT_NONE                 = BM( SDSS_RESEL_STT_NONE ),
                                           /* Not doing reselection
                                           */
  SDSSSCR_RESEL_STT_WAIT                 = BM( SDSS_RESEL_STT_WAIT ),
                                           /* Waiting for next reselection
                                           ** period
                                           */
  SDSSSCR_RESEL_STT_SRCH                 = BM( SDSS_RESEL_STT_SRCH ),
                                           /* Searching for more preferred
                                           ** systems full srv
                                           */
  SDSSSCR_RESEL_STT_REACQ                = BM( SDSS_RESEL_STT_REACQ ),
                                           /* Reacquiring the serving system
                                           */
  SDSSSCR_RESEL_STT_SS_PREF              = BM( SDSS_RESEL_STT_SS_PREF ),
                                           /* Reselection after SS-Pref change
                                           */
  SDSSSCR_RESEL_STT_RELEASE              = BM( SDSS_RESEL_STT_RELEASE ),
                                           /* Reselection after call is
                                           ** released
                                           */
  SDSSSCR_RESEL_STT_SETUP_SRCH           = BM( SDSS_RESEL_STT_SETUP_SRCH ),
                                           /* Setup state before SRCH
                                           */
  SDSSSCR_RESEL_STT_ABSR                 = BM( SDSS_RESEL_STT_ABSR ),
                                           /* Searching for more preferred
                                           ** avoided systems full srv
                                           */
  SDSSSCR_RESEL_STT_SRCH_ANY             = BM( SDSS_RESEL_STT_SETUP_SRCH )|
                                           BM( SDSS_RESEL_STT_SRCH ) |
                                           BM( SDSS_RESEL_STT_REACQ ) |
                                           BM( SDSS_RESEL_STT_ABSR) ,
                                           /* Doing any reselection searches
                                           */
  SDSSSCR_RESEL_STT_HYBR_BSR             = BM( SDSS_RESEL_STT_HYBR_BSR),
                                           /* Searching for Hybr systems while
                                           ** limited srv on main
                                           */
  SDSSSCR_RESEL_STT_MORE_PREF_HDR_SCAN    = BM (SDSS_RESEL_STT_MORE_PREF_HDR_SCAN),    
                                           /* Searching for more preferred DO systems 
                                           */
  SDSSSCR_RESEL_STT_MAX                  = BM( SDSS_RESEL_STT_MAX )
                                           /* INTERNAL SD USE
                                           */
} sdssscr_resel_stt_val_e_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of eCall mode combinations.
*/
typedef enum {

  SDSSSCR_ECALL_MODE_NONE        = BM( SDSS_ECALL_MODE_NONE ),

  SDSSSCR_ECALL_MODE_NORMAL      = BM( SDSS_ECALL_MODE_NORMAL),
  SDSSSCR_ECALL_MODE_ECALL_ONLY  = BM( SDSS_ECALL_MODE_ECALL_ONLY ),

  SDSSSCR_ECALL_MODE_MAX
} sdssscr_ecall_mode_val_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of boolean conditions.
*/
typedef enum {

  SDSSSCR_COND_REDIR_VALID,         /* Redirection is valid for this target*/
  SDSSSCR_COND_REDIR_RIF,           /* Redirection RETURN_IF_FAILED = 1 */
  SDSSSCR_COND_REDIR_WILDCARD_SID,  /* Redirection SID = wildcard-SID */
  SDSSSCR_COND_REDIR_WILDCARD_NID,  /* Redirection NID = wildcard-NID */
  SDSSSCR_COND_REDIR_IGNORE_CDMA,   /* Redirection IGNORE_CDMA = 1 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_COND_CAP_AMPS,            /* MS is capable of AMPS */
  SDSSSCR_COND_CAP_CDMA_CELL,       /* MS is capable of CDMA-Cellular */
  SDSSSCR_COND_CAP_CDMA_PCS,        /* MS is capable of CDMA-PCS  */
  SDSSSCR_COND_CAP_CDMA_CELL_NO_PCS,/* MS is capable of CDMA-Cellular and not
                                    ** capable of CDMA-PCS */
  SDSSSCR_COND_CAP_CDMA_PCS_NO_CELL,/* MS is capable of CDMA-PCS and not
                                    ** capable of CDMA-Cellular */
  SDSSSCR_COND_CAP_GSM,             /* MS is capable of GSM  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_COND_CDMA_LOCKED,         /* MS is CDMA locked until power cycle */
  SDSSSCR_COND_SRDA,                /* Silent redial with AMPS is enabled */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_COND_ORIG_MODE_NORM_CNG,  /* Normal origination mode change */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_COND_FEATURE_T53,         /* FEATURE_T53 is on */
  SDSSSCR_COND_FEATURE_JCDMA,       /* FEATURE_JCDMA is on */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_COND_TRUE_MODE_PREF_GSM,  /* True mode_pref has GSM */
  SDSSSCR_COND_SRV_DOMAIN_PREF_CS_PS,
                                    /* Service domain pref is CS + PS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_COND_CAP_3GPP_PREF_3GPP_ONLY, /* MS is 3GPP capable and mode preference
                                        ** is either (GSM or WCDMA or TD-SCDMA)
                                        ** or LTE on main stack.
                                        */

  SDSSSCR_COND_LTE_CONNECTED_MODE,    /* UE is in LTE Connected Mode
                                      **
                                      */
  #if defined (FEATURE_WRLF_SYSTEM_SEL)
  SDSSSCR_COND_WCDMA_CONNECTED_MODE,  /* UE is in WCDMA Connected Mode
                                      **
                                      */
  SDSSSCR_COND_WCDMA_SUITABLE_SEARCH, /* UE is in WCDMA Connected mode suitable search
                                      **
                                      */
  #endif /* FEATURE_WRLF_SYSTEM_SEL */
  SDSSSCR_COND_1X_3GPP_MODE_PREF,    /* Phone is set to CDMA + TGWL mode */
  SDSSSCR_COND_3GPP_MODE_PREF,       /* Phone pref contain TGWL mode */
  SDSSSCR_COND_CAP_HDR_PREF_HDR_ONLY,
                                    /* Phone is set to HDR only */

  SDSSSCR_COND_CAP_CDMA_HDR_PREF_CDMA_HDR_INCL,
                                    /* Phone mode has CDMA and HDR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                    /* Uniform Scan OOS is defined for 1X */
  SDSSSCR_COND_FEATURE_EOOS_UMTS,
                                    /* EOOS feature is ON for UMTS */

  SDSSSCR_COND_FEATURE_EOOS,        /* EUOOS is defined for either 1X or UMTS
                                    ** or both */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_COND_NO_HYBR_OPR,         /* Phone is not in hybrid operation */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_COND_FALSE,               /* Special condition which evaluates to
                                    ** FALSE */

  SDSSSCR_COND_SIM_DOMAIN_VALID,    /* SIM is valid for current selected
                                    ** domain */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_COND_CAP_3GPP_PREF_3GPP_TRGT, /* MS is 3GPP capable and mode preference
                                      ** is either (GSM or WCDMA or TD-SCDMA) or LTE for
                                      ** overall UE.
                                      */

  SDSSSCR_COND_MODE_PREF_NONE,       /* Phone's mode pref = NONE */

  SDSSSCR_COND_CAMPED_MODE_SET,     /* UE operating in CAMP ONLY mode. */

  SDSSSCR_COND_SWITCH_SVLTE,         /* UE is switching the SV opr. to SVLTE */

  SDSSSCR_COND_SWITCH_CSFB,          /* UE is switching the SV opr. to CSFB */

  
  SDSSSCR_COND_RLF_DURING_EMERG,     /* UE is in LTE Connected Mode
                                     */
  SDSSSCR_COND_CSG_SEARCH,       /* UE is in CSG search */  
  
  SDSSSCR_COND_CSG_SELECTION,    /* UE is in CSG cell selection */

  SDSSSCR_COND_EHRPD_SR_ENABLED, /*  EHRPD silent redial feature is enabled */
  SDSSSCR_COND_BUF_INT_SRV_LOST,    /* If int srv lost to be buffered*/

  SDSSSCR_COND_LTE_EMERG_REDIAL , /* Emerg call needs to redialed with LTE inlcuded
								      used to new service req with LTE during OPR state*/

  SDSSSCR_COND_SRV_DOMAIN_PREF_CS_ONLY, /*Check for SRV Domain Pref as CS_ONLY*/

  SDSSSCR_COND_MAX
} sdssscr_cond_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of list count conditions.
*/
typedef enum {

  SDSSSCR_CNT_COND_EXACTLY,       /* List contains exactly CNT elements */
  SDSSSCR_CNT_COND_OR_MORE,       /* List contains CNT or more elements */
  SDSSSCR_CNT_COND_OR_LESS,       /* List contains CNT or less elements */

  SDSSSCR_CNT_COND_MAX
} sdssscr_cnt_cond_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of list count inclusions.
*/
typedef enum {

  SDSSSCR_CNT_INC_ALL,        /* Count all systems */

  SDSSSCR_CNT_INC_COMPLY,     /* Count only systems that comply with the
                              ** target capabilities and current mode and
                              ** band preference */

  SDSSSCR_CNT_INC_AVAIL,      /* Count only systems that are available for
                              ** acquisition attempts. A system that is
                              ** available for acquisition attempts is a
                              ** system that:
                              **   1. Complies with the target capabilities
                              **      and current mode and band preference.
                              **   2. Currently is not marked to be avoided
                              **      from acquisition attempts.
                              */

  SDSSSCR_CNT_INC_NEW,        /* Count only systems that are available for
                              ** acquisition attempts and comply with the
                              ** new acquisition counter criteria. A system
                              ** complies with the new acquisition counter
                              ** criteria if it is not excluded from
                              ** acquisition attempt based on its new_acq_cnt
                              ** field */

  SDSSSCR_CNT_INC_ABSR,       /* Count only systems that are available for
                              ** acquisition attempts, the system is avoided
                              ** due to MAP and ABSR cnt is greater than 0.
                              */

  SDSSSCR_CNT_INC_MAX
} sdssscr_cnt_inc_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of SID matching conditions.
*/
typedef enum {

  SDSSSCR_SID_COND_EXPECTED,      /* SID/NID match expected SID/NID */
  SDSSSCR_SID_COND_NOT_EXPECTED,  /*SID/NID does not match expected SID/NID*/
  SDSSSCR_SID_COND_COLLOC_IDL_SYS,/*SID is collocated with SDSR_IDL_SYS */
  SDSSSCR_SID_COND_NOT_EXPECTED_WITH_BID, /* SID/NID/BID does not match expected SID/NID/BID*/

  SDSSSCR_SID_COND_MAX
} sdssscr_sid_cond_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration that defines the various configurable SD values that are stored
** in NV.
**
** ADD NEW enums ONLY to the end of the enum type (excluding
** SDSS_NV_ITEM_MAX).
**
** UPDATE sd_cfg_items_default.items to include default values for all new
** enums that are added.
**
** DO NOT DELETE any enum before you run out of space in the NV array. If you
** end up deleting unused enums due to lack of space in the NV array, then
** increment the value of "sd_cfg_items_default.version" by 1.
*/
typedef enum {

  /* 0 */
  /* AMPS idle reselection time. */
  SDSSSCR_NV_ITEM_TMR_AMPS_RESEL = 0,

  /* CDMA idle reselection time. */
  SDSSSCR_NV_ITEM_TMR_CDMA_RESEL,

  /* CDMA call end reselection time. */
  SDSSSCR_NV_ITEM_TMR_CDMA_RESEL_HOLD_CALL_END,

  /* CDMA call end reselection time. */
  SDSSSCR_NV_ITEM_TMR_CDMA_RESEL_HOLD_IDLE_HOFF,

  /* 4 */
  /* CDMA GSRDM wait time */
  SDSSSCR_NV_ITEM_TMR_CDMA_REDIR,

  /* HDR idle reselection time. */
  SDSSSCR_NV_ITEM_TMR_HDR_RESEL,

  /* HDR reselection hold time. */
  SDSSSCR_NV_ITEM_TMR_HDR_RESEL_HOLD,

  /* HDR GSRDM wait time. */
  SDSSSCR_NV_ITEM_TMR_HDR_REDIR,

  /* 8 */
  /* HDR call end reselection time. */
  SDSSSCR_NV_ITEM_TMR_HDR_RESEL_HOLD_CALL_END,

  /* CDMA Pilot strength measurement. */
  SDSSSCR_NV_ITEM_CDMA_PI_THR,

  /* Extend BSR for a duration if reselection is not allowed */
  SDSSSCR_NV_ITEM_RESEL_HOLD_3GPP_ACTIVITY,


  SDSSSCR_NV_ITEM_TMR_RESERVED_1,

  /* 12 */
  /* system reselection timer for ss pref */
  SDSSSCR_NV_ITEM_TMR_RESEL_TIMEOUT_SS_PREF,

  /* Flag to enable/disable C2K Alt Algorithm operation */
  SDSSSCR_NV_ITEM_C2K_ALT_ALGR,

  SDSSSCR_NV_ITEM_ALT_CDMA_BSR_STAGE1_TIME,
    /* Time for ALT 1x BSR during state1 */

  SDSSSCR_NV_ITEM_ALT_CDMA_BSR_STAGE2_TIME,
    /* Time for ALT 1x BSR during state2 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* 16 */
  /* CDMA resel hold time when there is HDR activty */
  SDSSSCR_NV_ITEM_TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Location based HDR to LTe resel timer */
  SDSSSCR_NV_ITEM_TMR_LOC_BAS_HDR_LTE_RESEL,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSSSCR_NV_ITEM_PWR_SAVE_ENTER_TIME,

  SDSSSCR_NV_ITEM_PWR_SAVE_WAKEUP_TIME,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* 19 */
  /* Power save timer for Deep and Shadow sleep*/
  SDSSSCR_NV_ITEM_TMR_PWR_SAVE_COLLOC,

  SDSSSCR_NV_ITEM_TMR_LTE_EHRPD_RESEL,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* 21 */
  /* CDMA MAP avoidance timer */
  SDSSSCR_NV_ITEM_TMR_CDMA_MAP_AVOID_TIMER,

  /* CDMA ABSR counter */
  SDSSSCR_NV_ITEM_TMR_CDMA_ABSR_COUNT,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* 23 */
  /* CDMA ABSR counter per BSR  */
  SDSSSCR_NV_ITEM_TMR_CDMA_NUM_ABSR_PER_BSR,

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Internal use. */
  SDSSSCR_NV_ITEM_MAX

} sdssscr_nv_item_e_type;


/* Local structure for storing configurable SD values read from NV.
*/
typedef struct {
  /* Version. */
  uint16                                           version;
  /* Total number of valid entries in the items array */
  uint16                                           count;
  /* Configurable SD values */
  uint32                                           items[SDSSSCR_NV_ITEM_MAX];
} sdssscr_sd_cfg_items_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration that defines the various configurable SD values that are stored
** in EFS.
**
** ADD NEW enums ONLY to the end of the enum type (excluding
** SDSSSCR_EFS_ITEM_MAX).
**
** UPDATE sd_efs_items_default.items to include default values for all new
** enums that are added.
**
** DO NOT DELETE any enum before you run out of space in the NV array. If you
** end up deleting unused enums due to lack of space in the NV array, then
** increment the value of "sd_efs_items_default.version" by 1.
*/
typedef enum {

  SDSSSCR_EFS_ITEM_EMERG_CB_TIME = 0,
    /* Time to try SRV_SYS in ECBM when sys is lost */


  SDSSSCR_EFS_ITEM_N_MIN_VOICE_SCANS = 1,
    /* Number of voice scans before trigger data scans */

  SDSSSCR_EFS_ITEM_TMR_LTE_LONG_RESEL = 2,
    /* LTE Long reselection time.
    ** Only used when ALT_CDMA_BSR flag is turned on
    */

  SDSSSCR_EFS_ITEM_LTE_NOT_AVAIL_ACQ_FAIL_MAX_COUNT = 3,
    /* LTE ACQ_FAIL Max count to declare LTE Not Available area
    ** Trigger LBSR if the fail counter is equal or bigger than the max count
    */

  SDSSSCR_EFS_ITEM_LTE_TELESCOPE_BSR_STAGE = 4,
   /* LTE telescopic BSR stage indicate how many BSR timer incremental stages
   ** will be used
   */

  SDSSSCR_EFS_ITEM_LTE_TELESCOPE_BSR_RPT_CUNT = 5,
   /* LTE telescopic BSR repeat counter indicate how many times one stage timer
   ** will run
   */

  SDSSSCR_EFS_ITEM_LTE_TELESCOPE_BSR_TIME = 6,
   /* LTE telescopic BSR timer indicate minimum LTE BSR timer granularity
   ** e.g stage is 5, rpt_cunt is 2, timer is 3min, the LTE telescopic time line will be:
   ** 3,3,6,6,12,12,24,24,48,48...48min forever*/
  
  SDSSSCR_EFS_ITEM_LOC_BASE_BSR =7,
  /* Flag to indicate if location based bsr feature is enabled */
  
  SDSSSCR_EFS_ITEM_MAX_ACQ_DB_SCAN_TIME = 8,
  /**< EFS id for Max Acq DB Scan duration */

  SDSSSCR_EFS_ITEM_T_ACQ_SCAN = 9,
  /**< EFS id for Time to trigger Acq DB Scan */

  SDSSSCR_EFS_ITEM_ACQ_SCAN_DURATION = 10,
  /**< EFS id for Acq DB Scan duration */  

  /* After adding a new enum, do the following:
  ** if (SDSSSCR_EFS_ITEM_MAX > SDSSSCR_EFS_ITEM_SLOTS )
  **   Adjust SDSSSCR_EFS_ITEM_SLOTS such that
  **     SDSSSCR_EFS_ITEM_SLOTS >= SDSSSCR_EFS_ITEM_MAX and
  **     Work with Diag team to update QXDM NV Browser software with new
  **       value of SDSSSCR_EFS_ITEM_SLOTS.
  */
  SDSSSCR_EFS_ITEM_MAX,
    /* Internal use. */

  SDSSSCR_EFS_ITEM_SLOTS = 50

} sdssscr_efs_item_e_type;


/* Local structure for storing configurable SD values read from EFS.
*/
typedef struct {
  /* Version. */
  uint16                                        version;
  /* Total number of valid entries in the items array */
  uint16                                        count;
  /* Configurable SD values */
  uint32                                        items[SDSSSCR_EFS_ITEM_SLOTS];
} sdssscr_sd_efs_items_s_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define SDSSSCR_RESERVED                           ( ( byte ) 0)


/* <EJECT> */
/*---------------------------------------------------------------------------
                             SS-Script Parser
---------------------------------------------------------------------------*/

/* An SS-Script shall only be parsed using an SS-Script-parser. The script
** parser provides an interface for parsing scripts and hides the details
** that are associated with the physical layout of the script and the
** implementation of the loop control constructs (i.e. the LOOP_TIME,
** LOOP_CNT, LOOP_FOREVER and LOOP constructs).
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for referencing an SS-Script. This type is defined as a void pointer,
** but is actually an opaque pointer to a sdssscr_elem_u_type. When this
** value is passed into an SS-Script routine, it is being cast to a
** *sdssscr_elem_u_type. Code outside of the SS-Script should never cast this
** type to a sdssscr_elem_u_type or make any use of it other than passing it
** to SS-Script routines.
*/
typedef   const void*         sdssscr_ref_type;


/* Definition for the NULL SS-Script reference (i.e. an SS-Script reference
** that is resolved to no script at all).
*/
#define   SDSSSCR_REF_NULL    ( (sdssscr_ref_type) NULL )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type to hold an SS-Script Construct Parameter.
*/
typedef   word                sdssscr_prm_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for SS-Script parser.
*/
typedef struct {

  /* Reference to an SS-Script.
  */
  sdssscr_ref_type    scr_ref;

  /* Keep track of script's event group and table index (for debugging).
  */
  sdss_evt_grp_e_type scr_evt_grp;
  int                 scr_tbl_idx;

  /* Index to record the script position of the next script-construct to be
  ** parsed.
  */
  int                 scr_idx;
  int                 scr_line;

  /* Item to record the type of the current loop. Set to SDSSSCR_CON_MAX if
  ** not currently in a loop
  **
  */
  sdssscr_con_e_type  loop_type;

  /* Index to record the script position of the last LOOP_XXX() script
  ** construct to be encountered.
  */
  int                 loop_idx;
  int                 loop_line;

  /* Loop value.
  **
  ** In the context of a LOOP_TIME( TIME ) script construct records the loop
  ** start time. When uptime > loop_value + TIME, script execution falls out
  ** of the loop and continues with the script construct that follows the
  ** matching LOOP construct.
  **
  ** In the context of a LOOP_NUM( NUM ) script construct records the
  ** remaining number of times to go through the loop. When loop_value = 0,
  ** script execution falls out of the loop and continues with the script
  ** construct that follows the matching LOOP construct.
  **
  ** In the context of LOOP_COND( COND ) is set to the COND
  **
  ** In the context of a LOOP_FOREVER script construct set to 1.
  **
  ** In the context of any other script construct set to 0.
  */
  dword               loop_val;

  /* Loop condition.
  ** In the context of LOOP_COND( COND ) script construct, records the
  ** loop condition. When COND is FALSE, script execution falls out
  ** of the loop and continues with the script construct that follows the
  ** matching LOOP construct.
  */
  sdss_loop_cond_e_type loop_cond;

  /* Counter to record the value of the script execution control counter
  ** upon entering a loop.
  **
  ** If when reaching the matching LOOP construct the script execution
  ** control counter has the same value as when the loop was entered, a loop
  ** that does not return control to the SS-Front-end as been encountered. In
  ** such a case script execution should fall out of the loop regardless
  ** of whether the loop condition evaluates to TRUE or FALSE.
  */
  int                 loop_ctrl_cnt;
  int                 loop_back_cnt;

  /* Index to record the script position of the if condition
  */
  int                 if_idx;
  int                 if_line;
  int                 nest_if_idx;
  int                 nest_if_line;

  /* If value.
  ** True is IF is true
  ** False otherwise
  */
  boolean             if_val;
  boolean             nest_if_val;

  /* Index to record the script position of the else condition
  */
  int                 else_idx;
  int                 else_line;
  int                 nest_else_idx;
  int                 nest_else_line;

  /* Index to record the script position of the endif condition
  */
  int                 endif_idx;
  int                 endif_line;
  int                 nest_endif_idx;
  int                 nest_endif_line;

  /* System selection with which this parser is associated.
  */
  sd_ss_e_type        ss;

} sdssscr_parser_s_type;

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== SS-SCRIPT TABLES =============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdssscr_get_ref

DESCRIPTION
  Get a reference to the script that is associated with a specified SS-Event
  and table entry number.

DEPENDENCIES
  None.

RETURN VALUE
  Reference to the script that is associated with the input event and table
  entry number. SDSSSCR_REF_NULL if no such script is found.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdssscr_ref_type          sdssscr_get_ref(

        sdss_evt_grp_e_type       event_group,
            /* Event group which to get the script reference.
            */

        int                       entry
            /* Entry number for which get the script reference.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_called_get_ref

DESCRIPTION
  Given a called SS-Script name, return a reference to the script that is
  referenced by this name. If no SS-Script is referenced by this name, return
  SDSSSCR_REF_NULL.

DEPENDENCIES
  None.

RETURN VALUE
  Reference to a called SS-Script if the input SS-Script name references an
  SS-Script. SDSSSCR_REF_NULL, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdssscr_ref_type          sdssscr_called_get_ref(

        sdssscr_e_type            scr_name
            /* Script name for which to return the referenced SS-Script.
            */
);




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
====================== SYSTEM SELECTION SCRIPT PARSER =======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdssscr_parser_init

DESCRIPTION
  Initialize a script-parser with a new SS-Script.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdssscr_parser_init(

        sdssscr_parser_s_type     *parser_ptr,
            /* Pointer to a script-parser item to be initialized with the
            ** SS-Script that is pointed to by the scr_ptr parameter.
            */

        sdssscr_ref_type          scr_ref,
            /* Reference to an SS-Script with which to initialize the
            ** script-parser that is pointed to by the parser_ptr parameter.
            */

        sdss_evt_grp_e_type       event_group,
        int                       scr_tbl_idx,
            /* The event group and script table index (for debugging).
            */

        sd_ss_e_type              ss
            /* System selection with which this parser is associated.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_parser_next

DESCRIPTION
  Return the next script-construct of the script with which the script-parser
  is currently associated.

DEPENDENCIES
  The script-parser that is pointed to by parser_ptr parameter must have
  already been associated with a script via a call to sdssscr_parser_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdssscr_con_e_type        sdssscr_parser_next(

        sdssscr_parser_s_type     *parser_ptr,
            /* Pointer to the script-parser that is associated with the
            ** script from which to get the next script-construct.
            */

        dword                     uptime,
            /* The current uptime.
            */

        int                       ctrl_cnt,
            /* Script control counter - being incremented every time control
            ** is returned to the SS-Client.
            */

        sdssscr_prm_type          *prm1_ptr,
            /* Pointer to an item where to store the 1st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm2_ptr,
            /* Pointer to an item where to store the 2st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm3_ptr,
            /* Pointer to an item where to store the 3st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm4_ptr,
            /* Pointer to an item where to store the 4st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm5_ptr,
            /* Pointer to an item where to store the 4st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm6_ptr,
            /* Pointer to an item where to store the 4st parameter that is
            ** associated with the returned script-construct (if any).
            */
        sdssscr_prm_type          *prm7_ptr,
            /* Pointer to an item where to store the 5th parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm8_ptr,
            /* Pointer to an item where to store the 6th parameter that is
            ** associated with the returned script-construct (if any).
            */
        sd_ss_e_type              ss
            /* System selection with which this parser is associated.
            */
        
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_parser_is_contain

DESCRIPTION
  Check whether the script that is associated with the script parser that is
  pointed to by parser_ptr contains a specified script construct.

DEPENDENCIES
  The script-parser that is pointed to by parser_ptr parameter must have
  already been associated with a script via a call to sdssscr_parser_init().

RETURN VALUE
  TRUE if the script that is associated with the script parser that is
  pointed to by parser_ptr contains the specified script construct. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdssscr_parser_is_contain(

        const sdssscr_parser_s_type   *parser_ptr,
            /* Pointer to the script-parser that is associated with the
            ** script to be searched.
            */

        sdssscr_con_e_type            scr_con,
            /* Script construct for which to search.
            */

        boolean                       from_beginning,
            /* Is the matching to be done from the beginning of the script
            */

        int                          *rtrn_idx,
            /* Record the index where the construct is found
            */

        int                          *rtrn_line
            /* Record the line where the construct is found
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_nv_items_init

DESCRIPTION
  Read the various configurable SD values that are stored in NV. Use defaults
  when items not available in NV.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdssscr_nv_items_init( void );



/*===========================================================================

FUNCTION sdssscr_nv_item_get

DESCRIPTION
  Read the NV value for the specified item

DEPENDENCIES
  None.

RETURN VALUE
  The value of the NV item

SIDE EFFECTS
  None.

===========================================================================*/

extern  uint32                     sdssscr_nv_item_get(

        sdssscr_nv_item_e_type     nv_item
            /* The NV item to read
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_efs_items_init

DESCRIPTION
  Read the various configurable SD values that are stored in EFS. Use defaults
  when items not available in EFS.

DEPENDENCIES
  sdefs_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdssscr_efs_items_init( void );

/*===========================================================================

FUNCTION sdssscr_efs_item_get

DESCRIPTION
  Read the efs value for the specified item

DEPENDENCIES
  None.

RETURN VALUE
  The value of the efs item

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  uint32                    sdssscr_efs_item_get(

        sdssscr_efs_item_e_type    efs_item,
            /* The efs item to read
            */
        sys_modem_as_id_e_type     asubs_id
);

/*===========================================================================

FUNCTION sdss_eng_stack_is_type

DESCRIPTION
  get the last acquisition type of the script engine that is at the top of
  the stack.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  sdssscr_acq_e_type
SIDE EFFECTS
  None.

===========================================================================*/
sdssscr_acq_e_type     sdss_eng_stack_get_acq_type
(

       sd_ss_e_type            ss
);

#endif /* FEATURE_SD_ENABLE_DEBUG_STRINGS */
#endif /* !defined(SDSSSCR_H) || defined(FEATURE_SD_ENABLE_DEBUG_STRINGS) */
