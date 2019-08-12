/*===========================================================================

                                  R R C T M R

GENERAL DESCRIPTION
 The RRCTMR module consists of RRC timers implementation.

EXTERNALIZED FUNCTIONS



Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  
All Rights Reserved.QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrctmr.c_v   1.10   30 May 2002 09:06:26   krishnan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrctmr.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/22/16   sp      Made changes to set activation timer expiry global to true if the timer expires during QTA
10/16/15   as      Added code to support LTA durting activation time
03/11/15   sr      Made changes to convert BPLMN_LOCK_REL_OFFSET_TIMER to a non-deferrable one
02/12/15   sa      Made changes for QTF with JO CRM
01/29/15   bc      Changes to crash the UE if there is no response to STATE_CHANGE_IND from LLC 
                   for more than 5 seconds.
01/20/15   bc      Changes to crash the UE if there is no response to ACTIVATION_IND 
                   from NAS during WTOG CCO.
06/23/14   vi      Made changes to skip SIB7 reading if SIB7 expiration timer is stil running
06/11/14   bc      Made changes to reduce DCH to FACH cell selection time.
06/11/14   as      Made changes to delay reselection while waiting for L2 ACK for PCH transition
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
11/05/13   rmn     Changes for displaying Partial PLMN Search Results during Manual Search
01/02/14   rmn     Reduction of unwanted F3's
09/16/13   sn      G2W TA feature changes
01/03/13   ad      Made changes to reduce the redundant F3s
12/25/12   db      Made changes to requeue transition to disconnected triggered by CU when there is no response
12/07/12   sn      CSG feature changes
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
09/21/12   gv      Added code to provide remaining time for RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/01/12   sks     Changes were made to start a delay timer if pre-emption cannot be handled, and handle it
                   once the timer expires.
05/01/12   sks     Made changes to create delay_preemption_timer using the correct timer ptr.
03/29/12   sks     Made changes to get the TA duration and pass it to L1.
04/09/12   gv      Added code to prevent CPU hogging during G2W redirection
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/10/11   sks     Made changes to implement 5s guard timer for LTOW redirection.
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/27/11   rm      Added code to support WCDMA to LTE events
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/18/10   rm      Added code to support EOOS Events
09/22/10   kp      Added code for Qchat timer that delay HSPA status indication
09/16/10   ss      Added missing code for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM
08/17/10   rm      Added code to return remaining time for RRCTMR_DED_PRI_VALIDITY_TIMER
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/25/10   rm      Added code to support DUAL SIM BPLMN
06/07/10   as      Added code for EOOS under feature flag FEATURE_EOOS
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/15/10   gkg     Made changes to put changes for "Multiple DRX Cycles" and 
                   "CU wait Timer" under feature R7. 
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
11/02/09   sks     Added code to suspend handling of STOP_MODE_REQ, until current 
                   WTOG process is aborted.
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
02/02/09   rm      Fixed Lint errors
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
12/29/08   rm      Added support for new Events
12/10/08   ps      Made changes to not allocate memory from static buffers
                   only for the timer expiry Ind, it should be allocated 
                   from heap only
11/06/08   sk      Added RRCTMR_WTOG_BPLMN_ABORT_GUARD_TIMER which guards against 
                   RRC waiting indefinitely for W2G BPLMN to be aborted
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/21/07   da      Support for FEATURE_MBMS
10/25/07   kp      Corrected right pointer name in rex_delete_timer_ex 
                   for BCCH Modification timer.
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
                   Added support for new timer RRCTMR_CONN_MODE_OOS_TIMER.
07/07/05   da      Added new func rrctmr_get_remaining_time() that returns
                   remaining tics of timer passed in.
03/03/05   vm      Added support for inter-frequency redirection feature.
                   Added new timer RRCTMR_FREQ_REDIRECT_WAIT_TIMER.
11/22/04   vm      Added support for RRCTMR_BPLMN_SRCH_TIMER and 
                   RRCTMR_BPLMN_FREQ_SCAN_TIMER.
11/24/04   da      Added support for RRCTMR_PHY_CHAN_FAILURE_TIMER
11/18/04   vm      Added support for RRCTMR_DEEP_SLEEP_NO_SVC_TIMER timer
11/18/04   vk      Added support to process RRCTMR_RRC_CON_DELAY_TIMER timer
09/15/04   sk      Added support for RRCTMR_T_OOS_TIMER.
08/30/04   sgk     Typecast third param of rex_def_timer_ex to uint32 in
                   function rrctmr_create_timers to fix multiple lint errors
                   Converting enum to int.
07/26/04   sk      Added debug prints for start, stop and expiry of timers.
05/05/04   bu      Added support for RRCTMR_FREQ_SCAN_TIMER.
03/19/04   kc      Added timer support for GtoW PCCO. T3174
11/20/03   bu      Added timer support for cell selection while going from
                   CELL_DCH to CELL_PCH/URA_PCH.
09/03/03   bu      Added support for a new timer to delay ERR_FATALs.
06/23/03   vn      Added support for a new timer to detect L1 deadlocks.
02/11/03   vn      Added code to support a timer during cell selection
                   while going from CELL_DCH->CELL_FACH.
01/15/03   sk      Added lint directive to suppress 'ignoring function return
                   value' error no534.
11/04/02   rj      Added implementaion for a timer to Ignore the BCCH 
                   Modification Info if RRC already processing one.
05/29/02   ram     Added implementation for SIB7 Expiration timer. This timer
                   is set to the expiration time for SIB7 in a cell. When the
                   timer expires, SIB7 has to be read again.
02/14/02   rj      Added implementation for timers T302, T305, T316, T317,
                   T307, T314 and T315. These timers are required for Cell
                   Update procedure implemenation.
10/15/01   ram     Added implementation for a SIB wait timer. This is the maximum
                   time to wait for SIBs in a cell. Changed all timers to use
                   rex_def_timer_ex instead of rex_create_timer_ex. The function
                   rex_def_timer_ex is the correct one to use since these
                   timers exist all the time and are not deleted until powerdown.
09/18/01   kmp     Featurized the RRC timer multiplication for debugging only.
08/28/01   xfg     Added a timer for reporting RSSI and BER to CM
08/08/01   rj      Multiplied all RRC timers by 10 for debugging purpose.
07/27/01   rj      Fixed a typo in function rrctmr_create_timers.
07/20/01   rj      Added implemenation for timer T304 for UE Capability
                   Information procedure.
05/02/01   jrs     Added semicolons after MSG_HIGHs.
02/12/01   rj      Added implementattion for Timer T308 and L2ACK timer for
                   RRC Connection Release procedure. The enum for RCE
                   L2ACK timer is changed to RRCTMR_RCE_L2ACK_TIMER from
                   RRCTMR_WAIT_FOR_L2_ACK_TIMER. The pointer variable for
                   RCE L2ACK timer is changed to rrc_t_rce_l2ack_ptr.
01/19/01   rj      Created file.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "rex.h"
#include "err.h"
#include "rrccmd_v.h"
#include "rrctmr.h"
#include "rrclogging.h"
#ifdef FEATURE_DUAL_SIM
#include "rrcsigs.h"
#include "task.h"
#endif
#ifdef FEATURE_QCHAT
#include "rrc_qchatupk.h"
#endif /* FEATURE_QCHAT*/
#include "rrccspi.h"
#include "rrcdata_v.h"

/*following lint directive is added to suppress the "ignoring return value" error
only for this file */
/*lint -e534*/
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

LOCAL rex_timer_type rrc_t_300;             /* Rex timer T300 */
LOCAL rex_timer_type *rrc_t_300_ptr;        /* Pointer Rex timer T300 */

LOCAL rex_timer_type rrc_t_308;             /* Rex timer T308 */
LOCAL rex_timer_type *rrc_t_308_ptr;        /* Pointer Rex timer T308 */

LOCAL rex_timer_type rrc_t_304;             /* Rex timer T304 */
LOCAL rex_timer_type *rrc_t_304_ptr;        /* Pointer Rex timer T304 */

LOCAL rex_timer_type rrc_t_wait_time;       /* Create Timer for Wait time */
LOCAL rex_timer_type *rrc_t_wait_time_ptr;  /* Pointer to Timer for Wait time */

LOCAL rex_timer_type rrc_t_ue_to_camp_on;   /* Create Timer for UE to camp on */ 
LOCAL rex_timer_type *rrc_t_ue_to_camp_on_ptr;
                                            /* Pointer to Timer for UE to camp on */

LOCAL rex_timer_type rrc_t_rrc_con_delay;   /* Create Timer for delaying release of RRC Conn */ 
LOCAL rex_timer_type *rrc_t_rrc_con_delay_ptr; /* Pointer to Timer for delaying RRC Conn Rel */
LOCAL rex_timer_type rrc_t_rce_l2ack;       /* Create timer for RCE for L2ACK */
LOCAL rex_timer_type *rrc_t_rce_l2ack_ptr;  /* Pointer to timer for RCE for L2ACK */

LOCAL rex_timer_type rrc_t_rcr_l2ack;       /* Create timer for RCR for L2ACK */
LOCAL rex_timer_type *rrc_t_rcr_l2ack_ptr;  /* Pointer to timer for RCR for L2ACK */

LOCAL rex_timer_type rrc_t_rssi_ber;        /* Create timer for RSSI report */
LOCAL rex_timer_type *rrc_t_rssi_ber_ptr;   /* Pointer timer for periodically 
                                               reporting RSSI & BER to CM */

LOCAL rex_timer_type rrc_t_sib_wait;        /* Create timer for waiting for
                                               collecting SIBs in a cell. */
LOCAL rex_timer_type *rrc_t_sib_wait_ptr;   /* Pointer to timer for waiting for
                                               collecting SIBs in a cell. */

#ifdef FEATURE_DUAL_SIM
LOCAL rex_timer_type rrc_t_sib_stop_part;
LOCAL rex_timer_type *rrc_t_sib_stop_part_ptr;

LOCAL rex_timer_type rrc_t_sib_mask;
LOCAL rex_timer_type *rrc_t_sib_mask_ptr;

LOCAL rex_timer_type rrc_t_sib_empty_mask;
LOCAL rex_timer_type *rrc_t_sib_empty_mask_ptr;
#endif

LOCAL rex_timer_type rrc_t_sib7_expiration;        /* Create timer for  SIB7 expiration in a cell. */
LOCAL rex_timer_type *rrc_t_sib7_expiration_ptr;   /* Pointer to  timer for  SIB7 expiration in a cell.*/

LOCAL rex_timer_type rrc_t_302;             /* Rex Timer for T302 */
LOCAL rex_timer_type *rrc_t_302_ptr;        /* Pointer Rex timer T302 */

LOCAL rex_timer_type rrc_t_305;             /* Rex Timer for T305*/
LOCAL rex_timer_type *rrc_t_305_ptr;        /* Pointer Rex timer T305 */

LOCAL rex_timer_type rrc_t_316;             /* Rex Timer for T316 */
LOCAL rex_timer_type *rrc_t_316_ptr;        /* Pointer Rex timer T316 */

LOCAL rex_timer_type rrc_t_317;             /* Rex Timer for T317 */
LOCAL rex_timer_type *rrc_t_317_ptr;        /* Pointer Rex timer T317 */

LOCAL rex_timer_type rrc_t_307;             /* Rex Timer for T307 */
LOCAL rex_timer_type *rrc_t_307_ptr;        /* Pointer Rex timer T307 */

LOCAL rex_timer_type rrc_t_314;             /* Rex Timer for T314 */
LOCAL rex_timer_type *rrc_t_314_ptr;        /* Pointer Rex timer T314 */

LOCAL rex_timer_type rrc_t_315;             /* Rex Timer for T315 */
LOCAL rex_timer_type *rrc_t_315_ptr;        /* Pointer Rex timer T315 */

LOCAL rex_timer_type rrc_t_sib7_exp;        /* Rex Timer for SIB7 Expiration */
LOCAL rex_timer_type *rrc_t_sib7_exp_ptr;   /* Pointer to Rex timer for SIB7 
                                               Expiration */
LOCAL rex_timer_type rrc_t_ignore_bcch_mod_exp;       
                                           /* Rex Timer for Ignoring BCCH 
                                              Modifcation Info */
LOCAL rex_timer_type *rrc_t_ignore_bcch_mod_exp_ptr; 
                                         /* Pointer to Rex Timer for Ignoring
                                            BCCH Modifcation Info */

LOCAL rex_timer_type rrc_t_dch_fach_cell_selection_exp;  /* Rex Timer for 
                                                          * cell selection when
                                                          * going from DCH to FACH
                                                          */    
LOCAL rex_timer_type *rrc_t_dch_fach_cell_selection_exp_ptr; /* Pointer to  
                                                              * Rex Timer for 
                                                              * cell selection when
                                                              * going from DCH to FACH
                                                              */    

LOCAL rex_timer_type rrc_t_fach_to_fach_pch_cell_selection_exp;  /* Rex Timer for 
                                                          * cell selection when
                                                          * going from FACH to FACH/PCH
                                                          */    
LOCAL rex_timer_type *rrc_t_fach_to_fach_pch_cell_selection_exp_ptr; /* Pointer to  
                                                              * Rex Timer for 
                                                              * cell selection when
                                                              * going from FACH to FACH/PCH
                                                              */    

LOCAL rex_timer_type rrc_t_l1_deadlock_detection_exp;  /* Rex Timer for 
                                                        * L1 deadlock detection.
                                                        */    
