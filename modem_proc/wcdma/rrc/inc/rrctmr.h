#ifndef RRCTMR_H
#define RRCTMR_H
/*===========================================================================
              R R C T M R   H E A D E R  F I L E

DESCRIPTION
 The RRCTMR module consists of RRC timers implementation.
  
Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  
All Rights Reserved.QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrctmr.h_v   1.6   30 May 2002 09:06:46   krishnan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrctmr.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/15   as      Added code to support LTA durting activation time
03/11/15   sr      Made changes to convert BPLMN_LOCK_REL_OFFSET_TIMER to a non-deferrable one
01/29/15   bc      Changes to crash the UE if there is no response to STATE_CHANGE_IND from LLC 
                   for more than 5 seconds.
01/20/15   bc      Changes to crash the UE if there is no response to ACTIVATION_IND 
                   from NAS during WTOG CCO.
06/23/14    vi      Made changes to skip SIB7 reading if SIB7 expiration timer is stil running
06/11/14   bc      Made changes to reduce DCH to FACH cell selection time.
06/18/14   as      Made changes to delay reselection while waiting for L2 ACK for PCH transition
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
11/05/13   rmn     Changes for displaying Partial PLMN Search Results during Manual Search
09/16/13   sn      G2W TA feature changes
12/25/12   db      Made changes to requeue transition to disconnected triggered by CU when there is no response
12/07/12   sn      CSG feature changes
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/13/12   gv      Defeaturize the structure rrctmr_timer_e_type to avoid timer id mismatches
05/01/12   sks     Changes were made to start a delay timer if pre-emption cannot be handled, and handle it
                   once the timer expires.
04/12/12   db      Residual Feature Cleanup
04/09/12   gv      Added code to prevent CPU hogging during G2W redirection
03/29/12   sks     Made changes to get the TA duration and pass it to L1.
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/10/11   sks     Made changes to implement 5s guard timer for LTOW redirection.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
09/22/10   kp      Added code for Qchat timer that delay HSPA status indication
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM_WCDMA_TUNEAWAY
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/25/10   rm      Added code to support DUAL SIM BPLMN
06/07/10   as      Added code for EOOS under feature flag FEATURE_EOOS
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/29/10   sks     Fixed compilation when FEATURE_WTOG_BACKGROUND_PLMN_SEARCH is not defined.
03/15/10   gkg     Made changes to put changes for "Multiple DRX Cycles" and 
                   "CU wait Timer" under feature R7. 
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
11/02/09   sks     Added code to suspend handling of STOP_MODE_REQ, until current 
                   WTOG process is aborted.
06/05/09   ps      Made changes to not start the T305 timer if value is infinity
05/08/09   ss      Updated Copyright Information
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
11/06/08   sk      Added RRCTMR_WTOG_BPLMN_ABORT_GUARD_TIMER which guards against 
                   RRC waiting indefinitely for W2G BPLMN to be aborted
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/21/07   da      Support for FEATURE_MBMS
09/10/07   pk      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
05/23/07   pk      Added support for new timer RRCTMR_MANUAL_PLMN_FREQ_SCAN_TIMER
                   under the feature flag FEATURE_MANUAL_SEARCH_FREQ_SCAN_SKIP_TIMER.
                   Full frequency scans are not done when this timer is active.
03/28/07   kp      Added support for new timer RRC_BCCH_MODIFICATION_TIMER 
                   which  is used to implemented support for BCCH Mod Timer 
                   I.E in BCCH ModInfo.
09/25/06   kp      Added support for new timer RRC_SIB_CHANGE_GUARD_TIMER which 
                   prevents bplmn search during sib_change_ind handling
08/25/06   vm      Added support for FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT.
07/21/06   vm      Added support for the feature UOOSI.
06/07/06   kp      Added support for six hours sib update
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search. 
                   Added two new timers WTOW DRX search timer and WTOW/WTOG 
                   max guard timer.
11/04/05  da/vr    Made changes to detect UE inactivity and take appropriate actions.
07/08/05   vm      FEATURE_OUT_OF_SERVICE_ENHANCEMENT: Added support for OOS in 
                   Connected Mode feature.
                   Added new timer RRCTMR_CONN_MODE_OOS_TIMER.
07/07/05   da      Added prototype for rrctmr_get_remaining_time()
03/03/05   vm      Added support for inter-frequency redirection feature.
                   Added new timer RRCTMR_FREQ_REDIRECT_WAIT_TIMER.
11/22/04   vm      Added support for RRCTMR_BPLMN_SRCH_TIMER and 
                   RRCTMR_BPLMN_FREQ_SCAN_TIMER.
11/22/04   da      Added support for RRCTMR_PHY_CHAN_FAILURE_TIMER
11/18/04   vm      Added RRCTMR_DEEP_SLEEP_NO_SVC_TIMER for no service duration
                   in deep sleep.
11/18/04   vk      Declared RRCTMR_RRC_CON_DELAY_TIMER that delays 
                   releasing RRC Connection
09/15/04   sk      Added support for RRCTMR_T_OOS_TIMER.
05/05/04   bu      Added RRCTMR_FREQ_SCAN_TIMER for frequeny scan.
03/19/03   kc      Added timer for T3174 for GtoW PCCO.
11/20/03   bu      Added RRCTMR_DCH_PCH_CELL_SELECTION_TIMER to support a
                   timer during cell selection during DCH->CELL_PCH or DCH->URA_PCH.
09/03/03   bu      Added support for a new timer to delay ERR_FATAL.
06/23/03   vn      Added support for a new timer to detect L1 deadlocks.
02/11/03   vn      Added RRCTMR_DCH_FACH_CELL_SELECTION_TIMER to support a 
                   timer during cell selection during CELL_DCH->CELL_FACH.
11/04/02   rj      Added a timer to Ignore the BCCH Modification Info if 
                   RRC already processing one.,
05/29/02   ram     Added a timer for SIB 7 Expiration.
02/14/02   rj      Added enums for T302, T305, T316, T317, T307, T314 and 
                   T315 timers. These timers are required for Cell
                   update procedure implementation.
10/15/01   ram     Added an enum for a SIB wait timer. This is the maximum
                   time to wait for SIBs in a cell.
08/28/01   xfg     Added en enum for RSSI and BER timer
07/20/01   rj      Added declarations for timer T304 for UE Capability
                   Information procedure.
02/12/01   rj      Added enums for Timer T308 and L2ACK timer for
                   RRC Connection Release procedure. The enum for RCE
                   L2ACK timer is changed to RRCTMR_RCE_L2ACK_TIMER from
                   RRCTMR_WAIT_FOR_L2_ACK_TIMER. 
01/18/01   rj      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/
#include "timer.h"

/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

#define RRCTMR_RSSI_BER_REPORT_TIME 3000 /* Periodical RSSI&BER reporting time */
#define RRCTMR_INFINITY_VAL         86400000 
/* At present setting the value of infinity as 24 hrs */

typedef void (*rrc_non_def_timer_cb_ptr)( unsigned long param );

/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/


typedef enum
{
  RRCTMR_T_300_TIMER,           /* T300 Timer from SIB 1 */
  RRCTMR_T_308_TIMER,           /* T308 Timer from SIB 1 */
  RRCTMR_WAIT_TIME_TIMER,       /* Wait Time Timer from RRC Connection Reject
                                   Message */
  RRCTMR_UE_TO_CAMP_ON_TIMER,   /* Timer for UE to camp on, if not already */
  RRCTMR_RRC_CON_DELAY_TIMER,   /* Timer for UE to delay RRC Conn Release */
  RRCTMR_RCE_L2ACK_TIMER,       /* Timer to receive L2 ACK in acknowledged
                                   mode transmission for RCE procedure */
  RRCTMR_RCR_L2ACK_TIMER,       /* Timer to receive L2 ACK in acknowledged
                                   mode transmission for RCR procedure */
  RRCTMR_T_304_TIMER,           /* T304 Timer from SIB 1 */
  RRCTMR_RSSI_BER_TIMER,        /* Timer to periodically report RSSI & BER 
                                   to CM */
  RRCTMR_SIB_WAIT_TIMER,        /* Timer for maximum time to wait for SIBs 
                                   in a cell */
  RRCTMR_T_302_TIMER,           /* T302 timer for CELL UPDATE retransmission */
  RRCTMR_T_305_TIMER,           /* T305 timer for periodic CELL UPDATE */
  RRCTMR_T_316_TIMER,           /* T316 Timer for Out of Service Area in
                                   CELL_PCH and URA_PCH state */
  RRCTMR_T_317_TIMER,           /* T317 Timer for Out of service Area in
                                   CELL_FACH state */
  RRCTMR_T_307_TIMER,           /* T307 Timer is max time for UE to get into
                                   In Service after T305 is expired */
  RRCTMR_T_314_TIMER,           /* T314 Re-establish timer */
  RRCTMR_T_315_TIMER,           /* T315 Re-establish timer */
  RRCTMR_SIB7_EXP_TIMER,        /* Timer for SIB7 Expiration */
  RRCTMR_IGNORE_BCCH_MOD_TIMER,  /* Timer to Ignore BCCH Modifcation Info IE
                                   if RRC is already processing one */
  RRCTMR_DCH_FACH_CELL_SELECTION_TIMER, /* Timer started during cell selection
                                           going DCH->FACH
                                         */ 
  RRCTMR_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER, /* Timer started during cell selection
                                           going FACH->FACH/PCH
                                         */ 
  RRCTMR_L1_DEADLOCK_DETECT_TIMER, /* Timer to detect L1 deadlocks */
  RRCTMR_DELAY_ERR_FATAL_TIMER, /* Timer to delay ERR_FATAL */
  RRCTMR_DCH_PCH_CELL_SELECTION_TIMER,  /* Timer started during cell selection
                                          going DCH->CELL_PCH or DCH->URA_PCH*/
  RRCTMR_T_3174_TIMER,               /* T3174 GTOW PCCO timer */
  RRCTMR_FREQ_SCAN_TIMER,             /* Timer indicating if FREQ_SCAN should be
                                        initiated or not */
  RRCTMR_T_OOS_TIMER,             /* Timer indicating if UE can not select a cell
                                        after OOS detection */
  RRCTMR_PHY_CHAN_FAILURE_TIMER,        /* Phy Chan Failure timer used when initiating Cell Selec when  
                                       we get a phy_chan_failure after a rrc conn setup */
  RRCTMR_DEEP_SLEEP_NO_SVC_TIMER, /* Time duration for which no service should be 
                                        returned to NAS */
  RRCTMR_BPLMN_SRCH_TIMER,         /* Timer for background PLMN search from 2G->3G */
  RRCTMR_BPLMN_FREQ_SCAN_TIMER,     /* Timer indicating if FULL FREQ SCAN should be
                                       initiated or not for a 2G->3G BPLMN srch request */
  RRCTMR_MANUAL_PLMN_FREQ_SCAN_TIMER,     /* Timer indicating if FULL FREQ SCAN 
                                       should be done or not for a Manual PLMN search */
  RRCTMR_FREQ_REDIRECT_WAIT_TIMER,   /* Timer for inter frequency redirection */
  RRCTMR_CONN_MODE_OOS_TIMER,        /* Timer for OOS in Connected Mode */
  RRCTMR_WTOW_BPLMN_DRX_SRCH_TIMER,  /* Timer for 3G->3G BPLMN DRX tmr expiry*/
  RRCTMR_BPLMN_GUARD_SRCH_TIMER    /* Max Timer to complete 3G->3G, 2G BPLMN search */

#include "wcdma_variation.h"
  ,RRCTMR_INACTIVITY_TIMER       /* Timer for inactivity handling */

  ,RRCTMR_SIXHR_SIB_UPDATE_TIMER  /* Timer for six hr sib clean up*/

  ,RRCTMR_UNIFORM_OOS_SEARCH_TIMER

  ,RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER
  ,RRCTMR_CONN_MODE_OOS_DS_TIMER

  ,RRC_SIB_CHANGE_GUARD_TIMER     /* Timer for sib change guard handling */
  
  ,RRC_BCCH_MODIFICATION_TIMER     /* Timer for BCCH Modification timer handling */

  ,RRCTMR_GTOW_REDIRECTION_WAIT_TIMER

  ,RRCTMR_GTOW_REDIRECTION_LFS_TIMER

  ,RRCTMR_WTOG_ABORT_GUARD_TIMER


  ,RRCTMR_T_319_TIMER            /*T319 timer for DRX coefficient*/

  ,RRCTMR_T_320_TIMER

  ,RRCTMR_COUEC_TIMER

  ,RRCTMR_WRM_LOCK_TIMER
  ,RRCTMR_WRM_DELAY_PREEMPTION_TIMER
  ,RRCTMR_DSIM_BPLMN_LOCK_REL_OFFSET_TIMER

  ,RRCTMR_WRM_UNLOCK_BY_TIMER
  ,RRCTMR_GET_TA_DURATION_TIMER
  ,RRCTMR_T_323_TIMER

  ,RRCTMR_EOOS_FULL_SCAN_TIMER

  ,RRCTMR_DED_PRI_VALIDITY_TIMER

#ifdef FEATURE_QCHAT
  ,RRCTMR_QCHAT_FA_RRC_TIMER
#endif

  ,RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER
  ,RRCTMR_RRCCU_TRANS_TO_DISCON_TIMER
#ifdef FEATURE_FEMTO_CSG
  ,RRCTMR_CSG_ASF_SRCH_TIMER
#endif

#ifdef FEATURE_DUAL_SIM
 ,RRCTMR_SIB_STOP_PARTITION_TIMER
 ,RRCTMR_SIB_MASK_INVERT_TIMER
 ,RRCTMR_SIB_EMPTY_MASK_TIMER
#endif
 ,RRCTMR_BPLMN_PARTIAL_REPORT_TIMER/*M-PLMN Partial Report timer*/
 ,RRCTMR_WAIT_FOR_L2_ACK_FOR_PCH_TRANSITION_TIMER
 ,RRCTMR_TENMIN_SIB_UPDATE_TIMER
 , RRCTMR_SIB7_EXPIRATION_TIMER
 ,RRCTMR_ACT_RSP_EF_TIMER
 ,RRCTMR_WT_STATE_CHANGE_EF_TIMER
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
 ,RRCTMR_OTA_BUFFER_ACTIVATION_TIMER
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
}rrctmr_timer_e_type;
/* Below #defines are added for logging purpose only */
  #define rrctmr_timer_e_type_value0 RRCTMR_T_300_TIMER,           /* T300 Timer from SIB 1 */
  #define rrctmr_timer_e_type_value1 RRCTMR_T_308_TIMER,           /* T308 Timer from SIB 1 */
  #define rrctmr_timer_e_type_value2 RRCTMR_WAIT_TIME_TIMER,       /* Wait Time Timer from RRC Connection Reject
                                   Message */
  #define rrctmr_timer_e_type_value3 RRCTMR_UE_TO_CAMP_ON_TIMER,   /* Timer for UE to camp on, if not already */
  #define rrctmr_timer_e_type_value4 RRCTMR_RRC_CON_DELAY_TIMER,   /* Timer for UE to delay RRC Conn Release */
  #define rrctmr_timer_e_type_value5 RRCTMR_RCE_L2ACK_TIMER,       /* Timer to receive L2 ACK in acknowledged
                                   mode transmission for RCE procedure */
  #define rrctmr_timer_e_type_value6 RRCTMR_RCR_L2ACK_TIMER,       /* Timer to receive L2 ACK in acknowledged
                                   mode transmission for RCR procedure */
  #define rrctmr_timer_e_type_value7 RRCTMR_T_304_TIMER,           /* T304 Timer from SIB 1 */
  #define rrctmr_timer_e_type_value8 RRCTMR_RSSI_BER_TIMER,        /* Timer to periodically report RSSI & BER 
                                   to CM */
  #define rrctmr_timer_e_type_value9 RRCTMR_SIB_WAIT_TIMER,        /* Timer for maximum time to wait for SIBs 
                                   in a cell */
  #define rrctmr_timer_e_type_value10 RRCTMR_T_302_TIMER,           /* T302 timer for CELL UPDATE retransmission */
  #define rrctmr_timer_e_type_value11 RRCTMR_T_305_TIMER,           /* T305 timer for periodic CELL UPDATE */
  #define rrctmr_timer_e_type_value12 RRCTMR_T_316_TIMER,           /* T316 Timer for Out of Service Area in
                                   CELL_PCH and URA_PCH state */
  #define rrctmr_timer_e_type_value13 RRCTMR_T_317_TIMER,           /* T317 Timer for Out of service Area in
                                   CELL_FACH state */
  #define rrctmr_timer_e_type_value14 RRCTMR_T_307_TIMER,           /* T307 Timer is max time for UE to get into
                                   In Service after T305 is expired */
  #define rrctmr_timer_e_type_value15 RRCTMR_T_314_TIMER,           /* T314 Re-establish timer */
  #define rrctmr_timer_e_type_value16 RRCTMR_T_315_TIMER,           /* T315 Re-establish timer */
  #define rrctmr_timer_e_type_value17 RRCTMR_SIB7_EXP_TIMER,        /* Timer for SIB7 Expiration */
  #define rrctmr_timer_e_type_value18 RRCTMR_IGNORE_BCCH_MOD_TIMER,  /* Timer to Ignore BCCH Modifcation Info IE
                                   if RRC is already processing one */
  #define rrctmr_timer_e_type_value19 RRCTMR_DCH_FACH_CELL_SELECTION_TIMER, /* Timer started during cell selection
                                           going DCH->FACH
                                         */ 
  #define rrctmr_timer_e_type_value20 RRCTMR_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER, /* Timer started during cell selection
                                           going FACH->FACH/PCH
                                         */ 
  #define rrctmr_timer_e_type_value21 RRCTMR_L1_DEADLOCK_DETECT_TIMER, /* Timer to detect L1 deadlocks */
  #define rrctmr_timer_e_type_value22 RRCTMR_DELAY_ERR_FATAL_TIMER, /* Timer to delay ERR_FATAL */
  #define rrctmr_timer_e_type_value23 RRCTMR_DCH_PCH_CELL_SELECTION_TIMER,  /* Timer started during cell selection
                                          going DCH->CELL_PCH or DCH->URA_PCH*/
  #define rrctmr_timer_e_type_value24 RRCTMR_T_3174_TIMER,               /* T3174 GTOW PCCO timer */
  #define rrctmr_timer_e_type_value25 RRCTMR_FREQ_SCAN_TIMER,             /* Timer indicating if FREQ_SCAN should be
                                        initiated or not */
  #define rrctmr_timer_e_type_value26 RRCTMR_T_OOS_TIMER,             /* Timer indicating if UE can not select a cell
                                        after OOS detection */
  #define rrctmr_timer_e_type_value27 RRCTMR_PHY_CHAN_FAILURE_TIMER,        /* Phy Chan Failure timer used when initiating Cell Selec when  
                                       we get a phy_chan_failure after a rrc conn setup */
  #define rrctmr_timer_e_type_value28 RRCTMR_DEEP_SLEEP_NO_SVC_TIMER, /* Time duration for which no service should be 
                                        returned to NAS */
  #define rrctmr_timer_e_type_value29 RRCTMR_BPLMN_SRCH_TIMER,         /* Timer for background PLMN search from 2G->3G */
  #define rrctmr_timer_e_type_value30 RRCTMR_BPLMN_FREQ_SCAN_TIMER,     /* Timer indicating if FULL FREQ SCAN should be
                                       initiated or not for a 2G->3G BPLMN srch request */
  #define rrctmr_timer_e_type_value31 RRCTMR_MANUAL_PLMN_FREQ_SCAN_TIMER,     /* Timer indicating if FULL FREQ SCAN 
                                       should be done or not for a Manual PLMN search */
  #define rrctmr_timer_e_type_value32 RRCTMR_FREQ_REDIRECT_WAIT_TIMER,   /* Timer for inter frequency redirection */
  #define rrctmr_timer_e_type_value34 RRCTMR_CONN_MODE_OOS_TIMER,        /* Timer for OOS in Connected Mode */
  #define rrctmr_timer_e_type_value35 RRCTMR_WTOW_BPLMN_DRX_SRCH_TIMER,  /* Timer for 3G->3G BPLMN DRX tmr expiry*/
  #define rrctmr_timer_e_type_value36 RRCTMR_BPLMN_GUARD_SRCH_TIMER    /* Max Timer to complete 3G->3G, 2G BPLMN search */

  #define rrctmr_timer_e_type_value37 RRCTMR_INACTIVITY_TIMER       /* Timer for inactivity handling */

  #define rrctmr_timer_e_type_value38 RRCTMR_SIXHR_SIB_UPDATE_TIMER  /* Timer for six hr sib clean up*/

  #define rrctmr_timer_e_type_value39 RRCTMR_UNIFORM_OOS_SEARCH_TIMER

  #define rrctmr_timer_e_type_value40 RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER
  #define rrctmr_timer_e_type_value41 RRCTMR_CONN_MODE_OOS_DS_TIMER

  #define rrctmr_timer_e_type_value42 RRC_SIB_CHANGE_GUARD_TIMER     /* Timer for sib change guard handling */
  
  #define rrctmr_timer_e_type_value43 RRC_BCCH_MODIFICATION_TIMER     /* Timer for BCCH Modification timer handling */

  #define rrctmr_timer_e_type_value44 RRCTMR_GTOW_REDIRECTION_WAIT_TIMER

  #define rrctmr_timer_e_type_value45 RRCTMR_GTOW_REDIRECTION_LFS_TIMER

  #define rrctmr_timer_e_type_value46 RRCTMR_WTOG_ABORT_GUARD_TIMER


  #define rrctmr_timer_e_type_value47 RRCTMR_T_319_TIMER            /*T319 timer for DRX coefficient*/

  #define rrctmr_timer_e_type_value48 RRCTMR_T_320_TIMER

  #define rrctmr_timer_e_type_value49 RRCTMR_COUEC_TIMER

  #define rrctmr_timer_e_type_value50 RRCTMR_WRM_LOCK_TIMER
  #define rrctmr_timer_e_type_value51 RRCTMR_WRM_DELAY_PREEMPTION_TIMER
  #define rrctmr_timer_e_type_value52 RRCTMR_DSIM_BPLMN_LOCK_REL_OFFSET_TIMER

  #define rrctmr_timer_e_type_value53 RRCTMR_WMR_UNLCOK_BY_TIMER
  #define rrctmr_timer_e_type_value54 RRCTMR_GET_TA_DURATION_TIMER
  #define rrctmr_timer_e_type_value55 RRCTMR_T_323_TIMER

  #define rrctmr_timer_e_type_value56 RRCTMR_EOOS_FULL_SCAN_TIMER

  #define rrctmr_timer_e_type_value57 RRCTMR_DED_PRI_VALIDITY_TIMER