LOCAL rex_timer_type *rrc_t_l1_deadlock_detection_exp_ptr; /* Pointer to Rex Timer for 
                                                            * L1 deadlock detection.
                                                            */    

LOCAL rex_timer_type rrc_t_delay_err_fatal_exp;  /* Rex Timer to delay ERR_FATAL
                                                        */    
LOCAL rex_timer_type *rrc_t_delay_err_fatal_exp_ptr; /* Pointer to Rex Timer for 
                                                            * ERR_FATAL delay.
                                                            */    
LOCAL rex_timer_type rrc_t_dch_pch_cell_selection_exp;  /* Rex Timer for 
                                                          * cell selection when
                                                          * going from DCH to PCH
                                                          */    
LOCAL rex_timer_type *rrc_t_dch_pch_cell_selection_exp_ptr; /* Pointer to  
                                                              * Rex Timer for 
                                                              * cell selection when
                                                              * going from DCH to PCH
                                                              */    

LOCAL rex_timer_type rrc_t_3174;             /* Rex Timer for T3174 */
LOCAL rex_timer_type *rrc_t_3174_ptr;        /* Pointer Rex timer T3174 */

LOCAL rex_timer_type rrc_t_freq_scan_exp;             /* Rex Timer for Freq Scan */
LOCAL rex_timer_type *rrc_t_freq_scan_exp_ptr;        /* Pointer Rex timer for Freq Scan */

LOCAL rex_timer_type rrc_t_oos;             /* Rex Timer for OOS handling */
LOCAL rex_timer_type *rrc_t_oos_ptr;        /* Pointer Rex timer for OOS handling */

LOCAL rex_timer_type rrc_t_deep_sleep_no_svc;         /* Rex Timer for Deep Sleep no service handling */
LOCAL rex_timer_type *rrc_t_deep_sleep_no_svc_ptr;    /* Pointer Rex timer for Deep Sleep no service handling */

LOCAL rex_timer_type rrc_t_phy_chan_failure;       /* Create Timer for Physical chan failure */
LOCAL rex_timer_type *rrc_t_phy_chan_failure_ptr;  /* Pointer to Phy chan failure timer */


LOCAL rex_timer_type rrc_t_bplmn_srch;             /* Rex Timer for 2G->3G BPLMN search */
LOCAL rex_timer_type *rrc_t_bplmn_srch_ptr;        /* Pointer Rex timer 2G->3G BPLMN search */

LOCAL rex_timer_type rrc_t_bplmn_full_scan_srch;      /* Rex Timer for 2G->3G BPLMN search full scan*/
LOCAL rex_timer_type *rrc_t_bplmn_full_scan_srch_ptr; /* Pointer Rex timer 2G->3G BPLMN search full scan*/

LOCAL rex_timer_type rrc_t_manual_plmn_full_scan_srch;      /* Rex Timer for 2G->3G BPLMN search full scan*/
LOCAL rex_timer_type *rrc_t_manual_plmn_full_scan_srch_ptr; /* Pointer Rex timer 2G->3G BPLMN search full scan*/


LOCAL rex_timer_type rrc_t_freq_redirect_wait_timer; /* Rex timer for inter freq redirection feature*/
LOCAL rex_timer_type *rrc_t_freq_redirect_wait_timer_ptr; /* Pointer Rex timer for inter freq redirection feature*/

LOCAL rex_timer_type rrc_t_conn_mode_oos_timer; /* Rex timer for OOS in connected mode */
LOCAL rex_timer_type *rrc_t_conn_mode_oos_timer_ptr; /* Pointer Rex timer for OOS in connected mode */

#ifdef FEATURE_INACTIVITY_HANDLING

LOCAL rex_timer_type rrc_t_inactivity_timer; /* Rex timer for inactivity handling */
LOCAL rex_timer_type *rrc_t_inactivity_timer_ptr; /* Pointer Rex timer for inactivity handling */

#endif /* FEATURE_INTACTIVITY_HANDLING */

LOCAL rex_timer_type rrc_t_wtow_bplmn_srch;        /* Rex Timer for 3G->3G BPLMN DRX tmr expiry */
LOCAL rex_timer_type *rrc_t_wtow_bplmn_srch_ptr;   /* Pointer Rex timer 3G->3G DRX tmr expiry */

LOCAL rex_timer_type rrc_t_wtow_bplmn_guard_srch;      /* Rex Timer for MAX 3G->3G,2G BPLMN search */
LOCAL rex_timer_type *rrc_t_wtow_bplmn_guard_srch_ptr; /* Pointer Rex timer MAX 3G->3G,2G BPLMN search */

LOCAL rex_timer_type rrc_sixhr_sib_update_timer; /* Rex timer for six hrs sib clean up handling */
LOCAL rex_timer_type *rrc_sixhr_sib_update_timer_ptr; /* Pointer Rex timer for six hrs sib clean up handling */

LOCAL rex_timer_type rrc_tenmin_sib_update_timer;
LOCAL rex_timer_type *rrc_tenmin_sib_update_timer_ptr;

LOCAL rex_timer_type rrc_cho_act_rsp_ef_timer;
LOCAL rex_timer_type *rrc_cho_act_rsp_ef_timer_ptr;

LOCAL rex_timer_type rrc_wt_state_change_ef_timer;
LOCAL rex_timer_type *rrc_wt_state_change_ef_timer_ptr;

#ifdef FEATURE_EOOS
#error code not present
#endif

LOCAL rex_timer_type rrc_t_conn_mode_oos_initial_no_ds_timer; /* Rex timer for initial Tmins continuous search when 
                                                                 UE is OOS in connected mode */
LOCAL rex_timer_type *rrc_t_conn_mode_oos_initial_no_ds_timer_ptr; /* Pointer Rex timer for initial Tmins 
                                                                      continuous search */
LOCAL rex_timer_type rrc_t_conn_mode_oos_ds_timer; /* Rex timer for OOS in connected mode deep sleep*/
LOCAL rex_timer_type *rrc_t_conn_mode_oos_ds_timer_ptr; /* Pointer Rex timer for OOS in connected mode deep sleep */


LOCAL rex_timer_type rrc_sib_change_guard_timer; /* Rex timer for sib change guarding with BPLMN*/
LOCAL rex_timer_type *rrc_sib_change_guard_timer_ptr; /* Pointer Rex timer for sib change guarding with BPLMN*/

LOCAL rex_timer_type rrc_bcch_mod_timer; /* Rex timer for BCCH Modification timer handling */
LOCAL rex_timer_type *rrc_bcch_mod_timer_ptr; /* Pointer Rex timer for BCCH Modification timer handling  */

LOCAL rex_timer_type rrc_t_gtow_redirection_timer; /* Rex timer for GtoW redirection search */
LOCAL rex_timer_type *rrc_t_gtow_redirection_timer_ptr; /* Rex timer for GtoW redirection search */
LOCAL rex_timer_type rrc_t_gtow_redir_lfs_timer;
LOCAL rex_timer_type *rrc_t_gtow_redir_lfs_timer_ptr;

LOCAL rex_timer_type rrc_t_wtog_abort_guard_timer;
LOCAL rex_timer_type * rrc_t_wtog_abort_guard_timer_ptr;


LOCAL rex_timer_type rrc_t_319;             /* Rex timer T319 */
LOCAL rex_timer_type *rrc_t_319_ptr;        /* Pointer Rex timer T319 */

LOCAL rex_timer_type rrc_t_320;             /* Rex timer T320 */
LOCAL rex_timer_type *rrc_t_320_ptr;        /* Pointer Rex timer T320 */

#ifdef FEATURE_WCDMA_REL7_SPECRS

LOCAL rex_timer_type rrc_t_couec_timer;
LOCAL rex_timer_type *rrc_t_couec_timer_ptr;
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

#ifdef FEATURE_DUAL_SIM
LOCAL rex_timer_type rrc_wrm_lock_timer;
LOCAL rex_timer_type *rrc_wrm_lock_timer_ptr;
LOCAL rex_timer_type rrc_wrm_delay_preemption_timer;
LOCAL rex_timer_type *rrc_wrm_delay_preemption_timer_ptr;
LOCAL timer_type rrc_dsim_bplmn_lock_rel_off_timer;
LOCAL timer_type *rrc_dsim_bplmn_lock_rel_off_timer_ptr;
LOCAL rex_timer_type rrc_dsim_wrm_unlock_by_timer;
LOCAL rex_timer_type *rrc_dsim_wrm_unlock_by_timer_ptr;
LOCAL rex_timer_type rrc_get_ta_duration_timer;
LOCAL rex_timer_type *rrc_get_ta_duration_timer_ptr;
#endif

#ifdef FEATURE_EOOS
#error code not present
#endif

LOCAL rex_timer_type rrc_t_323;             /* Rex timer T323 */
LOCAL rex_timer_type *rrc_t_323_ptr;        /* Pointer Rex timer T323 */

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
LOCAL rex_timer_type rrc_t_ded_pri_validity_timer;
LOCAL rex_timer_type *rrc_t_ded_pri_validity_timer_ptr;
#endif

#ifdef FEATURE_QCHAT
LOCAL rex_timer_type qchat_fa_rrc_timer;      
LOCAL rex_timer_type *qchat_fa_rrc_timer_ptr;
#endif /* FEATURE_QCHAT */

#ifdef FEATURE_LTE_TO_WCDMA
LOCAL rex_timer_type rrc_t_lte_to_wcdma_redir_search_timer;
LOCAL rex_timer_type *rrc_t_lte_to_wcdma_redir_search_timer_ptr;
#endif

#ifdef FEATURE_FEMTO_CSG
LOCAL rex_timer_type rrc_t_csg_asf_srch_timer;
LOCAL rex_timer_type *rrc_t_csg_asf_srch_timer_ptr;
#endif

LOCAL rex_timer_type rrc_t_rrccu_trans_to_disconn_debug_timer;
LOCAL rex_timer_type *rrc_t_rrccu_trans_to_disconn_debug_timer_ptr;

LOCAL rex_timer_type rrc_t_bplmn_partial_report_timer;
LOCAL rex_timer_type *rrc_t_bplmn_partial_report_timer_ptr;

LOCAL rex_timer_type rrc_t_rrc_waiting_for_L2_ack_for_pch;
LOCAL rex_timer_type *rrc_t_rrc_waiting_for_L2_ack_for_pch_ptr;

#ifdef FEATURE_DUAL_SIM
/*Define non deferrable timer group for RRC*/
timer_group_type rrc_timer_group_non_deferrable;
#endif
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
LOCAL rex_timer_type rrc_t_rrc_dch_activation_time_buffer_ota;
LOCAL rex_timer_type *rrc_t_rrc_dch_activation_time_buffer_ota_ptr;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
/*===========================================================================
** -----------------------------------------------------------------------------------
**-- ----------NON-Demand paging section Srart--------------------------------------------
** -----------------------------------------------------------------------------------
  This section will have code section that will not be demand paged. Function which should be in this section are
  -RRC functions that are called by L1/L2 which are higher priority then RRC
  -RRC functions that are called in ISR context or RTOS timer call back
  -RRC functions that indirectly gets called by L1/L2 /ISR/timer call back   
  For example: Fun1() gets called by L1. Fun2() is called by Fun1(). Now both  Fun1() & Fun2() should be NON demand paged, 
  so both should be added in this section  
  Usage of pragma 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void foo(void)
  {
    function body here
  }
        __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  If you have another function to be added in the same section then again it needs to be wrapped with these pragma 
  for example:- function bar() is just below function foo() then function bar() needs to be written like this …. 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void bar(void)
  {
    function body here
  }
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
** ----------------------------------------------------------------------- */

#ifdef FEATURE_DUAL_SIM
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
void rrc_def_non_defer_timer(timer_type *p_timer, rrc_non_def_timer_cb_ptr timer_cb_ptr, unsigned long param)
{
   timer_group_set_deferrable( &rrc_timer_group_non_deferrable, FALSE );
   timer_def(p_timer, &rrc_timer_group_non_deferrable, NULL, 0, timer_cb_ptr, param);   
}
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
)
{
   timer_set(p_timer, cnt, 0, T_MSEC);
}
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
void rrc_clr_non_defer_timer(timer_type *p_timer)
{
   timer_clr(p_timer, T_MSEC);
}
/*===========================================================================

FUNCTION tdsrrc_get_non_defer_timer

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
)
{
  return timer_get(p_timer, T_MSEC);
}
#endif

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
 /***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrctmr_timers_callback
(
   unsigned long param                   /* Parameter to Identify timer */                               
)
{
  rrc_cmd_type *cmd_ptr;                     /* Pointer to RRC Command */

  rrclog_rrc_timer_expired_event_type rrc_timer_expired_event;  
#ifdef FEATURE_FEMTO_CSG
  rrclog_wcdma_asf_timer_status_event_type asf_timer_status;
#endif
#ifdef FEATURE_DUAL_SIM
  if(param == RRCTMR_WRM_LOCK_TIMER)
  {
    /*Set TRM Signal, so that RRC is unblocked*/
    (void) rex_set_sigs( wcdma_rrc_get_tcb(), RRC_WRM_SIG);
    WRRC_MSG1_HIGH("Expired timer:%d",param);
    return ;
  }
#endif



  if( (cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
  {   
    switch(param)
    { 
      case RRCTMR_T_300_TIMER: 
        /* Fill in the command parameters to RRC_T300_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_T300_EXPIRED_IND;
        break;

      case RRCTMR_T_308_TIMER: 
        /* Fill in the command parameters to RRC_T308_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_T308_EXPIRED_IND;
        break;

      case RRCTMR_T_304_TIMER: 
        /* Fill in the command parameters to RRC_T304_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_T304_EXPIRED_IND;
        break;

      case RRCTMR_WAIT_TIME_TIMER:
        /* Fill in the command parameters to RRC_WAIT_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_WAIT_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_PHY_CHAN_FAILURE_TIMER:
        /* Fill in the command parameters to RRC_WAIT_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_PHY_CHAN_FAILURE_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_UE_TO_CAMP_ON_TIMER:
        /* Fill in the command parameters to RRC_TIMER_TO_CAMP_ON_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_TIMER_TO_CAMP_ON_EXPIRED_IND;
        break;

      case RRCTMR_RRC_CON_DELAY_TIMER:
        /* Fill in the command parameters to RRC_SIG_CON_DELAY_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_RRC_CON_DELAY_TIMER_EXPIRED_IND;
        break;
      case RRCTMR_RCE_L2ACK_TIMER:
         /* Fill in the command parameters to RRC_TIMER_FOR_L2ACK_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_TIMER_FOR_RCE_L2ACK_EXPIRED_IND;
        break;

      case RRCTMR_RCR_L2ACK_TIMER:
         /* Fill in the command parameters to RRC_TIMER_FOR_L2ACK_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_TIMER_FOR_RCR_L2ACK_EXPIRED_IND;
        break;

      case RRCTMR_RSSI_BER_TIMER:
         /* Fill in the Command Id to RRC_TIMER_FOR_RSSI_BER_EXPIRED_IND */
        cmd_ptr->cmd_hdr.cmd_id = RRC_TIMER_FOR_RSSI_BER_EXPIRED_IND;
        break;

      case RRCTMR_SIB_WAIT_TIMER:
         /* Fill in the Command Id to RRC_SIB_WAIT_TIMER_EXPIRED_IND */
        cmd_ptr->cmd_hdr.cmd_id = RRC_SIB_WAIT_TIMER_EXPIRED_IND;
        break;

#ifdef FEATURE_DUAL_SIM
      case RRCTMR_SIB_STOP_PARTITION_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_SIB_STOP_PARTITION_TMR_EXPIRED_IND;
        break;

      case RRCTMR_SIB_MASK_INVERT_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_SIB_MASK_INVERT_TMR_EXPIRED_IND;
        break;

      case RRCTMR_SIB_EMPTY_MASK_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_SIB_EMPTY_MASK_TMR_EXPIRED_IND;
        break;
#endif

      case RRCTMR_T_302_TIMER: 
        /* Fill in the command parameters to RRC_T302_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_T302_EXPIRED_IND;
        break;

      case RRCTMR_T_305_TIMER: 
        /* Fill in the command parameters to RRC_T305_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_T305_EXPIRED_IND;
        break;

      case RRCTMR_T_316_TIMER: 
        /* Fill in the command parameters to RRC_T316_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_T316_EXPIRED_IND;
        break;

      case RRCTMR_T_317_TIMER: 
        /* Fill in the command parameters to RRC_T317_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_T317_EXPIRED_IND;
        break;

      case RRCTMR_T_307_TIMER: 
        /* Fill in the command parameters to RRC_T307_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_T307_EXPIRED_IND;
        break;

      case RRCTMR_T_314_TIMER: 
        /* Fill in the command parameters to RRC_T314_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_T314_EXPIRED_IND;

        //EVENT: Event_Timer_Expired
        rrc_timer_expired_event.rrc_timer_id = (uint8) T_314_TIMER;
        event_report_payload(EVENT_WCDMA_RRC_TIMER_EXPIRED, sizeof(rrc_timer_expired_event),
                       (void *)&rrc_timer_expired_event);


        break;

      case RRCTMR_T_315_TIMER: 
        /* Fill in the command parameters to RRC_T315_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_T315_EXPIRED_IND;

        //EVENT: Event_Timer_Expired
        rrc_timer_expired_event.rrc_timer_id = (uint8) T_315_TIMER;
        event_report_payload(EVENT_WCDMA_RRC_TIMER_EXPIRED, sizeof(rrc_timer_expired_event),
                       (void *)&rrc_timer_expired_event);

        break;

      case RRCTMR_SIB7_EXP_TIMER: 
        /* Fill in the command parameters to RRC_SIB7_EXP_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_SIB7_EXP_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_IGNORE_BCCH_MOD_TIMER: 
        /* Fill in the command parameters to RRC_IGNORE_BCCH_MOD_TIMER_EXPIRED_IND
           Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_IGNORE_BCCH_MOD_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_DCH_FACH_CELL_SELECTION_TIMER:
        /* Fill in the command parameters to RRC_DCH_FACH_CELL_SELECTION_TIMER_EXPIRED_IND
           Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_DCH_FACH_CELL_SELECTION_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER:
        /* Fill in the command parameters to RRC_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND
           Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_DCH_PCH_CELL_SELECTION_TIMER:
        /* Fill in the command parameters to RRC_DCH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND
           Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_DCH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_L1_DEADLOCK_DETECT_TIMER:
        /* Fill in the command parameters to RRC_DCH_FACH_CELL_SELECTION_TIMER_EXPIRED_IND
           Command */
        cmd_ptr->cmd_hdr.cmd_id = RRCTMR_L1_DEADLOCK_DETECT_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_DELAY_ERR_FATAL_TIMER:
        /* Fill in the command parameters to RRCTMR_DELAY_ERR_FATAL_TIMER_EXPIRED_IND
           Command */
        cmd_ptr->cmd_hdr.cmd_id = RRCTMR_DELAY_ERR_FATAL_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_T_3174_TIMER: 
        /* Fill in the command parameters to RRC_T3174_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_T3174_EXPIRED_IND;
        break;

      case RRCTMR_FREQ_SCAN_TIMER: 
        /* Fill in the command parameters to RRC_FREQ_SCAN_TIMER__EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_FREQ_SCAN_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_T_OOS_TIMER: 
        /* Fill in the command parameters to RRC_FREQ_SCAN_TIMER__EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_OOS_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_DEEP_SLEEP_NO_SVC_TIMER: 
          /* Fill in the command parameters to RRC_DEEP_SLEEP_NO_SVC_TIMER_EXPIRED_IND Command */
          cmd_ptr->cmd_hdr.cmd_id = RRC_DEEP_SLEEP_NO_SVC_TIMER_EXPIRED_IND;
          break;

      case RRCTMR_BPLMN_SRCH_TIMER:
        /* Fill in the command parameters to RRC_BPLMN_SRCH_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_BPLMN_SRCH_TIMER_EXPIRED_IND;

        //EVENT: Event_BPLMN_Timer_Expired
        rrc_timer_expired_event.rrc_timer_id = (uint8) BPLMN_SRCH_TIMER;
        event_report_payload(EVENT_WCDMA_RRC_TIMER_EXPIRED, sizeof(rrc_timer_expired_event),
                       (void *)&rrc_timer_expired_event);

        break;
        
      case RRCTMR_BPLMN_FREQ_SCAN_TIMER:
        /* Fill in the command parameters to RRC_BPLMN_FREQ_SCAN_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_BPLMN_FREQ_SCAN_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_MANUAL_PLMN_FREQ_SCAN_TIMER:
        /* Fill in the command parameters to RRC_MANUAL_PLMN_FREQ_SCAN_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_MANUAL_PLMN_FREQ_SCAN_TIMER_EXPIRED_IND;
        break;


      case RRCTMR_FREQ_REDIRECT_WAIT_TIMER:
        /* Fill in the command parameters to RRC_FREQ_REDIRECTION_WAIT_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_FREQ_REDIRECT_WAIT_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_CONN_MODE_OOS_TIMER:
        /* Fill in the command parameters to RRC_CONN_MODE_OOS_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_CONN_MODE_OOS_TIMER_EXPIRED_IND;
        break;

#ifdef FEATURE_INACTIVITY_HANDLING
      case RRCTMR_INACTIVITY_TIMER:
        /* Fill in the command parameters to RRC_CONN_MODE_OOS_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_INACTIVITY_TIMER_EXPIRED_IND;
        break;
#endif /* FEATURE_INTACTIVITY_HANDLING */

      case RRCTMR_WTOW_BPLMN_DRX_SRCH_TIMER:
        /* Fill in the command parameters to RRC_WTOW_BPLMN_SRCH_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_WTOW_BPLMN_DRX_SRCH_TIMER_EXPIRED_IND;
        break;
  
      case RRCTMR_BPLMN_GUARD_SRCH_TIMER:
        /* Fill in the command parameters to RRC_WTOW_BPLMN_GUARD_SRCH_TIMER Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_BPLMN_GUARD_SRCH_TIMER_EXPIRED_IND;
        break;

      case  RRCTMR_SIXHR_SIB_UPDATE_TIMER:
        /* Fill in the command parameters to RRC_SIX_HRS_SIB_TIMER_UPDATE_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_SIX_HRS_SIB_TIMER_UPDATE_IND;
        break;

      case  RRCTMR_TENMIN_SIB_UPDATE_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_TEN_MIN_SIB_TIMER_UPDATE_IND;
        break;

#ifdef FEATURE_EOOS
      #error code not present
#endif

#ifdef FEATURE_EOOS 
      #error code not present
#endif

#ifdef FEATURE_DUAL_SIM 
      case RRCTMR_WRM_UNLOCK_BY_TIMER:
        /* Fill in the command parameters to RRCTMR_EOOS_FULL_SCAN_TIMER Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_DUAL_SIM_WRM_UNLOCK_BY_TIMER_EXP_IND;
        break;
#endif


      case RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER:
        /* Fill in the command parameters to RRC_CONN_MODE_OOS_INITIAL_NO_DS_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_CONN_MODE_OOS_INITIAL_NO_DS_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_CONN_MODE_OOS_DS_TIMER:
        /* Fill in the command parameters to RRC_CONN_MODE_OOS_DS_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_CONN_MODE_OOS_DS_TIMER_EXPIRED_IND;
        break;      

      case  RRC_SIB_CHANGE_GUARD_TIMER:
        /* Fill in the command parameters to RRC_SIB_CHANGE_GUARD_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_SIB_CHANGE_GUARD_TIMER_EXPIRED_IND;
        break;

      case RRC_BCCH_MODIFICATION_TIMER:
        /* Fill in the command parameters to RRC_BCCH_MODIFICATION_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_BCCH_MODIFICATION_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_GTOW_REDIRECTION_WAIT_TIMER:
        /* Fill in the command parameters to RRC_GTOW_REDIRECTION_WAIT_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_GTOW_REDIRECTION_WAIT_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_GTOW_REDIRECTION_LFS_TIMER:
        /* Fill in the command parameters to RRC_GTOW_REDIRECTION_LFS_TIMER_EXPIRED_IND Command */
        cmd_ptr->cmd_hdr.cmd_id = RRC_GTOW_REDIRECTION_LFS_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_WTOG_ABORT_GUARD_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_WTOG_ABORT_GUARD_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_T_319_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_T319_EXPIRED_IND;
        break;


      case RRCTMR_T_320_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_T320_EXPIRED_IND;
        break;	

#ifdef FEATURE_WCDMA_REL7_SPECRS
      case RRCTMR_COUEC_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_TIMER_COUEC_EXPIRED_IND;
        break;	
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

      case RRCTMR_T_323_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_T323_EXPIRED_IND;
        break;

#ifdef FEATURE_DUAL_SIM
      case RRCTMR_DSIM_BPLMN_LOCK_REL_OFFSET_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_BPLMN_SEARCH_RESUME_IND;
        WRRC_MSG0_HIGH("RRC_BPLMN_SEARCH_RESUME_IND sent to CSP");
        break;
#endif

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
      case RRCTMR_DED_PRI_VALIDITY_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_DED_PRI_VALDITY_TIMER_EXPIRED_IND;
        event_report(EVENT_WCDMA_DED_PRIORITIES_VALIDITY_TIMER_EXPIRED);
        break;
#endif
#ifdef FEATURE_QCHAT
      case RRCTMR_QCHAT_FA_RRC_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_QCHAT_FA_RRC_TIMER_EXPIRED_IND;
        break; 
#endif /* FEATURE_QCHAT */
#ifdef FEATURE_LTE_TO_WCDMA
      case RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_LTE_TO_WCDMA_SRCH_TIMER_EXPIRY_IND;
        WRRC_MSG0_HIGH("RRC_LTE_TO_WCDMA_SRCH_TIMER_EXPIRY_IND posted to CSP");
        break;
#endif

#ifdef FEATURE_DUAL_SIM
      case RRCTMR_WRM_DELAY_PREEMPTION_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_WRM_PREEMPT_IND;
        WRRC_MSG0_HIGH("DSIM: RRC_WRM_PREEMPT_IND posted to CSP");
        break;
#endif

#ifdef FEATURE_FEMTO_CSG
      case RRCTMR_CSG_ASF_SRCH_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_CSG_ASF_SRCH_TIMER_EXPIRY_IND;
        WRRC_MSG0_HIGH("RRC_CSG_ASF_SRCH_TIMER_EXPIRY_IND posted to CSP");
        asf_timer_status.timer_value = 0;
        asf_timer_status.timer_state = 2;
        event_report_payload(EVENT_WCDMA_RRC_ASF_TIMER_STATUS, sizeof(rrclog_wcdma_asf_timer_status_event_type),
                     (void*)&asf_timer_status);
        break;
#endif
      case RRCTMR_RRCCU_TRANS_TO_DISCON_TIMER: 
        cmd_ptr->cmd_hdr.cmd_id = RRC_CU_TRANS_TO_DISCON_TIMER_EXPIRED;
        break;

      case RRCTMR_BPLMN_PARTIAL_REPORT_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRC_BPLMN_PARTIAL_REPORT_TIMER_EXPIRED_IND;
        break;
      case RRCTMR_WAIT_FOR_L2_ACK_FOR_PCH_TRANSITION_TIMER: 
        WRRC_MSG0_ERROR("RRCTMR_WAIT_FOR_L2_ACK_FOR_PCH_TRANSITION_TIMER expired");
        break;

      case RRCTMR_ACT_RSP_EF_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRCTMR_ACT_RSP_EF_TIMER_EXPIRED_IND;
        break;

      case RRCTMR_WT_STATE_CHANGE_EF_TIMER:
        cmd_ptr->cmd_hdr.cmd_id = RRCTMR_WT_STATE_CHANGE_EF_TIMER_EXPIRED_IND;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
      case RRCTMR_OTA_BUFFER_ACTIVATION_TIMER: 
        if (RRC_CSP_UNLOCK_BY_SCAN == rrc_csp_int_data.curr_scan)
        {
          WRRC_MSG0_HIGH("BUF_OTA:Activation timer expiry is pended as unlock by scan is in progress");
          rrcdata_misc_info.rrc_buffered_ota_activation_time_expired_during_lta = TRUE;
        }
        cmd_ptr->cmd_hdr.cmd_id = RRC_ACTIVATION_TIMER_EXPIRY_FOR_BUFFER_OTA;
        break;
 #endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/         
      default:
        WRRC_MSG0_ERROR("ERR Unknown RRC timer received!");
        break;
    }
 
  /* sends an internal command to RRC */
    WRRC_MSG1_HIGH("Expired timer:%d",param);
    rrc_put_int_cmd(cmd_ptr);         
  }
} /* rrctmr_timers_callback */

/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
/*===========================================================================

FUNCTION rrctmr_create_timers

DESCRIPTION
  This function will create all RRC  related timers.
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
 /***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrctmr_create_timers
(
  void
)
 
{
  /* Create Timer T300 */
  rrc_t_300_ptr = &rrc_t_300;
  rex_def_timer_ex( rrc_t_300_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_T_300_TIMER );

  /* Create Timer T308 */
  rrc_t_308_ptr = &rrc_t_308;
  rex_def_timer_ex( rrc_t_308_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_T_308_TIMER );

  /* Create Timer T304 */
  rrc_t_304_ptr = &rrc_t_304;
  rex_def_timer_ex( rrc_t_304_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_T_304_TIMER );

  /* Create Timer for Wait time */
  rrc_t_wait_time_ptr = &rrc_t_wait_time;
  rex_def_timer_ex( rrc_t_wait_time_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_WAIT_TIME_TIMER );

  /* Create Timer for UE to camp on */ 
  rrc_t_ue_to_camp_on_ptr = &rrc_t_ue_to_camp_on;
  rex_def_timer_ex( rrc_t_ue_to_camp_on_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_UE_TO_CAMP_ON_TIMER );

  /* Create Timer for UE to camp on */ 
  rrc_t_rrc_con_delay_ptr = &rrc_t_rrc_con_delay;
  rex_def_timer_ex( rrc_t_rrc_con_delay_ptr,
                    rrctmr_timers_callback,
                    (uint32) RRCTMR_RRC_CON_DELAY_TIMER );
  /* Create RCE timer for L2 ACK */
  rrc_t_rce_l2ack_ptr = &rrc_t_rce_l2ack;
  rex_def_timer_ex( rrc_t_rce_l2ack_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_RCE_L2ACK_TIMER );

  /* Create RCR timer for L2 ACK */
  rrc_t_rcr_l2ack_ptr = &rrc_t_rcr_l2ack;
  rex_def_timer_ex( rrc_t_rcr_l2ack_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_RCR_L2ACK_TIMER );

  /* Create reporting RSSI_BER timer */
  rrc_t_rssi_ber_ptr = &rrc_t_rssi_ber;
  rex_def_timer_ex( rrc_t_rssi_ber_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_RSSI_BER_TIMER );

  /* Create reporting SIB wait timer */
  rrc_t_sib_wait_ptr = &rrc_t_sib_wait;
  rex_def_timer_ex( rrc_t_sib_wait_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_SIB_WAIT_TIMER );

#ifdef FEATURE_DUAL_SIM
  rrc_t_sib_stop_part_ptr = &rrc_t_sib_stop_part;
  rex_def_timer_ex( rrc_t_sib_stop_part_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_SIB_STOP_PARTITION_TIMER );

  rrc_t_sib_mask_ptr = &rrc_t_sib_mask;
  rex_def_timer_ex( rrc_t_sib_mask_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_SIB_MASK_INVERT_TIMER );

  rrc_t_sib_empty_mask_ptr = &rrc_t_sib_empty_mask;
  rex_def_timer_ex( rrc_t_sib_empty_mask_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_SIB_EMPTY_MASK_TIMER );
#endif

  /*Create SIB7 expiration timer*/
  rrc_t_sib7_expiration_ptr = &rrc_t_sib7_expiration;
  rex_def_timer_ex( rrc_t_sib7_expiration_ptr,
                    NULL,
                   (uint32) RRCTMR_SIB7_EXPIRATION_TIMER);

  /* Create Timer T302 */
  rrc_t_302_ptr = &rrc_t_302;
  rex_def_timer_ex( rrc_t_302_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_T_302_TIMER );

  /* Create Timer T305 */
  rrc_t_305_ptr = &rrc_t_305;
  rex_def_timer_ex( rrc_t_305_ptr,
                    rrctmr_timers_callback,
                  (uint32)  RRCTMR_T_305_TIMER );

  /* Create Timer T316 */
  rrc_t_316_ptr = &rrc_t_316;
  rex_def_timer_ex( rrc_t_316_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_T_316_TIMER );

  /* Create Timer T317 */
  rrc_t_317_ptr = &rrc_t_317;
  rex_def_timer_ex( rrc_t_317_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_T_317_TIMER );

  /* Create Timer T307 */
  rrc_t_307_ptr = &rrc_t_307;
  rex_def_timer_ex( rrc_t_307_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_T_307_TIMER );

  /* Create Timer T314 */
  rrc_t_314_ptr = &rrc_t_314;
  rex_def_timer_ex( rrc_t_314_ptr,
                    rrctmr_timers_callback,
                  (uint32)  RRCTMR_T_314_TIMER );

  /* Create Timer T315 */
  rrc_t_315_ptr = &rrc_t_315;
  rex_def_timer_ex( rrc_t_315_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_T_315_TIMER );

  /* Create Timer for SIB7 Expiration */
  rrc_t_sib7_exp_ptr = &rrc_t_sib7_exp;
  rex_def_timer_ex( rrc_t_sib7_exp_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_SIB7_EXP_TIMER );

  /* Create Timer to Ignore BCCH Modification Info */
  rrc_t_ignore_bcch_mod_exp_ptr = &rrc_t_ignore_bcch_mod_exp;
  rex_def_timer_ex( rrc_t_ignore_bcch_mod_exp_ptr,
                    rrctmr_timers_callback,
                  (uint32)  RRCTMR_IGNORE_BCCH_MOD_TIMER );

  /* Create Timer during Cell Selection, going DCH->FACH */
  rrc_t_dch_fach_cell_selection_exp_ptr = &rrc_t_dch_fach_cell_selection_exp;
  rex_def_timer_ex( rrc_t_dch_fach_cell_selection_exp_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_DCH_FACH_CELL_SELECTION_TIMER );

  /* Create Timer during Cell Selection, going FACH->FACH/PCH */
  rrc_t_fach_to_fach_pch_cell_selection_exp_ptr = &rrc_t_fach_to_fach_pch_cell_selection_exp;
  rex_def_timer_ex( rrc_t_fach_to_fach_pch_cell_selection_exp_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER );

  /* Create Timer to detect L1 deadlock */
  rrc_t_l1_deadlock_detection_exp_ptr = &rrc_t_l1_deadlock_detection_exp;
  rex_def_timer_ex( rrc_t_l1_deadlock_detection_exp_ptr,
                    rrctmr_timers_callback,
                  (uint32)  RRCTMR_L1_DEADLOCK_DETECT_TIMER );

  /* Create Timer to delay an ERR_FATAL */
  rrc_t_delay_err_fatal_exp_ptr = &rrc_t_delay_err_fatal_exp;
  rex_def_timer_ex( rrc_t_delay_err_fatal_exp_ptr,
                    rrctmr_timers_callback,
                   (uint32) RRCTMR_DELAY_ERR_FATAL_TIMER );

  /* Create Timer during Cell Selection, going DCH->PCH */
  rrc_t_dch_pch_cell_selection_exp_ptr = &rrc_t_dch_pch_cell_selection_exp;
  rex_def_timer_ex( rrc_t_dch_pch_cell_selection_exp_ptr,
                    rrctmr_timers_callback,
                 (uint32)   RRCTMR_DCH_PCH_CELL_SELECTION_TIMER );

  /* Create Timer for Wait time */
  rrc_t_phy_chan_failure_ptr = &rrc_t_phy_chan_failure;
  rex_def_timer_ex( rrc_t_phy_chan_failure_ptr,
                    rrctmr_timers_callback,
                 (uint32)   RRCTMR_PHY_CHAN_FAILURE_TIMER );

  /* Create Timer T3174 */
  rrc_t_3174_ptr = &rrc_t_3174;
  rex_def_timer_ex( rrc_t_3174_ptr,
                    rrctmr_timers_callback,
                  (uint32)  RRCTMR_T_3174_TIMER );

  /* Create Timer for frequency scan */
  rrc_t_freq_scan_exp_ptr = &rrc_t_freq_scan_exp;
  rex_def_timer_ex( rrc_t_freq_scan_exp_ptr,
                    rrctmr_timers_callback,
                 (uint32)   RRCTMR_FREQ_SCAN_TIMER );

  /* Create Timer for frequency scan */
  rrc_t_oos_ptr = &rrc_t_oos;
  rex_def_timer_ex( rrc_t_oos_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_T_OOS_TIMER );

  /* Create Timer for returning no service if consecutive service requests
   * are received in deep sleep before this timer expires.
   */
  rrc_t_deep_sleep_no_svc_ptr = &rrc_t_deep_sleep_no_svc;
  rex_def_timer_ex( rrc_t_deep_sleep_no_svc_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_DEEP_SLEEP_NO_SVC_TIMER );

   /* Create Timer for BPLMN full freq scan */
  rrc_t_bplmn_srch_ptr = &rrc_t_bplmn_srch;
  rex_def_timer_ex( rrc_t_bplmn_srch_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_BPLMN_SRCH_TIMER );

  /* Create Timer for BPLMN full frequency scan */
  rrc_t_bplmn_full_scan_srch_ptr = &rrc_t_bplmn_full_scan_srch;
  rex_def_timer_ex( rrc_t_bplmn_full_scan_srch_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_BPLMN_FREQ_SCAN_TIMER );

  /* Create Timer for Manual search full frequency scan */
  rrc_t_manual_plmn_full_scan_srch_ptr = &rrc_t_manual_plmn_full_scan_srch;
  rex_def_timer_ex( rrc_t_manual_plmn_full_scan_srch_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_MANUAL_PLMN_FREQ_SCAN_TIMER );


  /* Create Timer for Freq Redirect Wait Timer */
  rrc_t_freq_redirect_wait_timer_ptr = &rrc_t_freq_redirect_wait_timer;
  rex_def_timer_ex( rrc_t_freq_redirect_wait_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_FREQ_REDIRECT_WAIT_TIMER );

  /* Create Timer for OOS in Connected Mode handling */
  rrc_t_conn_mode_oos_timer_ptr = &rrc_t_conn_mode_oos_timer;
  rex_def_timer_ex( rrc_t_conn_mode_oos_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_CONN_MODE_OOS_TIMER );

#ifdef FEATURE_INACTIVITY_HANDLING
  rrc_t_inactivity_timer_ptr = &rrc_t_inactivity_timer;
  rex_def_timer_ex( rrc_t_inactivity_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_INACTIVITY_TIMER );
#endif /* FEATURE_INTACTIVITY_HANDLING */

  /* Create Timer for WTOW BPLMN drx search */
  rrc_t_wtow_bplmn_srch_ptr = &rrc_t_wtow_bplmn_srch;
  rex_def_timer_ex( rrc_t_wtow_bplmn_srch_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_WTOW_BPLMN_DRX_SRCH_TIMER );

  /* Create Timer for WTOW/WTOG MAX BPLMN allowed search */
  rrc_t_wtow_bplmn_guard_srch_ptr = &rrc_t_wtow_bplmn_guard_srch;
  rex_def_timer_ex( rrc_t_wtow_bplmn_guard_srch_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_BPLMN_GUARD_SRCH_TIMER );

  /* Create Timer for Updating SIX Hours SIB Clean Up Timer*/
  rrc_sixhr_sib_update_timer_ptr = &rrc_sixhr_sib_update_timer;
  rex_def_timer_ex( rrc_sixhr_sib_update_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_SIXHR_SIB_UPDATE_TIMER);

  rrc_tenmin_sib_update_timer_ptr = &rrc_tenmin_sib_update_timer;
  rex_def_timer_ex( rrc_tenmin_sib_update_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_TENMIN_SIB_UPDATE_TIMER);


  rrc_cho_act_rsp_ef_timer_ptr = &rrc_cho_act_rsp_ef_timer;
  rex_def_timer_ex( rrc_cho_act_rsp_ef_timer_ptr,
                    NULL,
                    (uint32)RRCTMR_ACT_RSP_EF_TIMER);


  rrc_wt_state_change_ef_timer_ptr = &rrc_wt_state_change_ef_timer;
  rex_def_timer_ex( rrc_wt_state_change_ef_timer_ptr,
                    NULL,
                    (uint32)RRCTMR_WT_STATE_CHANGE_EF_TIMER);

#ifdef FEATURE_EOOS
  #error code not present
#endif

  /* Create Timer for Deep Sleep OOS in Connected Mode handling */
  rrc_t_conn_mode_oos_initial_no_ds_timer_ptr = &rrc_t_conn_mode_oos_initial_no_ds_timer;
  rex_def_timer_ex( rrc_t_conn_mode_oos_initial_no_ds_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER );

  /* Create Timer for Deep Sleep OOS in Connected Mode handling */
  rrc_t_conn_mode_oos_ds_timer_ptr = &rrc_t_conn_mode_oos_ds_timer;
  rex_def_timer_ex( rrc_t_conn_mode_oos_ds_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_CONN_MODE_OOS_DS_TIMER );


  /* Create Timer for SIB CHANGE GUARD Timer*/
  rrc_sib_change_guard_timer_ptr = &rrc_sib_change_guard_timer;
  rex_def_timer_ex( rrc_sib_change_guard_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRC_SIB_CHANGE_GUARD_TIMER);

  /* Create Timer for BCCH Modification Time*/
  rrc_bcch_mod_timer_ptr = &rrc_bcch_mod_timer;
  rex_def_timer_ex( rrc_bcch_mod_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRC_BCCH_MODIFICATION_TIMER);

  /* Create Timer for GtoW Redirection handling */
  rrc_t_gtow_redirection_timer_ptr = &rrc_t_gtow_redirection_timer;
  rex_def_timer_ex( rrc_t_gtow_redirection_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_GTOW_REDIRECTION_WAIT_TIMER );

  rrc_t_gtow_redir_lfs_timer_ptr = &rrc_t_gtow_redir_lfs_timer;
  rex_def_timer_ex( rrc_t_gtow_redir_lfs_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_GTOW_REDIRECTION_LFS_TIMER );


  rrc_t_wtog_abort_guard_timer_ptr = &rrc_t_wtog_abort_guard_timer;
  rex_def_timer_ex( rrc_t_wtog_abort_guard_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_WTOG_ABORT_GUARD_TIMER );


  rrc_t_319_ptr = &rrc_t_319;
  rex_def_timer_ex( rrc_t_319_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_T_319_TIMER);


  rrc_t_320_ptr = &rrc_t_320;
  rex_def_timer_ex( rrc_t_320_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_T_320_TIMER);
  
#ifdef FEATURE_WCDMA_REL7_SPECRS
 
  rrc_t_couec_timer_ptr = &rrc_t_couec_timer;
  rex_def_timer_ex( rrc_t_couec_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_COUEC_TIMER);
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

#ifdef FEATURE_DUAL_SIM
  rrc_wrm_lock_timer_ptr = &rrc_wrm_lock_timer;
  rex_def_timer_ex( rrc_wrm_lock_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_WRM_LOCK_TIMER);

  rrc_wrm_delay_preemption_timer_ptr = &rrc_wrm_delay_preemption_timer;
  rex_def_timer_ex( rrc_wrm_delay_preemption_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_WRM_DELAY_PREEMPTION_TIMER);


  rrc_dsim_bplmn_lock_rel_off_timer_ptr = &rrc_dsim_bplmn_lock_rel_off_timer;
  rrc_def_non_defer_timer( rrc_dsim_bplmn_lock_rel_off_timer_ptr,
                    rrctmr_timers_callback, 
                    (uint32)RRCTMR_DSIM_BPLMN_LOCK_REL_OFFSET_TIMER);

  rrc_dsim_wrm_unlock_by_timer_ptr = & rrc_dsim_wrm_unlock_by_timer;
  rex_def_timer_ex(rrc_dsim_wrm_unlock_by_timer_ptr,
                          rrctmr_timers_callback, 
                          (uint32)RRCTMR_WRM_UNLOCK_BY_TIMER);

  rrc_get_ta_duration_timer_ptr = &rrc_get_ta_duration_timer;
  rex_def_timer_ex( rrc_get_ta_duration_timer_ptr,
                    rrctmr_timers_callback, 
                    (uint32)RRCTMR_GET_TA_DURATION_TIMER);
#endif

  rrc_t_323_ptr = &rrc_t_323;
  rex_def_timer_ex( rrc_t_323_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_T_323_TIMER);
#ifdef FEATURE_EOOS 
  #error code not present
#endif

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
  rrc_t_ded_pri_validity_timer_ptr = &rrc_t_ded_pri_validity_timer;
  rex_def_timer_ex(rrc_t_ded_pri_validity_timer_ptr,
                              rrctmr_timers_callback,
                              (uint32)RRCTMR_DED_PRI_VALIDITY_TIMER);
#endif

#ifdef FEATURE_QCHAT
  qchat_fa_rrc_timer_ptr = &qchat_fa_rrc_timer;
  rex_def_timer_ex( qchat_fa_rrc_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_QCHAT_FA_RRC_TIMER);
#endif
#ifdef FEATURE_LTE_TO_WCDMA
  rrc_t_lte_to_wcdma_redir_search_timer_ptr = &rrc_t_lte_to_wcdma_redir_search_timer;
  rex_def_timer_ex(rrc_t_lte_to_wcdma_redir_search_timer_ptr,
                              rrctmr_timers_callback,
                              (uint32)RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER);
#endif

#ifdef FEATURE_FEMTO_CSG
  rrc_t_csg_asf_srch_timer_ptr = &rrc_t_csg_asf_srch_timer;
  rex_def_timer_ex( rrc_t_csg_asf_srch_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_CSG_ASF_SRCH_TIMER);
#endif

  rrc_t_rrccu_trans_to_disconn_debug_timer_ptr= &rrc_t_rrccu_trans_to_disconn_debug_timer;
  rex_def_timer_ex( rrc_t_rrccu_trans_to_disconn_debug_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_RRCCU_TRANS_TO_DISCON_TIMER );

  /* Create Timer for BPLMN Reporting*/
  rrc_t_bplmn_partial_report_timer_ptr = &rrc_t_bplmn_partial_report_timer;
  rex_def_timer_ex( rrc_t_bplmn_partial_report_timer_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_BPLMN_PARTIAL_REPORT_TIMER );

  rrc_t_rrc_waiting_for_L2_ack_for_pch_ptr= &rrc_t_rrc_waiting_for_L2_ack_for_pch;
  rex_def_timer_ex( rrc_t_rrc_waiting_for_L2_ack_for_pch_ptr,
                    rrctmr_timers_callback,
                    (uint32)RRCTMR_WAIT_FOR_L2_ACK_FOR_PCH_TRANSITION_TIMER );

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
  rrc_t_rrc_dch_activation_time_buffer_ota_ptr= &rrc_t_rrc_dch_activation_time_buffer_ota;
  rex_def_timer_ex( rrc_t_rrc_dch_activation_time_buffer_ota_ptr,
                  rrctmr_timers_callback,
                  (uint32)RRCTMR_OTA_BUFFER_ACTIVATION_TIMER );
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
} /* rrctmr_create_timers */

/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
/*===========================================================================

FUNCTION rrctmr_delete_timers

DESCRIPTION
  This function will delete all RRC related timers.
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
 /***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrctmr_delete_timers
(
  void
)
{

  #ifdef FEATURE_REX_DYNA_MEM 
  /* delete the timer rrc_t_300_ptr */ 
  rex_delete_timer_ex( rrc_t_300_ptr );

  /* delete the timer rrc_t_308_ptr */ 
  rex_delete_timer_ex( rrc_t_308_ptr );

  /* delete the timer rrc_t_304_ptr */ 
  rex_delete_timer_ex( rrc_t_304_ptr );

  /* delete the timer rrc_t_ue_to_camp_on_ptr */ 
  rex_delete_timer_ex( rrc_t_ue_to_camp_on_ptr );

  /* delete the timer rrc_t_wait_time_ptr */ 
  rex_delete_timer_ex( rrc_t_wait_time_ptr );

  /* delete the timer rrc_t_rce_l2ack_ptr */ 
  rex_delete_timer_ex( rrc_t_rce_l2ack_ptr );

  /* delete the timer rrc_t_rcr_l2ack_ptr */ 
  rex_delete_timer_ex( rrc_t_rcr_l2ack_ptr );

  /* delete the RSSI&BER report timer rrc_t_rssi_ber_ptr */
  rex_delete_timer_ex( rrc_t_rssi_ber_ptr );

  /* delete the timer rrc_t_sib_wait_ptr */
  rex_delete_timer_ex( rrc_t_sib_wait_ptr );

#ifdef FEATURE_DUAL_SIM
  rex_delete_timer_ex( rrc_t_sib_stop_part_ptr );

  rex_delete_timer_ex( rrc_t_sib_mask_ptr );

  rex_delete_timer_ex( rrc_t_sib_empty_mask_ptr );
#endif

  /* delete the timer rrc_t_sib7_expiration_ptr */
  rex_delete_timer_ex( rrc_t_sib7_expiration_ptr );


  /* delete the timer rrc_t_302_ptr */
  rex_delete_timer_ex( rrc_t_302_ptr );
 
 /* delete the timer rrc_t_305_ptr */
  rex_delete_timer_ex( rrc_t_305_ptr );

  /* delete the timer rrc_t_316_ptr */
  rex_delete_timer_ex( rrc_t_316_ptr );

  /* delete the timer rrc_t_317_ptr */
  rex_delete_timer_ex( rrc_t_317_ptr );
 
 /* delete the timer rrc_t_307_ptr */
  rex_delete_timer_ex( rrc_t_307_ptr );

  /* delete the timer rrc_t_314_ptr */
  rex_delete_timer_ex( rrc_t_314_ptr );

  /* delete the timer rrc_t_315_ptr */
  rex_delete_timer_ex( rrc_t_315_ptr );

  /* delete the timer rrc_t_sib7_exp_ptr */
  rex_delete_timer_ex( rrc_t_sib7_exp_ptr );

  /* delete the timer to ignore BCCH modification Info */
  rex_delete_timer_ex( rrc_t_ignore_bcch_mod_exp_ptr );

  /* delete the timer for DCH->FACH cell selection */
  rex_delete_timer_ex( rrc_t_dch_fach_cell_selection_exp_ptr );

  /* delete the timer for FACH->FACH/PCH cell selection */
  rex_delete_timer_ex( rrc_t_fach_to_fach_pch_cell_selection_exp_ptr );

  /* delete the timer for L1 deadlock detection */
  rex_delete_timer_ex( rrc_t_l1_deadlock_detection_exp_ptr );
  
  /* delete the timer for ERR_FATAL delay */
  rex_delete_timer_ex( rrc_t_delay_err_fatal_exp_ptr );

  /* delete the timer for DCH->PCH cell selection */
  rex_delete_timer_ex( rrc_t_dch_pch_cell_selection_exp_ptr );

  /*delete GtoW PCCO timer */
  rex_delete_timer_ex( rrc_t_3174_ptr );

  /*delete Freq Scan timer */
  rex_delete_timer_ex( rrc_t_freq_scan_exp_ptr );
  /*delete rrcconn rel delay timer */
  rex_delete_timer_ex( rrc_t_rrc_con_delay_ptr );

  /*delete Deep Sleep No Service timer */
  rex_delete_timer_ex( rrc_t_deep_sleep_no_svc_ptr );

  /*delete BPLMN 2G->3G search timer */
  rex_delete_timer_ex( rrc_t_bplmn_srch_ptr );

  /*delete BPLMN 2G->3G full scan timer */
  rex_delete_timer_ex( rrc_t_bplmn_full_scan_srch_ptr );

  /*delete Manual PLMN full scan timer */
  rex_delete_timer_ex( rrc_t_manual_plmn_full_scan_srch_ptr );


  /*delete inter frequency redirection scan timer */
  rex_delete_timer_ex( rrc_t_freq_redirect_wait_timer_ptr );

  /*delete OOS Connected Mode timer */
  rex_delete_timer_ex( rrc_t_conn_mode_oos_timer_ptr );
  
  /*delete BPLMN 3G->3G drx search timer */
  rex_delete_timer_ex( rrc_t_wtow_bplmn_srch_ptr );

  /*delete MAX BPLMN 3G->3G, 2G search timer */
  rex_delete_timer_ex( rrc_t_wtow_bplmn_guard_srch_ptr );

  /*delete six hrs sib clean up timer*/
  rex_delete_timer_ex(rrc_sixhr_sib_update_timer_ptr); 

  rex_delete_timer_ex(rrc_tenmin_sib_update_timer_ptr);

  rex_delete_timer_ex(rrc_cho_act_rsp_ef_timer_ptr);

  rex_delete_timer_ex(rrc_wt_state_change_ef_timer_ptr);

#ifdef FEATURE_EOOS
  #error code not present
#endif

  /*delete OOS Connected Mode timer */
  rex_delete_timer_ex( rrc_t_conn_mode_oos_initial_no_ds_timer_ptr );

  /*delete DS OOS Connected Mode timer */
  rex_delete_timer_ex( rrc_t_conn_mode_oos_ds_timer_ptr );


  /*delete Sib change guard timer */
  rex_delete_timer_ex(rrc_sib_change_guard_timer_ptr); 

  /*delete BCCH Modification timer */
  rex_delete_timer_ex(rrc_bcch_mod_timer_ptr);

  /*delete GtoW Redirection timer */
  rex_delete_timer_ex( rrc_t_gtow_redirection_timer_ptr );


  rex_delete_timer_ex( rrc_t_wtog_abort_guard_timer_ptr );



  /*delete UTRAN DRX COEF timer */
  rex_delete_timer_ex( rrc_t_319_ptr );

  rex_delete_timer_ex(rrc_t_320_ptr);

#ifdef FEATURE_WCDMA_REL7_SPECRS
  rex_delete_timer_ex(rrc_t_couec_timer_ptr);
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

  rex_delete_timer_ex(rrc_t_323_ptr);

#ifdef FEATURE_QCHAT
  rex_delete_timer_ex(qchat_fa_rrc_timer_ptr);
#endif
#ifdef FEATURE_LTE_TO_WCDMA
  rex_delete_timer_ex(rrc_t_lte_to_wcdma_redir_search_timer_ptr);
#endif

#ifdef FEATURE_FEMTO_CSG
  rex_delete_timer_ex(rrc_t_csg_asf_srch_timer_ptr);
#endif

#endif
rex_delete_timer_ex(rrc_t_rrccu_trans_to_disconn_debug_timer_ptr);
rex_delete_timer_ex(rrc_t_rrc_waiting_for_L2_ack_for_pch_ptr);

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
rex_delete_timer_ex(rrc_t_rrc_dch_activation_time_buffer_ota_ptr);
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
} /* rrctmr_delete_timers */