#ifdef FEATURE_QCHAT
  #define rrctmr_timer_e_type_value58 RRCTMR_QCHAT_FA_RRC_TIMER
#endif

  #define rrctmr_timer_e_type_value59 RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER
  #define rrctmr_timer_e_type_value60 RRCTMR_RRCCU_TRANS_TO_DISCON_TIMER
#ifdef FEATURE_FEMTO_CSG
  #define rrctmr_timer_e_type_value61 RRCTMR_CSG_ASF_SRCH_TIMER
#endif

#ifdef FEATURE_DUAL_SIM
 #define rrctmr_timer_e_type_value62 RRCTMR_SIB_STOP_PARTITION_TIMER
 #define rrctmr_timer_e_type_value63 RRCTMR_SIB_MASK_INVERT_TIMER
 #define rrctmr_timer_e_type_value64 RRCTMR_SIB_EMPTY_MASK_TIMER
#endif

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION rrctmr_timers_callback

DESCRIPTION
  This function will be called when ever RRC timers expire.

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void rrctmr_timers_callback
(
   unsigned long param                   /* Parameter to Identify timer */                               
)
;

/*===========================================================================

FUNCTION rrctmr_create_timers

DESCRIPTION
  This function will create all RRC related timers. This function will be 
  called at RRC Task startup.
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void rrctmr_create_timers
(
  void
);
/*===========================================================================

FUNCTION rrctmr_delete_timers

DESCRIPTION
  This function will delete all RRC related timers. This function willbe
  called when RRC Task killed.
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void rrctmr_delete_timers
(
  void
);
/*===========================================================================

FUNCTION rrctmr_start_timer

DESCRIPTION
  This function will set the timer value and starts the timer.
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void rrctmr_start_timer
(
  rrctmr_timer_e_type timer,                /* Timer to start */
  unsigned long timer_value                 /* Value for timer */                      
);