/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
/*===========================================================================

FUNCTION rrctmr_start_timer

DESCRIPTION
  This function will set the timer value in milli seconds and starts the timer
  timers.
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrctmr_start_timer
(
  rrctmr_timer_e_type timer,                /* Timer to start */
  unsigned long timer_value                 /* Value for timer */                      
)
{
#ifdef FEATURE_EOOS
  #error code not present
#endif

#ifdef FEATURE_FEMTO_CSG
  rrclog_wcdma_asf_timer_status_event_type asf_timer_status;
#endif

#ifdef FEATURE_TIMER_DEBUG
  #error code not present
#endif

  switch(timer)
  {
    case RRCTMR_T_300_TIMER:
      rex_set_timer(rrc_t_300_ptr, timer_value); 
      break;
    case RRCTMR_T_308_TIMER:
      rex_set_timer(rrc_t_308_ptr, timer_value); 
      break;
    case RRCTMR_T_304_TIMER:
      rex_set_timer(rrc_t_304_ptr, timer_value); 
      break;
    case RRCTMR_WAIT_TIME_TIMER:
      rex_set_timer(rrc_t_wait_time_ptr, timer_value);
      break;
    case RRCTMR_UE_TO_CAMP_ON_TIMER:
      rex_set_timer(rrc_t_ue_to_camp_on_ptr, timer_value);
      break;
    case RRCTMR_RRC_CON_DELAY_TIMER:
      rex_set_timer(rrc_t_rrc_con_delay_ptr, timer_value);
      break;
    case RRCTMR_RCE_L2ACK_TIMER:
      rex_set_timer(rrc_t_rce_l2ack_ptr, timer_value);
      break;
    case RRCTMR_RCR_L2ACK_TIMER:
      rex_set_timer(rrc_t_rcr_l2ack_ptr, timer_value);
      break;
    case RRCTMR_RSSI_BER_TIMER:
      rex_set_timer(rrc_t_rssi_ber_ptr, timer_value);
      break;
    case RRCTMR_SIB_WAIT_TIMER:
      rex_set_timer(rrc_t_sib_wait_ptr, timer_value);
      break;
#ifdef FEATURE_DUAL_SIM
    case RRCTMR_SIB_STOP_PARTITION_TIMER:
      rex_set_timer(rrc_t_sib_stop_part_ptr, timer_value);
      break;

    case RRCTMR_SIB_MASK_INVERT_TIMER:
      rex_set_timer(rrc_t_sib_mask_ptr, timer_value);
      break;

    case RRCTMR_SIB_EMPTY_MASK_TIMER:
      rex_set_timer(rrc_t_sib_empty_mask_ptr, timer_value);
      break;
#endif
    /*Case to hanndle SIB 7 expiration*/
    case RRCTMR_SIB7_EXPIRATION_TIMER:
      rex_set_timer(rrc_t_sib7_expiration_ptr, timer_value);
      break;

    case RRCTMR_T_302_TIMER:
      rex_set_timer(rrc_t_302_ptr, timer_value); 
      break;
    case RRCTMR_T_305_TIMER:
      rex_set_timer(rrc_t_305_ptr, timer_value); 
      break;
    case RRCTMR_T_316_TIMER:
      rex_set_timer(rrc_t_316_ptr, timer_value); 
      break;
    case RRCTMR_T_317_TIMER:
      rex_set_timer(rrc_t_317_ptr, timer_value); 
      break;
    case RRCTMR_T_307_TIMER:
      rex_set_timer(rrc_t_307_ptr, timer_value); 
      break;
    case RRCTMR_T_314_TIMER:
      rex_set_timer(rrc_t_314_ptr, timer_value); 
      break;
    case RRCTMR_T_315_TIMER:
      rex_set_timer(rrc_t_315_ptr, timer_value); 
      break;
    case RRCTMR_SIB7_EXP_TIMER:
      rex_set_timer(rrc_t_sib7_exp_ptr, timer_value); 
      break;
    case RRCTMR_IGNORE_BCCH_MOD_TIMER:
      rex_set_timer(rrc_t_ignore_bcch_mod_exp_ptr, timer_value); 
      break;
    case RRCTMR_DCH_FACH_CELL_SELECTION_TIMER:
      rex_set_timer(rrc_t_dch_fach_cell_selection_exp_ptr, timer_value); 
      break;
    case RRCTMR_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER:
      rex_set_timer(rrc_t_fach_to_fach_pch_cell_selection_exp_ptr, timer_value); 
      break;
    case RRCTMR_DCH_PCH_CELL_SELECTION_TIMER:
      rex_set_timer(rrc_t_dch_pch_cell_selection_exp_ptr, timer_value); 
      break;
    case RRCTMR_L1_DEADLOCK_DETECT_TIMER:
      rex_set_timer(rrc_t_l1_deadlock_detection_exp_ptr, timer_value); 
      break;
    case RRCTMR_DELAY_ERR_FATAL_TIMER:
      rex_set_timer(rrc_t_delay_err_fatal_exp_ptr, timer_value); 
      break;
    case RRCTMR_T_3174_TIMER:
      rex_set_timer(rrc_t_3174_ptr, timer_value); 
      break;
    case RRCTMR_FREQ_SCAN_TIMER:
      rex_set_timer(rrc_t_freq_scan_exp_ptr, timer_value); 
      break;
    case RRCTMR_T_OOS_TIMER:
      rex_set_timer(rrc_t_oos_ptr, timer_value); 
      break;

    case RRCTMR_DEEP_SLEEP_NO_SVC_TIMER:
      rex_set_timer(rrc_t_deep_sleep_no_svc_ptr, timer_value); 
      break;                                   

    case RRCTMR_PHY_CHAN_FAILURE_TIMER:
      rex_set_timer(rrc_t_phy_chan_failure_ptr, timer_value);
      break;

    case RRCTMR_BPLMN_SRCH_TIMER:
      rex_set_timer(rrc_t_bplmn_srch_ptr, timer_value); 
      break;

    case RRCTMR_BPLMN_FREQ_SCAN_TIMER:
      rex_set_timer(rrc_t_bplmn_full_scan_srch_ptr, timer_value); 
      break;       

    case RRCTMR_MANUAL_PLMN_FREQ_SCAN_TIMER:
      rex_set_timer(rrc_t_manual_plmn_full_scan_srch_ptr, timer_value); 
      break;


    case RRCTMR_FREQ_REDIRECT_WAIT_TIMER:
      rex_set_timer(rrc_t_freq_redirect_wait_timer_ptr, timer_value); 
      break; 

    case RRCTMR_CONN_MODE_OOS_TIMER:
      rex_set_timer(rrc_t_conn_mode_oos_timer_ptr, timer_value); 
      break;       

#ifdef FEATURE_INACTIVITY_HANDLING

    case RRCTMR_INACTIVITY_TIMER:
      rex_set_timer(rrc_t_inactivity_timer_ptr, timer_value); 
      break;  

#endif /* FEATURE_INTACTIVITY_HANDLING */

    case RRCTMR_WTOW_BPLMN_DRX_SRCH_TIMER:
      rex_set_timer(rrc_t_wtow_bplmn_srch_ptr, timer_value); 
      break;
  
    case RRCTMR_BPLMN_GUARD_SRCH_TIMER:
      rex_set_timer(rrc_t_wtow_bplmn_guard_srch_ptr, timer_value); 
      break;

    case RRCTMR_SIXHR_SIB_UPDATE_TIMER:
      rex_set_timer(rrc_sixhr_sib_update_timer_ptr, timer_value); 
      break;
      
    case RRCTMR_TENMIN_SIB_UPDATE_TIMER:
      rex_set_timer(rrc_tenmin_sib_update_timer_ptr, timer_value);
      break;
      
    case RRCTMR_ACT_RSP_EF_TIMER:
      rex_set_timer(rrc_cho_act_rsp_ef_timer_ptr, timer_value);
      break;
      
    case RRCTMR_WT_STATE_CHANGE_EF_TIMER:
      rex_set_timer(rrc_wt_state_change_ef_timer_ptr, timer_value);
      break;

#ifdef FEATURE_EOOS
    #error code not present
#endif


    case RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER:
      rex_set_timer(rrc_t_conn_mode_oos_initial_no_ds_timer_ptr, timer_value); 
      break;       

    case RRCTMR_CONN_MODE_OOS_DS_TIMER:
      rex_set_timer(rrc_t_conn_mode_oos_ds_timer_ptr, timer_value); 
      break;       

    case RRC_SIB_CHANGE_GUARD_TIMER:
      rex_set_timer(rrc_sib_change_guard_timer_ptr, timer_value); 
      break; 

    case RRC_BCCH_MODIFICATION_TIMER:
      rex_set_timer(rrc_bcch_mod_timer_ptr, timer_value); 
      break;

    case RRCTMR_GTOW_REDIRECTION_WAIT_TIMER:
      rex_set_timer(rrc_t_gtow_redirection_timer_ptr, timer_value); 
      break;

    case RRCTMR_GTOW_REDIRECTION_LFS_TIMER:
      rex_set_timer(rrc_t_gtow_redir_lfs_timer_ptr, timer_value); 
      break;

    case RRCTMR_WTOG_ABORT_GUARD_TIMER:
      rex_set_timer(rrc_t_wtog_abort_guard_timer_ptr, timer_value); 
      break;


    case RRCTMR_T_319_TIMER:
      rex_set_timer(rrc_t_319_ptr, timer_value); 
      break;

    case RRCTMR_T_320_TIMER:
      rex_set_timer(rrc_t_320_ptr, timer_value);
      break;	  

#ifdef FEATURE_WCDMA_REL7_SPECRS

    case RRCTMR_COUEC_TIMER:
      rex_set_timer(rrc_t_couec_timer_ptr, timer_value);
      break;	  
#endif /*FEATURE_WCDMA_REL7_SPECRS*/


    case RRCTMR_T_323_TIMER:
      rex_set_timer(rrc_t_323_ptr, timer_value);
      break;	

#ifdef FEATURE_DUAL_SIM
    case RRCTMR_WRM_LOCK_TIMER:
      rex_set_timer(rrc_wrm_lock_timer_ptr, timer_value);
      break;

    case RRCTMR_WRM_DELAY_PREEMPTION_TIMER:
      rex_set_timer(rrc_wrm_delay_preemption_timer_ptr, timer_value);
      break;

    case RRCTMR_DSIM_BPLMN_LOCK_REL_OFFSET_TIMER:
      rrc_set_non_defer_timer(rrc_dsim_bplmn_lock_rel_off_timer_ptr,timer_value);
      break;

    case RRCTMR_WRM_UNLOCK_BY_TIMER:
      rex_set_timer(rrc_dsim_wrm_unlock_by_timer_ptr, timer_value);
      break;

    case RRCTMR_GET_TA_DURATION_TIMER:
      rex_set_timer(rrc_get_ta_duration_timer_ptr, timer_value);
      break;

#endif

#ifdef FEATURE_EOOS
    #error code not present
#endif

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
    case RRCTMR_DED_PRI_VALIDITY_TIMER:
      rex_set_timer(rrc_t_ded_pri_validity_timer_ptr,timer_value);
      break;
#endif
#ifdef FEATURE_QCHAT
    case RRCTMR_QCHAT_FA_RRC_TIMER:
      rex_set_timer(qchat_fa_rrc_timer_ptr, timer_value); 
      break;
#endif
#ifdef FEATURE_LTE_TO_WCDMA
    case RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER:
      rex_set_timer(rrc_t_lte_to_wcdma_redir_search_timer_ptr,timer_value);
      break;
#endif

#ifdef FEATURE_FEMTO_CSG
    case RRCTMR_CSG_ASF_SRCH_TIMER:
      rex_set_timer(rrc_t_csg_asf_srch_timer_ptr, timer_value); 
      asf_timer_status.timer_value = timer_value;
      asf_timer_status.timer_state = 0;
      event_report_payload(EVENT_WCDMA_RRC_ASF_TIMER_STATUS, sizeof(rrclog_wcdma_asf_timer_status_event_type),
                     (void*)&asf_timer_status);
      break;
#endif
    case RRCTMR_RRCCU_TRANS_TO_DISCON_TIMER:
      rex_set_timer(rrc_t_rrccu_trans_to_disconn_debug_timer_ptr,timer_value);
      break;
    case RRCTMR_BPLMN_PARTIAL_REPORT_TIMER:
      rex_set_timer(rrc_t_bplmn_partial_report_timer_ptr, timer_value); 
      break;
    case RRCTMR_WAIT_FOR_L2_ACK_FOR_PCH_TRANSITION_TIMER:
      rex_set_timer(rrc_t_rrc_waiting_for_L2_ack_for_pch_ptr,timer_value);
      break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
    case RRCTMR_OTA_BUFFER_ACTIVATION_TIMER:
      rex_set_timer(rrc_t_rrc_dch_activation_time_buffer_ota_ptr,timer_value);
      break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:
      WRRC_MSG0_HIGH("Unsupported timer is attempted to start");
      break; 
  }
  WRRC_MSG2_HIGH("Started timer:rrctmr_timer_e_type_value%d val:%d ms",timer,timer_value);
}/* rrctmr_start_timer */
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
/*===========================================================================

FUNCTION rrctmr_stop_timer

DESCRIPTION
  This function will clears the timer value and stops the timer
  timers.
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
void rrctmr_stop_timer
(
  rrctmr_timer_e_type timer                 /* Timer to stop */
)       
{
#ifdef FEATURE_FEMTO_CSG
  rrclog_wcdma_asf_timer_status_event_type asf_timer_status;
#endif
  switch(timer)
  {
    case RRCTMR_T_300_TIMER:
      rex_clr_timer(rrc_t_300_ptr);
      break;
    case RRCTMR_T_308_TIMER:
      rex_clr_timer(rrc_t_308_ptr);
      break;
    case RRCTMR_T_304_TIMER:
      rex_clr_timer(rrc_t_304_ptr); 
      break;
    case RRCTMR_WAIT_TIME_TIMER:
      rex_clr_timer(rrc_t_wait_time_ptr);
      break;
    case RRCTMR_UE_TO_CAMP_ON_TIMER:
      rex_clr_timer(rrc_t_ue_to_camp_on_ptr);
      break;
    case RRCTMR_RRC_CON_DELAY_TIMER:
      rex_clr_timer(rrc_t_rrc_con_delay_ptr);
      break;
    case RRCTMR_RCE_L2ACK_TIMER:
      rex_clr_timer(rrc_t_rce_l2ack_ptr);
      break;
    case RRCTMR_RCR_L2ACK_TIMER:
      rex_clr_timer(rrc_t_rcr_l2ack_ptr);
      break;
    case RRCTMR_RSSI_BER_TIMER:
      rex_clr_timer(rrc_t_rssi_ber_ptr);
      break;
    case RRCTMR_SIB_WAIT_TIMER:
      rex_clr_timer(rrc_t_sib_wait_ptr);
      break;

#ifdef FEATURE_DUAL_SIM
    case RRCTMR_SIB_STOP_PARTITION_TIMER:
      rex_clr_timer(rrc_t_sib_stop_part_ptr);
      break;

    case RRCTMR_SIB_MASK_INVERT_TIMER:
      rex_clr_timer(rrc_t_sib_mask_ptr);
      break;

    case RRCTMR_SIB_EMPTY_MASK_TIMER:
      rex_clr_timer(rrc_t_sib_empty_mask_ptr);
      break;
#endif
    /*Case to handle SIB 7 Expiration*/
    case RRCTMR_SIB7_EXPIRATION_TIMER:
      rex_clr_timer(rrc_t_sib_wait_ptr);
      break;

    case RRCTMR_T_302_TIMER:
      rex_clr_timer(rrc_t_302_ptr);
      break;
    case RRCTMR_T_305_TIMER:
      rex_clr_timer(rrc_t_305_ptr);
      break;
    case RRCTMR_T_316_TIMER:
      rex_clr_timer(rrc_t_316_ptr);
      break;
    case RRCTMR_T_317_TIMER:
      rex_clr_timer(rrc_t_317_ptr);
      break;
    case RRCTMR_T_307_TIMER:
      rex_clr_timer(rrc_t_307_ptr);
      break;
    case RRCTMR_T_314_TIMER:
      rex_clr_timer(rrc_t_314_ptr);
      break;
    case RRCTMR_T_315_TIMER:
      rex_clr_timer(rrc_t_315_ptr);
      break;
    case RRCTMR_SIB7_EXP_TIMER:
      rex_clr_timer(rrc_t_sib7_exp_ptr); 
      break;
    case RRCTMR_IGNORE_BCCH_MOD_TIMER:
      rex_clr_timer(rrc_t_ignore_bcch_mod_exp_ptr); 
      break;
    case RRCTMR_DCH_FACH_CELL_SELECTION_TIMER:
      rex_clr_timer(rrc_t_dch_fach_cell_selection_exp_ptr); 
      break;
    case RRCTMR_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER:
      rex_clr_timer(rrc_t_fach_to_fach_pch_cell_selection_exp_ptr); 
      break;
    case RRCTMR_DCH_PCH_CELL_SELECTION_TIMER:
      rex_clr_timer(rrc_t_dch_pch_cell_selection_exp_ptr); 
      break;
    case RRCTMR_L1_DEADLOCK_DETECT_TIMER:
      rex_clr_timer(rrc_t_l1_deadlock_detection_exp_ptr); 
      break;
    case RRCTMR_DELAY_ERR_FATAL_TIMER:
      rex_clr_timer(rrc_t_delay_err_fatal_exp_ptr); 
      break;
    case RRCTMR_T_3174_TIMER:
      rex_clr_timer(rrc_t_3174_ptr);
      break;
    case RRCTMR_FREQ_SCAN_TIMER:
      rex_clr_timer(rrc_t_freq_scan_exp_ptr); 
      break;
    case RRCTMR_T_OOS_TIMER:
      rex_clr_timer(rrc_t_oos_ptr); 
      break;

    case RRCTMR_DEEP_SLEEP_NO_SVC_TIMER:
      rex_clr_timer(rrc_t_deep_sleep_no_svc_ptr); 
      break;

     case RRCTMR_PHY_CHAN_FAILURE_TIMER:
      rex_clr_timer(rrc_t_phy_chan_failure_ptr);
      break;  

    case RRCTMR_BPLMN_SRCH_TIMER:
      rex_clr_timer(rrc_t_bplmn_srch_ptr); 
      break;

    case RRCTMR_BPLMN_FREQ_SCAN_TIMER:
      rex_clr_timer(rrc_t_bplmn_full_scan_srch_ptr); 
      break;     

    case RRCTMR_MANUAL_PLMN_FREQ_SCAN_TIMER:
      rex_clr_timer(rrc_t_manual_plmn_full_scan_srch_ptr); 
      break;


    case RRCTMR_FREQ_REDIRECT_WAIT_TIMER:
      rex_clr_timer(rrc_t_freq_redirect_wait_timer_ptr); 
      break;   

    case RRCTMR_CONN_MODE_OOS_TIMER:
      rex_clr_timer(rrc_t_conn_mode_oos_timer_ptr);
      break;       

#ifdef FEATURE_INACTIVITY_HANDLING
    case RRCTMR_INACTIVITY_TIMER:
      rex_clr_timer(rrc_t_inactivity_timer_ptr);
      break;  
#endif /* FEATURE_INTACTIVITY_HANDLING */

    case RRCTMR_WTOW_BPLMN_DRX_SRCH_TIMER:
      rex_clr_timer(rrc_t_wtow_bplmn_srch_ptr); 
      break;
  
    case RRCTMR_BPLMN_GUARD_SRCH_TIMER:
      rex_clr_timer(rrc_t_wtow_bplmn_guard_srch_ptr); 
      break;

    case RRCTMR_SIXHR_SIB_UPDATE_TIMER:
      rex_clr_timer(rrc_sixhr_sib_update_timer_ptr);
     break;

    case RRCTMR_TENMIN_SIB_UPDATE_TIMER:
      rex_clr_timer(rrc_tenmin_sib_update_timer_ptr);
      break;


    case RRCTMR_ACT_RSP_EF_TIMER:
      rex_clr_timer(rrc_cho_act_rsp_ef_timer_ptr);
      break;

    case RRCTMR_WT_STATE_CHANGE_EF_TIMER:
      rex_clr_timer(rrc_wt_state_change_ef_timer_ptr);
      break;

#ifdef FEATURE_EOOS
    #error code not present
#endif

    case RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER:
      rex_clr_timer(rrc_t_conn_mode_oos_initial_no_ds_timer_ptr);
      break;       

    case RRCTMR_CONN_MODE_OOS_DS_TIMER:
      rex_clr_timer(rrc_t_conn_mode_oos_ds_timer_ptr);
      break;       

    case RRC_SIB_CHANGE_GUARD_TIMER:
      rex_clr_timer(rrc_sib_change_guard_timer_ptr); 
      break; 

    case RRC_BCCH_MODIFICATION_TIMER:
      rex_clr_timer(rrc_bcch_mod_timer_ptr); 
      break;

    case RRCTMR_GTOW_REDIRECTION_WAIT_TIMER:
      rex_clr_timer(rrc_t_gtow_redirection_timer_ptr);
      break;

    case RRCTMR_GTOW_REDIRECTION_LFS_TIMER:
      rex_clr_timer(rrc_t_gtow_redir_lfs_timer_ptr);
      break;

    case RRCTMR_WTOG_ABORT_GUARD_TIMER:
      rex_clr_timer(rrc_t_wtog_abort_guard_timer_ptr); 
      break;


    case RRCTMR_T_319_TIMER:
      rex_clr_timer(rrc_t_319_ptr);
      break;

    case RRCTMR_T_320_TIMER:
      rex_clr_timer(rrc_t_320_ptr);
      break;	 

#ifdef FEATURE_WCDMA_REL7_SPECRS
    case RRCTMR_COUEC_TIMER:
      rex_clr_timer(rrc_t_couec_timer_ptr);
      break;
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

    case RRCTMR_T_323_TIMER:
      rex_clr_timer(rrc_t_323_ptr);   
      break;  

#ifdef FEATURE_DUAL_SIM
    case RRCTMR_WRM_LOCK_TIMER:
      rex_clr_timer(rrc_wrm_lock_timer_ptr);
      break;

    case RRCTMR_WRM_DELAY_PREEMPTION_TIMER:
      rex_clr_timer(rrc_wrm_delay_preemption_timer_ptr);
      break;

    case RRCTMR_DSIM_BPLMN_LOCK_REL_OFFSET_TIMER:
      rrc_clr_non_defer_timer(rrc_dsim_bplmn_lock_rel_off_timer_ptr);
      break;

    case RRCTMR_WRM_UNLOCK_BY_TIMER:
      rex_clr_timer(rrc_dsim_wrm_unlock_by_timer_ptr);
      break;

    case RRCTMR_GET_TA_DURATION_TIMER:
      rex_clr_timer(rrc_get_ta_duration_timer_ptr);
      break;
#endif

#ifdef FEATURE_EOOS
    #error code not present
#endif

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
    case RRCTMR_DED_PRI_VALIDITY_TIMER:
      rex_clr_timer(rrc_t_ded_pri_validity_timer_ptr);
      break;
#endif
#ifdef FEATURE_QCHAT
    case RRCTMR_QCHAT_FA_RRC_TIMER:
      rex_clr_timer(qchat_fa_rrc_timer_ptr); 
      break; 
#endif
#ifdef FEATURE_LTE_TO_WCDMA
    case RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER:
      rex_clr_timer(rrc_t_lte_to_wcdma_redir_search_timer_ptr);
      break;
#endif

#ifdef FEATURE_FEMTO_CSG
    case RRCTMR_CSG_ASF_SRCH_TIMER:
      rex_clr_timer(rrc_t_csg_asf_srch_timer_ptr);
      asf_timer_status.timer_value = 0;
      asf_timer_status.timer_state = 1;
      event_report_payload(EVENT_WCDMA_RRC_ASF_TIMER_STATUS, sizeof(rrclog_wcdma_asf_timer_status_event_type),
                     (void*)&asf_timer_status);
      break;
#endif
    case RRCTMR_RRCCU_TRANS_TO_DISCON_TIMER:
      rex_clr_timer(rrc_t_rrccu_trans_to_disconn_debug_timer_ptr);
      break;

    case RRCTMR_BPLMN_PARTIAL_REPORT_TIMER:
      rex_clr_timer(rrc_t_bplmn_partial_report_timer_ptr); 
      break;      
     
    case RRCTMR_WAIT_FOR_L2_ACK_FOR_PCH_TRANSITION_TIMER:
      rex_clr_timer(rrc_t_rrc_waiting_for_L2_ack_for_pch_ptr);
      break;     

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
    case RRCTMR_OTA_BUFFER_ACTIVATION_TIMER:
      rex_clr_timer(rrc_t_rrc_dch_activation_time_buffer_ota_ptr);
      break;    
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:
      WRRC_MSG0_ERROR("ERR Unsupported timer is attempted to stop");
      break; 
  }
  WRRC_MSG1_HIGH("Stopped timer:rrctmr_timer_e_type_value%d",timer);
} /* rrctmr_stop_timer */
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
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
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
uint64 rrctmr_get_remaining_time
(
  rrctmr_timer_e_type timer                 /* Timer to stop */
)       
{
  uint64 remaining_time = 0; 

  switch(timer)
  {
    case RRCTMR_T_300_TIMER:
      remaining_time = rex_get_timer(rrc_t_300_ptr);
      break;
    case RRCTMR_T_308_TIMER:
      remaining_time = rex_get_timer(rrc_t_308_ptr);
      break;
    case RRCTMR_T_304_TIMER:
      remaining_time = rex_get_timer(rrc_t_304_ptr); 
      break;
    case RRCTMR_WAIT_TIME_TIMER:
      remaining_time = rex_get_timer(rrc_t_wait_time_ptr);
      break;
    case RRCTMR_UE_TO_CAMP_ON_TIMER:
      remaining_time = rex_get_timer(rrc_t_ue_to_camp_on_ptr);
      break;
    case RRCTMR_RRC_CON_DELAY_TIMER:
      remaining_time = rex_get_timer(rrc_t_rrc_con_delay_ptr);
      break;
    case RRCTMR_RCE_L2ACK_TIMER:
      remaining_time = rex_get_timer(rrc_t_rce_l2ack_ptr);
      break;
    case RRCTMR_RCR_L2ACK_TIMER:
      remaining_time = rex_get_timer(rrc_t_rcr_l2ack_ptr);
      break;
    case RRCTMR_RSSI_BER_TIMER:
      remaining_time = rex_get_timer(rrc_t_rssi_ber_ptr);
      break;
    case RRCTMR_SIB_WAIT_TIMER:
      remaining_time = rex_get_timer(rrc_t_sib_wait_ptr);
      break;

#ifdef FEATURE_DUAL_SIM
    case RRCTMR_SIB_STOP_PARTITION_TIMER:
      remaining_time = rex_get_timer(rrc_t_sib_stop_part_ptr);
      break;

    case RRCTMR_SIB_MASK_INVERT_TIMER:
      remaining_time = rex_get_timer(rrc_t_sib_mask_ptr);
      break;

    case RRCTMR_SIB_EMPTY_MASK_TIMER:
      remaining_time = rex_get_timer(rrc_t_sib_empty_mask_ptr);
      break;
#endif
    /*Case to handle SIB7 expiry timer*/
    case RRCTMR_SIB7_EXPIRATION_TIMER:
       remaining_time = rex_get_timer(rrc_t_sib7_expiration_ptr);
        break;

    case RRCTMR_T_302_TIMER:
      remaining_time = rex_get_timer(rrc_t_302_ptr);
      break;
    case RRCTMR_T_305_TIMER:
      remaining_time = rex_get_timer(rrc_t_305_ptr);
      break;
    case RRCTMR_T_316_TIMER:
      remaining_time = rex_get_timer(rrc_t_316_ptr);
      break;
    case RRCTMR_T_317_TIMER:
      remaining_time = rex_get_timer(rrc_t_317_ptr);
      break;
    case RRCTMR_T_307_TIMER:
      remaining_time = rex_get_timer(rrc_t_307_ptr);
      break;
    case RRCTMR_T_314_TIMER:
      remaining_time = rex_get_timer(rrc_t_314_ptr);
      break;
    case RRCTMR_T_315_TIMER:
      remaining_time = rex_get_timer(rrc_t_315_ptr);
      break;
    case RRCTMR_SIB7_EXP_TIMER:
      remaining_time = rex_get_timer(rrc_t_sib7_exp_ptr); 
      break;
    case RRCTMR_IGNORE_BCCH_MOD_TIMER:
      remaining_time = rex_get_timer(rrc_t_ignore_bcch_mod_exp_ptr); 
      break;
    case RRCTMR_DCH_FACH_CELL_SELECTION_TIMER:
      remaining_time = rex_get_timer(rrc_t_dch_fach_cell_selection_exp_ptr); 
      break;
    case RRCTMR_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER:
      remaining_time = rex_get_timer(rrc_t_fach_to_fach_pch_cell_selection_exp_ptr); 
      break;
    case RRCTMR_DCH_PCH_CELL_SELECTION_TIMER:
      remaining_time = rex_get_timer(rrc_t_dch_pch_cell_selection_exp_ptr); 
      break;
    case RRCTMR_L1_DEADLOCK_DETECT_TIMER:
      remaining_time = rex_get_timer(rrc_t_l1_deadlock_detection_exp_ptr); 
      break;
    case RRCTMR_DELAY_ERR_FATAL_TIMER:
      remaining_time = rex_get_timer(rrc_t_delay_err_fatal_exp_ptr); 
      break;
    case RRCTMR_T_3174_TIMER:
      remaining_time = rex_get_timer(rrc_t_3174_ptr);
      break;
    case RRCTMR_FREQ_SCAN_TIMER:
      remaining_time = rex_get_timer(rrc_t_freq_scan_exp_ptr); 
      break;
    case RRCTMR_T_OOS_TIMER:
      remaining_time = rex_get_timer(rrc_t_oos_ptr); 
      break;

    case RRCTMR_DEEP_SLEEP_NO_SVC_TIMER:
      remaining_time = rex_get_timer(rrc_t_deep_sleep_no_svc_ptr); 
      break;

     case RRCTMR_PHY_CHAN_FAILURE_TIMER:
      remaining_time = rex_get_timer(rrc_t_phy_chan_failure_ptr);
      break;  

    case RRCTMR_BPLMN_SRCH_TIMER:
      remaining_time = rex_get_timer(rrc_t_bplmn_srch_ptr); 
      break;

    case RRCTMR_BPLMN_FREQ_SCAN_TIMER:
      remaining_time = rex_get_timer(rrc_t_bplmn_full_scan_srch_ptr); 
      break;     

    case RRCTMR_MANUAL_PLMN_FREQ_SCAN_TIMER:
      remaining_time = rex_get_timer(rrc_t_manual_plmn_full_scan_srch_ptr); 
      break; 


    case RRCTMR_FREQ_REDIRECT_WAIT_TIMER:
      remaining_time = rex_get_timer(rrc_t_freq_redirect_wait_timer_ptr); 
      break;   

    case RRCTMR_CONN_MODE_OOS_TIMER:
      remaining_time = rex_get_timer(rrc_t_conn_mode_oos_timer_ptr); 
      break;   

#ifdef FEATURE_INACTIVITY_HANDLING
    case RRCTMR_INACTIVITY_TIMER:
      remaining_time = rex_get_timer(rrc_t_inactivity_timer_ptr);
      break;  
#endif /* FEATURE_INTACTIVITY_HANDLING */

    case RRCTMR_WTOW_BPLMN_DRX_SRCH_TIMER:
      remaining_time = rex_get_timer(rrc_t_wtow_bplmn_srch_ptr); 
      break;                                                 
  
    case RRCTMR_BPLMN_GUARD_SRCH_TIMER:
      remaining_time = rex_get_timer(rrc_t_wtow_bplmn_guard_srch_ptr); 
      break;         

    case RRCTMR_SIXHR_SIB_UPDATE_TIMER:
      remaining_time = rex_get_timer(rrc_sixhr_sib_update_timer_ptr);
     break;

    case RRCTMR_TENMIN_SIB_UPDATE_TIMER:
      remaining_time = rex_get_timer(rrc_tenmin_sib_update_timer_ptr);
      break;

    case RRCTMR_ACT_RSP_EF_TIMER:
      remaining_time = rex_get_timer(rrc_cho_act_rsp_ef_timer_ptr);
      break;

    case RRCTMR_WT_STATE_CHANGE_EF_TIMER:
      remaining_time = rex_get_timer(rrc_wt_state_change_ef_timer_ptr);
      break;

#ifdef FEATURE_EOOS
    #error code not present
#endif

    case RRCTMR_CONN_MODE_OOS_INITIAL_NO_DS_TIMER:
      remaining_time = rex_get_timer(rrc_t_conn_mode_oos_initial_no_ds_timer_ptr);
     break;

    case RRCTMR_CONN_MODE_OOS_DS_TIMER:
      remaining_time = rex_get_timer(rrc_t_conn_mode_oos_ds_timer_ptr);
     break;

    case RRC_SIB_CHANGE_GUARD_TIMER:
      remaining_time = rex_get_timer(rrc_sib_change_guard_timer_ptr); 
      break; 

    case RRC_BCCH_MODIFICATION_TIMER:
      remaining_time = rex_get_timer(rrc_bcch_mod_timer_ptr); 
      break;

    case RRCTMR_GTOW_REDIRECTION_WAIT_TIMER:
      remaining_time = rex_get_timer(rrc_t_gtow_redirection_timer_ptr);
     break;

    case RRCTMR_GTOW_REDIRECTION_LFS_TIMER:
      remaining_time = rex_get_timer(rrc_t_gtow_redir_lfs_timer_ptr);
     break;

    case RRCTMR_WTOG_ABORT_GUARD_TIMER:
      remaining_time = rex_get_timer(rrc_t_wtog_abort_guard_timer_ptr);
      break;

 
    case RRCTMR_T_319_TIMER:
      remaining_time = rex_get_timer(rrc_t_319_ptr);
      break;

    case RRCTMR_T_320_TIMER:
      remaining_time = rex_get_timer(rrc_t_320_ptr);
      break;

#ifdef FEATURE_WCDMA_REL7_SPECRS

    case RRCTMR_COUEC_TIMER:
      remaining_time = rex_get_timer(rrc_t_couec_timer_ptr);
      break;
#endif /* FEATURE_WCDMA_REL7_SPECRS */


    case RRCTMR_T_323_TIMER:
      remaining_time = rex_get_timer(rrc_t_323_ptr);
      break;

#ifdef FEATURE_DUAL_SIM
    case RRCTMR_WRM_LOCK_TIMER:
      remaining_time = rex_get_timer(rrc_wrm_lock_timer_ptr);
      break;

    case RRCTMR_WRM_DELAY_PREEMPTION_TIMER:
      remaining_time = rex_get_timer(rrc_wrm_delay_preemption_timer_ptr);
      break;

    case RRCTMR_DSIM_BPLMN_LOCK_REL_OFFSET_TIMER:
      remaining_time = rrc_get_non_defer_timer(rrc_dsim_bplmn_lock_rel_off_timer_ptr);
      break;

    case RRCTMR_WRM_UNLOCK_BY_TIMER:
      remaining_time = rex_get_timer(rrc_dsim_wrm_unlock_by_timer_ptr);
      break;

    case RRCTMR_GET_TA_DURATION_TIMER:
      remaining_time = rex_get_timer(rrc_get_ta_duration_timer_ptr);
      break;
#endif
#ifdef FEATURE_EOOS
    #error code not present
#endif
#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
    case RRCTMR_DED_PRI_VALIDITY_TIMER:
      remaining_time = rex_get_timer(rrc_t_ded_pri_validity_timer_ptr);
      break;
#endif

#ifdef FEATURE_QCHAT
    case RRCTMR_QCHAT_FA_RRC_TIMER:
      remaining_time = rex_get_timer(qchat_fa_rrc_timer_ptr); 
      break;
#endif
    case RRCTMR_RRCCU_TRANS_TO_DISCON_TIMER:
      remaining_time = rex_get_timer(rrc_t_rrccu_trans_to_disconn_debug_timer_ptr);
      break;


#ifdef FEATURE_LTE_TO_WCDMA
    case RRCTMR_LTE_TO_WCDMA_REDIR_SRCH_TIMER:
      remaining_time = rex_get_timer(rrc_t_lte_to_wcdma_redir_search_timer_ptr); 
      break;
#endif

#ifdef FEATURE_FEMTO_CSG
    case RRCTMR_CSG_ASF_SRCH_TIMER:
      remaining_time = rex_get_timer(rrc_t_csg_asf_srch_timer_ptr); 
      break;
#endif
    case RRCTMR_WAIT_FOR_L2_ACK_FOR_PCH_TRANSITION_TIMER:
      remaining_time = rex_get_timer(rrc_t_rrc_waiting_for_L2_ack_for_pch_ptr);
      break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
    case RRCTMR_OTA_BUFFER_ACTIVATION_TIMER:
      remaining_time = rex_get_timer(rrc_t_rrc_dch_activation_time_buffer_ota_ptr);
      break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:
      WRRC_MSG1_ERROR("ERR Getting remaining time for timer %d is not supported",timer);
      break; 
  }

  WRRC_MSG2_HIGH("Remaining time %d for timer rrctmr_timer_e_type_value%d",remaining_time, timer);

  return remaining_time;
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

/*The following lint directive limits the scope of this directive to 
 this file only */
/*lint +e534*/