/*===========================================================================

FUNCTION rrctmr_stop_timer

DESCRIPTION
  This function will clear the timer value and stops the timer.
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void rrctmr_stop_timer
(
  rrctmr_timer_e_type timer                 /* Timer to stop */
);

/*===========================================================================

FUNCTION rrctmr_get_remaining_time

DESCRIPTION
  This function will return the remaining tics for the Timer passed in.
  
DEPENDENCIES
  None

RETURN VALUE
  Unsigned Long.  Returns 0 if times is not recognized.
  
SIDE EFFECTS
  None
===========================================================================*/
uint64 rrctmr_get_remaining_time
(
  rrctmr_timer_e_type timer                 /* Timer to stop */
);

/*===========================================================================

FUNCTION rrc_def_non_defer_timer

DESCRIPTION
  Define non deferrable timer for RRC

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void rrc_def_non_defer_timer(timer_type *p_timer, rrc_non_def_timer_cb_ptr timer_cb_ptr, unsigned long param);
/*===========================================================================

FUNCTION rrc_set_non_defer_timer

DESCRIPTION
  Set non deferrable timer for RRC

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/

void rrc_set_non_defer_timer
(
   timer_type     *p_timer,  /* pointer to timer to set  */
   timetick_type  cnt       /* value to set the timer   */
);
/*===========================================================================

FUNCTION rrc_clr_non_defer_timer

DESCRIPTION
  Clear non deferrable timer for RRC

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void rrc_clr_non_defer_timer(timer_type *p_timer);
/*===========================================================================

FUNCTION rrc_clr_non_defer_timer

DESCRIPTION
  Get remaining time of non deferrable timer for RRC

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
timetick_type rrc_get_non_defer_timer
(
  timer_type *p_timer       /* pointer to the timer to get */
);

#endif /* RRCTMR_H */

