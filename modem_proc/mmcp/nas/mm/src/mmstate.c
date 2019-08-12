/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmstate.c_v   1.23   06 Jun 2002 14:25:04   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmstate.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/29/11   gps     pended stop_mode_req until abort procedure completes
06/03/01   sbk     Global variables are renamed to use mm_ prefix
06/08/01   jca     Fixed multiple bugs in state table and logic flow.
06/26/01   jca     Fixed idle transition bug.
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
08/15/01   ab      Rename CM to CNM.
08/22/01   jca     Removed errant CM to CNM rename.
09/18/01   sbk     Intertask primitives are logged and QXDM logging is supported
11/06/01   jca     Fixed bugs involving use of T3230 and T3240 timers.
11/08/01   jca     Transition to state 9 from state 5 on MMCNM_REL_REQ.
11/08/01   jca     Added MM_STD_AUTHENTICATION_REJECT event code and processing.
12/06/01   jca     Now sending UE_DYNAMIC_ID log packet when TMSI value reset.
12/13/01   jca     Transition to state 19 from state 20 when RRC_REL_IND received.
02/01/02   jca     Deleted invalid MM_STD_ABORT_RECEIVED entries from state table.
02/12/02   jca     Fix a number of bugs in MM state table related to CNM-MM processing.
02/19/02   jca     Now send RRC_ABORT_REQ when MMCNM_REL_REQ received in state 14.
04/24/02   jca     Modifications to reflect changes to MM-CNM interface.
05/28/02   jca     Now handle event MM_STD_RR_CONNECTION_ESTABLISHED in state 9.
06/03/02   jca     Now handle event MM_STD_AUTHENTICATION_REJECT in state 9.
06/05/02   ks      Added support for dual mode operation.
07/23/02   ks      Added Handling of dynamic RAT change support.
07/25/02   jca     Added support for GSM Call Reestablishment procedures.
08/09/02   jca     Mods to make T3212/T3213 timer processing spec compliant.
09/04/02   vdr     Fixed determination of the CKSN
10/07/02   jca     Changes to support CS/PS combined attach procedures.
10/11/02   cd      Modified call to mm_send_mmcnm_est_rej to include passing
                   the reject cause based on the mm_event
                   Replaced action_0 for action_19 in state table when transitioning
                   from state 5 to 9 on a T3230 timeout.
10/25/02   cd      Corrected typo in mm_state_control.
11/12/02   jca     Modified prototype for function mm_abort_rr().
11/15/02   mks     Resume timer T3211 during transition to IDLE state when the
                   idle substate is either MM_NORMAL_SERVICE or MM_ATTEMPTING_TO_UPDATE
12/05/02   jca     Changes to support PS_ONLY<->CS_ONLY mode switches in GSM mode.
01/10/03   jca     Changes to support CS_ONLY (or PS_ONLY) to CS/PS mode switching.
01/10/03   jca     Now processing MM_STD_RANDOM_ACCESS_FAILURE in state 15.
01/10/03   vdr     Changes of CKSN determination
01/15/02   ks      Stopping TIMER_T3210 in ACTION_27, triggered by event
                   MM_STD_RANDOM_ACCESS_FAILURE when MM is in state 13.
01/16/03   vdr     Added GSM CS connection management in order to support dual mode features
01/27/03   jca     Changes to support PS_ONLY<->CS/PS mode switching.
                   Changes to support IMSI Detach via Combined GPRS Detach procedure.
01/28/03   cd      Now sending mm info to GSTK when entering mm idle state
01/31/03   mks     Added a fix to avoid sending REG_CNF when RAT change is still
                   pending.
02/07/03   ks      Added support for SYS_STOP_MODE_REASON_MODE_CHANGE reason.
02/21/03   vdr     Removed connection management & updated mm_confirm_mm_connection
                   function call according to the new interface
04/03/03   cd      Moved call to GSTK down to ensure the most current information is sent
04/08/03   jca     Added new reason code for pending LU request.
04/15/03   cd      Added processing new mm_pending_lu_request set to IMSI_ATTACH
05/08/03   cd      Now transition from state 6 to 20 initiates ACTION_3 which starts
                   T3230
05/13/03   jca     Start T3220 on transition from state 15 to state 7 as per spec.
05/19/03   mks     Now initiating ACTION_0 when MM transitions from 0 to 18.
07/23/03   ks      Calling Action_10 when MM goes from state 10 to state 19
                   for event MM_STD_RR_CONNECTION_ABORTED.
07/29/03   jca     Fixed PS Attach-on-demand problem related to clearing of the
                   mm_serv_change_pending boolean flag.
08/07/03   mks     If a normal LU is pending when MM returns to MM state
                   IDLE.NORMAL-SERVICE, a normal LU procedure is initiated.
09/18/03   jca     Now resetting mm_pending_lu_request within mm_initiate_lu().
                   Now calling mm_initiate_lu() instead of
                   mm_state_control( MM_STD_LU_REQUIRED ).
09/18/03   mks     Merged Kurt's changes to support silent redial feature - Now
                   notifying CNM about voice call release and going to idle state
                   when lower layer failure occurs with reason "silent redial not
                   allowed" in state 14 (MM_WAIT_FOR_RR_CONNECTION_MM).
10/02/03   mks     Added processing for MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED
                   event in state 9 (MM_WAIT_FOR_NETWORK_COMMAND).
11/25/03   mks     Added processing for MM_STD_LU_REQUIRED event in non idle state.
12/11/03   mks     Added processing for MM_STD_LU_REQUIRED event in state 9.
01/30/04   cd      Sending a MMR_REG_CNF after LU is rejected with cause NATIONAL_ROAMING_NOT_ALLOWED,
                   and T3240 expires.
02/17/04   cd      Not sending MM info to GSTK when the state transition is from
                   MM_IDLE to MM_IDLE
03/08/04   cd      Now supporting processing of MMCNM_REL_REQ in state 20
03/10/04   jca     Added support for event MM_STD_LOW_LEVEL_FAIL in state 7.
03/15/04   cd      Corrected action for transtitions out of states 17 and 21
                   when the abort_ind reason is MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED
04/06/04   cd      LCS support
05/04/04   jca     Now handle event MM_STD_RR_CONNECTION_RELEASED in state 13.
05/17/04   ks      Corrected action for transtitions out of states 15 when the
                   abort_ind reason is MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED and
                   MM_STD_RR_CONNECTION_RELEASED.
06/07/04   jca     Allow transition to from state 3 to state 5 when an emergency
                   call is requested and a Location Update is in-progress.
06/08/04   jca     Allow transitions from state 13 to state 14, state 9 to state 5,
                   and state 10 to state 5 when an emergency call is requested
                   and a Location Update is in-progress.
06/08/04   cd      LCS support.
07/15/04   cd      Added processing of event MM_STD_CIPHERING_HAS_STARTED in state 20.
08/02/04   cd      Corrected processing of CM_SERVICE_REJ in state 5, 20 based on reject cause
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
09/13/04   jca     The MM state should NOT be set to 19 (IDLE) until the receipt
                   of the RR_ABORT_CNF (in response to the RR_ABORT_REQ).
10/06/04   jca     When returning to MM_IDLE, if the SIM is considered invalid,
                   then set the substate to NO_IMSI as per 24.008 4.2.3.
12/13/04   jca     Now handle event MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED in state 13.
12/17/04   vdr     Now reseting authentication cache if MM goes into MM_IDLE state in UMTS
12/24/04   abe     While UE is detaching for CS service (IMSI DETACH), should stop CS timers
                   T3211, T3212 & T3213
01/05/04   vdr     Moved auth_reset_cache_memory() right after MM new state determination
03/04/05   sm      Added transition for event MM_STD_RR_CONNECTION_ABORTED
                   when mmstate is in 3,9.
05/20/05   jca     When T3210 expires, do not transition to MM Idle state until a
                   RR(C)_ABORT_CNF is received signalling RR connection has been released.
06/06/05   abe     Added code for CCBS support
07/19/05   jca     Now handle event MM_STD_LCS_STARTED in state 19.
07/22/05   jca     Added support for event MM_STD_LOW_LEVEL_FAIL in state 6.
09/16/05   vdr     Added support for event MM_STD_MM_CONNECTION_REQUEST in state 14
10/06/05   abe     Added processing of MM_STD_CCO_FAILED in state 13
10/08/05   cm/src  Send MM idle info to GSTK only if MM is not waiting for RR
                   active.
12/5/05    hj      Stopping T3211 when MM enters state 6.
06/09/06   sn      Added SYS_STOP_MODE_REASON_LPM
12/5/05    hj      Start T3212 along with T3211 when entering idle state.
06/14/06   up      Called new mm_update_mm_serving_plmn_info for GSTK event.
06/15/06   nr      Passing proper cause in mm_notify_reestablishment_fail.
04/09/07   rd      SMS optimization changes are incorporated.
02/29/08   jbk     Service domain updated from state 15 to 19                   
07/04/11   jbk     Handle MM_STD_RADIO_UNAVAILABLE in state 9 and 17                   
20/10/11   rajesh  Added process to handle event MM_STD_LOW_LEVEL_FAIL in state 20.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mm_v.h"

#ifdef FEATURE_GSTK
#include "mm_gstk.h"
#endif

#include "err.h"
#include "gmm_v.h"
#include "mm_coord.h"
#include "naslog_v.h"
#include "mm_sim.h"
#include "mmauth.h"
#include "gmm_int_v.h"
#include "reg_state.h"
#include "mmrat.h"

#ifdef FEATURE_GSM_GPRS
#include "gmmllcm.h"
#endif
#ifdef FEATURE_DUAL_SIM
#include "subs_prio.h"
#endif

extern void mmcoord_process_mmr_stop_mode_req( mm_cmd_type *message );

/*===========================================================================

                     VARIABLE DECLARATIONS MODULE

===========================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern byte mm_last_paging_cause_sim[MAX_NAS_STACKS];

#define  mm_last_paging_cause mm_last_paging_cause_sim[mm_as_id]

extern   byte gprs_attach_attempt_counter_sim[MAX_NAS_STACKS];
#define  gprs_attach_attempt_counter gprs_attach_attempt_counter_sim[mm_as_id]

extern   byte gmm_rau_attempt_counter_sim[MAX_NAS_STACKS];
#define  gmm_rau_attempt_counter gmm_rau_attempt_counter_sim[mm_as_id]

extern   boolean gmm_foreground_search_pending_sim[MAX_NAS_STACKS];
#define  gmm_foreground_search_pending gmm_foreground_search_pending_sim[mm_as_id]

extern boolean mm_stop_mode_local_detach_peding_sim[MAX_NAS_STACKS];
#define mm_stop_mode_local_detach_peding mm_stop_mode_local_detach_peding_sim[mm_as_id]
#else

extern byte mm_last_paging_cause;

extern byte    gprs_attach_attempt_counter;
extern byte    gmm_rau_attempt_counter;
extern boolean gmm_foreground_search_pending;
extern boolean        mm_stop_mode_local_detach_peding;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE
extern void mmcoord_handle_ue_sglte_mode_switch(void);
#endif
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define ACTION_0     0
#define ACTION_1     1
#define ACTION_2     2
#define ACTION_3     3
#define ACTION_4     4
#define ACTION_5     5
#define ACTION_6     6
#define ACTION_7     7
#define ACTION_8     8
#define ACTION_9     9
#define ACTION_10    10
#define ACTION_11    11
#define ACTION_12    12
#define ACTION_13    13
#define ACTION_14    14
#define ACTION_15    15
#define ACTION_16    16
#define ACTION_17    17
#define ACTION_18    18
#define ACTION_19    19
#define ACTION_20    20
#define ACTION_21    21
#define ACTION_22    22
#define ACTION_23    23
#define ACTION_24    24
#define ACTION_25    25
#define ACTION_26    26
#define ACTION_27    27
#define ACTION_28    28
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
#define ACTION_29    29
#define ACTION_30    30
#define ACTION_31    31
#define ACTION_32    32
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
#ifdef FEATURE_CCBS
#define ACTION_33    33
#endif /*FEATURE_CCBS */
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
#define ACTION_34    34
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
#ifdef FEATURE_LTE
#define ACTION_35    35
#endif
#ifdef FEATURE_FEMTO_CSG
#define ACTION_36   36
#endif 

typedef struct state_event_tag
{
   byte        current_state;
   byte        next_state;
   mm_event_T  event;
   byte        action_code;
} state_event_T;

/*
 * Note: State numbers correspond to those used in 2.4-s1
 */

state_event_T table[] =
{
   {  0, 18, MM_STD_MS_ACTIVATED,                     ACTION_0  },
   {  0,  0, MM_STD_LOW_LEVEL_FAIL,                   ACTION_0  },

   {  3,  3, MM_STD_CIPHERING_HAS_STARTED,            ACTION_0  },
   {  3,  3, MM_STD_LU_REQUIRED,                      ACTION_0  },
   {  3,  5, MM_STD_LU_ACCEPT_FOLLOW_ON,              ACTION_8  },
   {  3,  9, MM_STD_LU_ACCEPT,                        ACTION_8  },
   {  3,  9, MM_STD_AUTHENTICATION_REJECT,            ACTION_11 },
   {  3, 10, MM_STD_LU_REJECT,                        ACTION_8  },
   {  3, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_10 },
   {  3, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_10 },
#ifdef FEATURE_SGLTE
   {  3, 19, MM_STD_RADIO_UNAVAILABLE,                ACTION_10 },
#endif
   {  3, 10, MM_STD_T3210_TIMEOUT,                    ACTION_0 },
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   {  3,  3, MM_STD_LCS_FINISHED,                     ACTION_0 },
   {  3, 25, MM_STD_RR_CONNECTION_REL_NOT_ALLOWED,    ACTION_10 },
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
   {  3,  5, MM_STD_MM_CONNECTION_REQUEST,            ACTION_0  },
   {  3,  9, MM_STD_RR_CONNECTION_ABORTED,            ACTION_0  },
   {  3, 13, MM_STD_REESTABLISHMENT_REQUIRED,         ACTION_0 },
   {  5,  5, MM_STD_NETWORK_MESSAGE_RECEIVED,         ACTION_0  },
   {  5,  5, MM_STD_LU_REQUIRED,                      ACTION_28 },
   {  5,  6, MM_STD_CIPHERING_HAS_STARTED,            ACTION_21 },
   {  5,  6, MM_STD_CNM_SERVICE_ACCEPT,               ACTION_21 },
   {  5,  9, MM_STD_T3230_TIMEOUT,                    ACTION_19  },
   {  5,  9, MM_STD_ABORT_MESSAGE_RECEIVED,           ACTION_11 },
   {  5,  9, MM_STD_SERVICE_REJECT_CAUSE_4_6,         ACTION_11 },
   {  5,  9, MM_STD_SERVICE_REJECT,                   ACTION_11 },
   {  5,  9, MM_STD_LAST_CONNECTION_RELEASED,         ACTION_20 },
   {  5, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_19 },
   {  5, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_15 },
   {  5,  9, MM_STD_AUTHENTICATION_REJECT,            ACTION_11  },
   {  5, 19, MM_STD_RANDOM_ACCESS_FAILURE,            ACTION_19 },
   {  5,  9, MM_STD_RR_CONNECTION_ABORTED,            ACTION_0 },
   {  5,  5, MM_STD_LU_ACCEPT,                        ACTION_0  },
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   {  5,  5, MM_STD_LCS_FINISHED,                     ACTION_0 },
   {  5, 25, MM_STD_LCS_IN_PROGRESS,                  ACTION_0 },
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
   {  5, 14, MM_STD_REESTABLISHMENT_REQUIRED,         ACTION_0 },
   {  6,  6, MM_STD_NETWORK_MESSAGE_RECEIVED,         ACTION_0  },
   {  6,  6, MM_STD_CIPHERING_HAS_STARTED,            ACTION_0  },
   {  6,  6, MM_STD_LU_REQUIRED,                      ACTION_28 },
   {  6,  9, MM_STD_LAST_CONNECTION_RELEASED,         ACTION_24 },
   {  6,  9, MM_STD_ABORT_MESSAGE_RECEIVED,           ACTION_0  },
   {  6,  7, MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED,    ACTION_13 },
   {  6, 19, MM_STD_LOW_LEVEL_FAIL_REEST_NOT_ALLOWED, ACTION_17 },
   {  6, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_15 },
   {  6, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_17 },
   {  6, 20, MM_STD_SERVICE_REQUEST_SENT,             ACTION_3  },
   {  6, 22, MM_STD_LOW_LEVEL_FAIL_REEST_ALLOWED,     ACTION_16 },
   {  6,  9, MM_STD_RR_CONNECTION_ABORTED,            ACTION_0  },
   {  6,  6, MM_STD_LU_ACCEPT,                        ACTION_0  },
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   {  6,  6, MM_STD_LCS_STARTED,                      ACTION_32 },
   {  6,  6, MM_STD_LCS_FINISHED,                     ACTION_0 },
   {  6, 25, MM_STD_RR_CONNECTION_REL_NOT_ALLOWED,    ACTION_30 },
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
#ifdef FEATURE_CCBS
   {  6,  8, MM_STD_CM_SERVICE_PROMPT_RECEIVED,       ACTION_0 },
#endif /*FEATURE_CCBS*/
#ifdef FEATURE_FEMTO_CSG
   {  6,  9, MM_STD_SERVICE_REJECT_CAUSE_25,      ACTION_36 },
#endif 
   {  7, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_12 },
   {  7,  7, MM_STD_RR_CONNECTION_ABORTED,            ACTION_12 },
   {  7, 15, MM_STD_REESTABLISHMENT_REQUIRED,         ACTION_0 },
   {  7, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_12 },
   {  7, 19, MM_STD_RANDOM_ACCESS_FAILURE,            ACTION_12 },
   {  7, 19, MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED,ACTION_12 },
   {  7, 19, MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE,        ACTION_12 },
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   {  7,  7, MM_STD_LCS_FINISHED,                     ACTION_0 },
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
#ifdef FEATURE_DUAL_SIM
   {  7, 19, MM_STD_RADIO_UNAVAILABLE,                ACTION_12 },
#endif
#ifdef FEATURE_CCBS
   {  8,  6, MM_STD_CM_SERVICE_PROMPT_ACCEPT,         ACTION_0 },
   {  8,  9, MM_STD_LAST_CONNECTION_RELEASED,         ACTION_33 },
   {  8, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_15 },
   {  8, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_0 },
   {  8,  9, MM_STD_RR_CONNECTION_ABORTED,            ACTION_0 },
   {  8,  9, MM_STD_ABORT_MESSAGE_RECEIVED,           ACTION_0 },
   {  8,  9, MM_STD_AUTHENTICATION_REJECT,            ACTION_11 },
   {  8,  5, MM_STD_SERVICE_REQUEST_SENT,             ACTION_3 },
   {  8,  7, MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED,    ACTION_13 },
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   {  8,  8, MM_STD_LCS_STARTED,                      ACTION_32 },
   {  8, 25, MM_STD_RR_CONNECTION_REL_NOT_ALLOWED,    ACTION_0 },
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
#endif /*FEATURE_CCBS */

   {  9,  5, MM_STD_SERVICE_REQUEST_SENT,             ACTION_0  },
   {  9,  6, MM_STD_NETWORK_MESSAGE_RECEIVED,         ACTION_2  },
   {  9,  7, MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED,    ACTION_13 },
   {  9,  9, MM_STD_CIPHERING_HAS_STARTED,            ACTION_0  },
   {  9,  9, MM_STD_LAST_CONNECTION_RELEASED,         ACTION_0  },
   {  9,  5, MM_STD_MM_CONNECTION_REQUEST,            ACTION_3  },
   {  9,  9, MM_STD_T3240_TIMEOUT,                    ACTION_0  },
   {  9, 19, MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED,ACTION_19 },
   {  9, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_15 },
   {  9, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_26 },
   {  9, 19, MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE,        ACTION_26 },
   {  9, 19, MM_STD_RANDOM_ACCESS_FAILURE,            ACTION_26 },
   {  9,  0, MM_STD_MS_DEACTIVATED_NO_ATTACH,         ACTION_4  },
   {  9,  9, MM_STD_RR_CONNECTION_ESTABLISHED,        ACTION_0  },
   {  9,  9, MM_STD_AUTHENTICATION_REJECT,            ACTION_0  },
   {  9,  9, MM_STD_LU_REQUIRED,                      ACTION_28 },
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   {  9, 25, MM_STD_LCS_STARTED,                      ACTION_32 },
   {  9, 25, MM_STD_LCS_IN_PROGRESS,                  ACTION_15 },
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
   {  9, 9,  MM_STD_RR_CONNECTION_ABORTED,            ACTION_0 },
#ifdef FEATURE_CCBS
   {  9, 8, MM_STD_CM_SERVICE_PROMPT_RECEIVED,        ACTION_0 },
#endif /*FEATURE_CCBS */
#ifdef FEATURE_DUAL_SIM
   { 9, 19, MM_STD_RADIO_UNAVAILABLE,                 ACTION_26 },
#endif

   { 10, 10, MM_STD_RR_CONNECTION_ABORTED,            ACTION_0  },
   { 10, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_10 },
   { 10, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_10  },
   { 10, 10, MM_STD_T3240_TIMEOUT,                    ACTION_0  },
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   { 10, 25, MM_STD_LCS_IN_PROGRESS,                  ACTION_10 },
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
   { 10,  5, MM_STD_MM_CONNECTION_REQUEST,            ACTION_3  },

   { 13,  3, MM_STD_RR_CONNECTION_ESTABLISHED,        ACTION_0  },
   { 13, 13, MM_STD_LU_REQUIRED,                      ACTION_0  },
   { 13, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_10 },
   { 13, 19, MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE,        ACTION_10 },
   { 13, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_10 },
   { 13, 19, MM_STD_RANDOM_ACCESS_FAILURE,            ACTION_27 },
   { 13, 13, MM_STD_T3210_TIMEOUT,                    ACTION_0 },
   { 13, 14, MM_STD_MM_CONNECTION_REQUEST,            ACTION_0  },
   { 13, 19, MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED,ACTION_10 },
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
   { 13, 19, MM_STD_CCO_FAILED,                       ACTION_34 },
#endif   
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   { 13, 19, MM_STD_RADIO_UNAVAILABLE,                ACTION_10 },
#endif
   { 13, 13, MM_STD_REESTABLISHMENT_REQUIRED,         ACTION_0 },
   { 14,  5, MM_STD_RR_CONNECTION_ESTABLISHED,        ACTION_3  },
   { 14, 14, MM_STD_LU_REQUIRED,                      ACTION_28 },
   { 14, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_19 },
   { 14, 19, MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE,        ACTION_19 },
   { 14, 19, MM_STD_RANDOM_ACCESS_FAILURE,            ACTION_19 },
   { 14,  9, MM_STD_LAST_CONNECTION_RELEASED,         ACTION_6  },
   { 14,  9, MM_STD_RR_CONNECTION_ABORTED,            ACTION_0  },
   { 14, 14, MM_STD_REESTABLISHMENT_REQUIRED,         ACTION_0 },
   { 14, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_0  },
   { 14, 19, MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED,ACTION_19 },
   { 14, 14, MM_STD_MM_CONNECTION_REQUEST,            ACTION_0  },
#ifdef FEATURE_DUAL_SIM
   { 14, 19, MM_STD_RADIO_UNAVAILABLE,                ACTION_19 },
#endif

   { 15,  7, MM_STD_RR_CONNECTION_ESTABLISHED,        ACTION_1  },
   { 15, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_12  },
   { 15, 19, MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE,        ACTION_12  },
   { 15, 19, MM_STD_RANDOM_ACCESS_FAILURE,            ACTION_12 },
   { 15, 19, MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED, ACTION_12 },
   { 15, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_12  },
#ifdef FEATURE_DUAL_SIM
   { 15, 19, MM_STD_RADIO_UNAVAILABLE,                ACTION_12 },
#endif

   { 17,  6, MM_STD_CIPHERING_HAS_STARTED,            ACTION_23 },
   { 17,  6, MM_STD_CNM_SERVICE_ACCEPT,               ACTION_23 },
   { 17,  9, MM_STD_ABORT_MESSAGE_RECEIVED,           ACTION_0  },
   { 17,  9, MM_STD_SERVICE_REJECT,                   ACTION_18 },
   { 17,  9, MM_STD_SERVICE_REJECT_CAUSE_4_6,         ACTION_18 },
   { 17, 17, MM_STD_NETWORK_MESSAGE_RECEIVED,         ACTION_0  },
   { 17, 17, MM_STD_LU_REQUIRED,                      ACTION_28 },
   { 17, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_18  },
   { 17,  9, MM_STD_LAST_CONNECTION_RELEASED,         ACTION_20 },
   { 17, 19, MM_STD_RANDOM_ACCESS_FAILURE,            ACTION_18 },
   { 17, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_18 },
   { 17, 19, MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED,ACTION_18 },
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   { 17, 17, MM_STD_LCS_STARTED,                      ACTION_32 },
   { 17, 17, MM_STD_LCS_FINISHED,                     ACTION_0 },
   { 17, 25, MM_STD_LCS_IN_PROGRESS,                  ACTION_18 },
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
#ifdef FEATURE_DUAL_SIM
   { 17, 19, MM_STD_RADIO_UNAVAILABLE,                ACTION_18 },
#endif

   { 18, 19, MM_STD_ATTACH_LU_NOT_NEEDED,             ACTION_0  },
   { 18,  0, MM_STD_MS_DEACTIVATED_NO_ATTACH,         ACTION_4  },

   { 19,  0, MM_STD_MS_DEACTIVATED_NO_ATTACH,         ACTION_4  },
   { 19,  9, MM_STD_RR_CONNECTION_ESTABLISHED,        ACTION_0  },
   { 19, 13, MM_STD_LU_REQUIRED,                      ACTION_5  },
   { 19, 14, MM_STD_MM_CONNECTION_REQUEST,            ACTION_0  },
   { 19, 15, MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED,    ACTION_13 },
   { 19, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_0  },
   { 19, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_0  },
   { 19, 19, MM_STD_LAST_CONNECTION_RELEASED,         ACTION_14 },
   { 19, 19, MM_STD_MS_NREG_REQ_NO_POWER_DOWN,        ACTION_0  },
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   { 19, 19, MM_STD_LCS_STARTED,                      ACTION_32 },
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
   { 19, 23, MM_STD_COMBINED_RA_UPDATE_INITIATED,     ACTION_0  },
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   { 19, 19, MM_STD_RADIO_UNAVAILABLE,                ACTION_0 },
#endif
#ifdef FEATURE_LTE
   { 19, 0,  MM_STD_LOCALLY_DEACTIVATE_MM,            ACTION_35 },
   { 19, 6,  MM_STD_SRVCC_HANDOVER,                   ACTION_0 },
#endif

   { 20,  6, MM_STD_CIPHERING_HAS_STARTED,            ACTION_22 },
   { 20,  6, MM_STD_CNM_SERVICE_ACCEPT,               ACTION_22 },
   { 20,  6, MM_STD_T3230_TIMEOUT,                    ACTION_19 },
   { 20,  6, MM_STD_SERVICE_REJECT,                   ACTION_0  },
   { 20,  9, MM_STD_SERVICE_REJECT_CAUSE_4_6,         ACTION_11 },
   { 20,  9, MM_STD_ABORT_MESSAGE_RECEIVED,           ACTION_0  },
   { 20, 19, MM_STD_LOW_LEVEL_FAIL_REEST_NOT_ALLOWED, ACTION_17 },
   { 20, 22, MM_STD_LOW_LEVEL_FAIL_REEST_ALLOWED,     ACTION_16 },
   { 20,  9, MM_STD_RR_CONNECTION_ABORTED,            ACTION_17 },
   { 20, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_0  },
   { 20, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_17 },
   { 20, 20, MM_STD_NETWORK_MESSAGE_RECEIVED,         ACTION_0  },
   { 20, 20, MM_STD_LU_REQUIRED,                      ACTION_28  },
   { 20,  9, MM_STD_LAST_CONNECTION_RELEASED,         ACTION_20 },
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   { 20, 20, MM_STD_LCS_STARTED,                      ACTION_32 },
   { 20, 20, MM_STD_LCS_FINISHED,                     ACTION_0 },
   { 20, 25, MM_STD_RR_CONNECTION_REL_NOT_ALLOWED,    ACTION_29 },
   { 20, 25, MM_STD_LCS_IN_PROGRESS,                  ACTION_0 },
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

   { 20, 14, MM_STD_REESTABLISHMENT_REQUIRED,         ACTION_0 },

   { 21,  9, MM_STD_ABORT_MESSAGE_RECEIVED,           ACTION_0  },
   { 21, 17, MM_STD_RR_CONNECTION_ESTABLISHED,        ACTION_3  },
   { 21, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_18 },
   { 21, 19, MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE,        ACTION_18 },
   { 21, 19, MM_STD_RANDOM_ACCESS_FAILURE,            ACTION_18 },
   { 21, 19, MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED,ACTION_18 },
   { 21,  9, MM_STD_LAST_CONNECTION_RELEASED,         ACTION_6  },
   { 21, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_18 },
   { 21, 21, MM_STD_LU_REQUIRED,                      ACTION_28 },
   { 21, 19, MM_STD_MMCC_REEST_REJ,                   ACTION_0},
#ifdef FEATURE_DUAL_SIM
   { 21, 19, MM_STD_RADIO_UNAVAILABLE,                ACTION_18 },
#endif

   { 22,  9, MM_STD_ABORT_MESSAGE_RECEIVED,           ACTION_0  },
   { 22, 19, MM_STD_LAST_CONNECTION_RELEASED,         ACTION_14 },
   { 22, 21, MM_STD_REESTABLISHMENT_REQUESTED,        ACTION_25 },
   { 22, 22, MM_STD_LU_REQUIRED,                      ACTION_28 },

   { 23, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_0  },

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   { 25,  0, MM_STD_MS_DEACTIVATED_NO_ATTACH,         ACTION_4  },
   { 25,  5, MM_STD_SERVICE_REQUEST_SENT,             ACTION_3  },
   { 25,  6, MM_STD_NETWORK_MESSAGE_RECEIVED,         ACTION_2  },
   { 25,  7, MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED,    ACTION_13 },
   { 25,  9, MM_STD_LCS_FINISHED_RR_CONN_ACTIVE,      ACTION_31 },
   { 25,  3, MM_STD_LU_REQUIRED,                      ACTION_5  },
   { 25, 19, MM_STD_RR_CONNECTION_RELEASED,           ACTION_15 },
   { 25, 19, MM_STD_LOW_LEVEL_FAIL,                   ACTION_0 },
   { 25, 19, MM_STD_RANDOM_ACCESS_FAILURE,            ACTION_0 },
   { 25, 25, MM_STD_RR_CONNECTION_ABORTED,            ACTION_0 },
#ifdef FEATURE_DUAL_SIM
   { 25, 19, MM_STD_RADIO_UNAVAILABLE,                ACTION_0 },
#endif
#endif
   /* THIS MUST BE LAST */

   {  0,  0, MM_STD_INVALID_EVENT,                    ACTION_0  }
};

/* -------------------
** Function prototypes
** ------------------- */
void mm_check_ra_retry( void );


/*===========================================================================

FUNCTION MM_STATE_CONTROL

DESCRIPTION
  This function implements the state transition diagram of 2.4-s1.
  The diagram is encoded as a table of current state, event,
  next state, and action number. The function searches for the
  entry corresponding to the curent state and event, performs
  the action, and sets the state to be next_state.

  Note: This function may be reentered if any of the functions
        calls need to initiate a state change.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_state_control( mm_event_T mm_event )
{

   /* Set action to a non-existing action as a default*/
   byte              action = 0xFF;
   boolean           entry_found;
   byte              entry;
   byte              current_state;
   byte              next_state;
   reject_type_enum_T cause_type;
   reject_cause_T cause_value;
#ifdef FEATURE_DUAL_SIM
   sp_mt_call_info mt_call_subs_update = {SP_TECH_MT_CALL_STATE_NONE, SYS_MODEM_AS_ID_NO_CHANGE, SYS_SYS_MODE_MAX};            
#endif   
   mmr_service_ind_s_type mmr_service_ind;

   byte           table_size = (sizeof(table)/sizeof(state_event_T));/*Klocwork fix*/
   current_state = mm_state;
   next_state    = mm_state;
   entry_found   = FALSE;
   entry         = 0;

/*
 * FOR all entries in table
 */

   while (table[entry].event != MM_STD_INVALID_EVENT )
   {
      if ( table[entry].current_state == mm_state )
      {
         if (table[entry].event ==  mm_event )
         {
            next_state = table[entry].next_state;

            action = table[entry].action_code;
            entry_found = TRUE;
            break;
         }
      }
      entry++;
      if(entry >= table_size) /*Klocwork fix to ensure array is accessed with in the bounds*/
      {
         MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
         break;
      }
   }

   if (entry_found)
   {
      MSG_HIGH_DS_3( MM_SUB, "=MM= MM State change from %d to %d, evt:%d", mm_state, next_state, mm_event );
      if ((mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page) &&
          (mm_timer_status[TIMER_SCRI_PAGING] != TIMER_STOPPED) && 
          (current_state == MM_WAIT_FOR_NETWORK_COMMAND) && 
          (next_state != MM_WAIT_FOR_NETWORK_COMMAND)) 
      {
         mm_stop_timer(TIMER_SCRI_PAGING);
      }
      mm_state = next_state;
      if(mm_state == MM_IDLE) 
      {
        mm_connection_activated = FALSE;
      }
      else if(mm_state == MM_CONNECTION_ACTIVE)
      {
         mm_connection_activated = TRUE;
      }
      mmsend_mm_state_log_packet();

      if (is_umts_mode() && (mm_state == MM_IDLE))
      {
        auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED,RRC_CS_DOMAIN_CN_ID) ;
      }

      switch (action)
      {
         case ACTION_0:      /* NULL action, do nothing, go to next_state */
            break;

         case ACTION_1:
            mm_start_timer( TIMER_T3220, DEFAULT_TIMEOUT );
            break;

         case ACTION_2:      /* */
            mm_stop_timer( TIMER_T3212 );
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
            /*
            * RR(C) DATA IND is received in state 9, 25, and
            * MM is now in state 6
            */
            mm_stop_timer( TIMER_T3240 );
            mm_stop_timer( TIMER_T3241 );
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

            /* 
             *  Following condition takes care of conflict between MO and MT CS call
             *  on a target/mode where CNm has single stack and MM has multiple stacks 
             *  but there's single RF chain and TRM arbitration is needed.
             *  
             * If next state in MM for this subscription is MM_ACTIVE,
             * then check that MM aborts connection establishment procedures 
             * on other subscriptions.
             *
             */
#ifdef FEATURE_DUAL_SIM     
            if(next_state == MM_CONNECTION_ACTIVE && 
               current_state == MM_WAIT_FOR_NETWORK_COMMAND)
            {
               mm_check_other_mm_cnm_connections();
            }
#endif

            if (mm_held_cnm_service_req.present)
            {
              if (((mm_held_cnm_service_req.establishment_cause == (int)MM_RR_SHORT_MESSAGE ) ||
                    (mm_held_cnm_service_req.establishment_cause == (int)MM_RR_SUPPLEMENTARY_SERVICE )) &&
                   (mm_location_information.location_update_status == UPDATED )&&
                   (memcmp( mm_serving_lai,mm_location_information.lai, LAI_SIZE ) == 0 ))
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= Process pended MO SMS on receiving SETUP from network ");
                mm_check_pending_requests();
              }
              else
              {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                mm_send_mmcnm_est_rej(
                        AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL ,MMCC_SMS_CS);
#else
                mm_send_mmcnm_est_rej(
                        AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL );
#endif
              }
            } 

            break;

         case ACTION_3:
            mm_start_timer( TIMER_T3230, DEFAULT_TIMEOUT );
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
            /*
            * CM service req has been sent (25)-->(5)
            * Stop T3241, just in case it was running
            */
            mm_stop_timer( TIMER_T3241 );
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
            break;

         case ACTION_4:      /* deactivate rr */

            /*
             * nothing to do here for SIM as it is always kept up to date with
             * location update status
             */

            if (mm_rat_change_pending)
            {
              mm_stop_current_rat();
            }
            else
            {
              mm_deactivate_rr();
            }
            break;

         case ACTION_5:      /* send_location_update_request, stop_T3212 */
            mm_send_lu_request(
                      mm_get_current_security_context( RRC_CS_DOMAIN_CN_ID ));
            if((mm_serving_plmn.ms_op_mode != SYS_SRV_DOMAIN_CS_ONLY) ||
               (gmm_state == GMM_DEREGISTERED) ||
               (gmm_state == GMM_NULL))
            {
              mm_serv_change_pending = FALSE;
            }
            break;

         case ACTION_6:
            mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );
            mm_start_timer(TIMER_T3240, DEFAULT_TIMEOUT);
            break;

         case ACTION_7:      /* stop_T3210, PLMN search */
            mm_stop_timer( TIMER_T3210 );
            mm_substate_control( MM_STD_PLMN_SEARCH );
            break;

         case ACTION_8:      /* stop T3210 */
            mm_stop_timer( TIMER_T3210 ) ;

            if ((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) ||
                (mm_state == MM_LOCATION_UPDATE_REJECTED))
            {
              mm_start_timer (TIMER_T3240, DEFAULT_TIMEOUT );
            }

            mm_check_for_detach();
            break;

         case ACTION_9:      /* Not used */
            break;

         case ACTION_10:     /* check_lu_fail_action */

              if(!(current_state == MM_LOCATION_UPDATE_REJECTED && mm_state ==  MM_IDLE))
              {
                 mmsend_nas_reg_event_packet(PROC_NAS_LAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
              }
              else 
              {
                 mmsend_nas_reg_event_packet(PROC_NAS_LAU_EVENT,PROC_END,PROC_END_REJECTED,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
              }

            mm_check_lu_fail_action();

            
            break;

         case ACTION_11:
            mm_start_timer( TIMER_T3240, DEFAULT_TIMEOUT );
            mm_stop_timer( TIMER_T3212 );
            break;

         case ACTION_12:     /* IMSI detach complete */
            if (mm_timer_status[TIMER_T3220] != TIMER_STOPPED)
            {
              mm_stop_timer( TIMER_T3220 );
            }
            if(((gmm_detach_type != GMM_MO_IMSI_DETACH) && (gmm_detach_type != GMM_COMBINED_GPRS_IMSI_DETACH) && mm_state == MM_IDLE)
                || (mm_state == MM_IDLE && current_state == MM_IMSI_DETACH_INITIATED))
            {
            if(mm_event == MM_STD_RR_CONNECTION_RELEASED)
            {
                mmsend_nas_reg_event_packet(PROC_NAS_IMSI_DETACH_EVENT,PROC_END,PROC_END_ACCEPT,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
            }
            else
            {
                mmsend_nas_reg_event_packet(PROC_NAS_IMSI_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
            }
            }

            mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                            SYS_SRV_DOMAIN_NO_SRV,
                                            FALSE,
                                            SYS_SRV_STATUS_LIMITED,
                                            TRUE );

            /* Incase of IMSI DETACH stop the following timers */
            mm_stop_timer( TIMER_T3211 );
            mm_stop_timer( TIMER_T3212 );
            mm_stop_timer( TIMER_T3213 );
#ifdef FEATURE_DUAL_SIM
            mm_stop_timer( TIMER_MM_RADIO_RETRY );
#endif
            mm_initiate_lu_upon_entry_into_new_cell = FALSE;
#ifdef FEATURE_NAS_ECALL
            if (mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY)
            {
              mm_perform_ecall_inactivity();
            }
#endif
            break;

         case ACTION_13:     /* send_imsi_detach */
            mm_send_imsi_detach();
            mmsend_nas_reg_event_packet(PROC_NAS_IMSI_DETACH_EVENT,PROC_START,PROC_END_NO_REASON,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
            mm_possible_imsi_attach = TRUE;
            break;

         case ACTION_14:
            mm_stop_timer( TIMER_T3240 );
            mm_send_mmcnm_rel_cnf();
            break;

         case ACTION_15:
            if( (mm_held_cnm_service_req.present == TRUE) &&
                 (mm_held_cnm_service_req.establishment_cause != (int)MM_RR_EMERGENCY_CALL) &&
                 ((mm_location_information.location_update_status == ROAMING_NOT_ALLOWED) ||
                  (mm_location_information.location_update_status == NOT_UPDATED) ||
                  ( memcmp( mm_serving_lai, mm_location_information.lai, LAI_SIZE ) != 0 )))
            {
              MSG_HIGH_DS_1(MM_SUB,"=MM= MM-REJ UE update status = %d", mm_location_information.location_update_status);
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
              mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE, MMCC_SMS_CS);
#else
              mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
            }
            else
            {
              mm_check_pending_requests();
            }
            break;

         case ACTION_16:     /* Re-establishment may be possible */
            if (mm_event != MM_STD_LOW_LEVEL_FAIL_REEST_ALLOWED)
            {
                mm_notify_cnm_mm_connection_failure( TRUE );
            }
            break;

         case ACTION_17:     /* Re-establishment not possible */
            if (mm_event != MM_STD_LOW_LEVEL_FAIL_REEST_NOT_ALLOWED)
            {
                mm_notify_cnm_mm_connection_failure( FALSE );
            }
            break;

         case ACTION_18:     /* notify_reestablishment_fail, */
            mm_get_mmcc_rej_cause(table[entry].event, &cause_type, &cause_value);
            mm_notify_reestablishment_fail(cause_type,cause_value);
            break;

         case ACTION_19:
            /* Send a establish reject to CNM */
            if (mm_event == MM_STD_T3230_TIMEOUT)
            {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_TIMER_T3230_EXP, MMCC_SMS_CS);
#else
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_TIMER_T3230_EXP);
#endif
            }
            else if (mm_event == MM_STD_LOW_LEVEL_FAIL)  /* RR(C)_ABORT_IND/CNF */
            {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_CS);
#else
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
            }
            else if (mm_event == MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE)  /* RR(C)_ABORT_IND */
            {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_CS);
#else
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
            }
            else if (mm_event == MM_STD_RANDOM_ACCESS_FAILURE)
            {
               /* When NAS receives RACH failure indication, silent redial should not be done */
               if(gmm_anite_gcf_flag)
               {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                 mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED, MMCC_SMS_CS);
#else
                 mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED);
#endif
               }
               else
               {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                 mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_CS);
#else
                 mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
               }
            }
            else if (mm_event == MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED)  /* RR(C)_ABORT_IND/CNF */
            {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED, MMCC_SMS_CS);
#else
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED);
#endif
            }
#ifdef FEATURE_DUAL_SIM
            else if (mm_event == MM_STD_RADIO_UNAVAILABLE)
            {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_ABORT_RADIO_UNAVAILABLE, MMCC_SMS_CS);
#else
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_ABORT_RADIO_UNAVAILABLE);
#endif
            }
#endif
            else
            {
                MSG_HIGH_DS_0(MM_SUB, "=MM= Wrong MM event for mmcnm_est_rej");
            }

            break;

         case ACTION_20:
            mm_send_cm_service_abort();
            mm_start_timer( TIMER_T3240, DEFAULT_TIMEOUT );
            break;

         case ACTION_21:     /* confirm mm connection, reset T3212 */

            if((mm_timer_status [ TIMER_T3211] != TIMER_STOPPED) &&
               (mm_idle_substate == MM_LIMITED_SERVICE))
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= MM conn cnf: mm_previous_idle_substate is reset");
              mm_previous_idle_substate = NULL;
            }

            if ( (mm_location_information.location_update_status == UPDATED ) &&
                 ( memcmp( mm_serving_lai, mm_location_information.lai, LAI_SIZE ) == 0 ))
            {
               boolean mm_inform_service_ind = FALSE;
               if(mm_pending_lu_request != LTE_IRAT_LU)
               {
                  if(mm_timer_status[TIMER_T3211] != TIMER_STOPPED || mm_service_ind_blocked_due_to_cs_reg == TRUE)
                  {
                    mm_inform_service_ind = TRUE;
                  }

                  mm_stop_timer( TIMER_T3211);
                  mm_stop_timer( TIMER_T3213);
                  mm_pending_lu_request = NO_LU;
                  if(mm_cnm_srv_est_cause != MM_RR_EMERGENCY_CALL)
                  {
                    mm_idle_transition_timer = NULL;
                  }
               }
#ifdef FEATURE_DUAL_SIM			   
			   if((gmm_state == GMM_DEREGISTERED_INITIATED )
                        && (gmm_force_local_detach_sim[mm_sub_id] == TRUE)
                        && (mm_timer_status[TIMER_T3321] == TIMER_ACTIVE)) 
               {
                 MSG_HIGH_DS_0( MM_SUB, "=MM= Preventing mmr_reg_cnf (if liable to send)as PS Detach for dds switch is in progress");
               }
			   else
#endif
               if (mm_reg_waiting_for_reg_cnf &&
                   (mm_connection_type != EMERGENCY_MM_CONNECTION))
               {
                  MSG_HIGH_DS_0(MM_SUB,"=MM=Sending MMR_REG_CNF ");
                  mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                  mm_send_mmr_reg_cnf();
                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
               }
               else if(((mm_inform_service_ind == TRUE) || (mm_service_ind_blocked_due_to_irat_cs_reg == TRUE))&&
                      (mm_connection_type != EMERGENCY_MM_CONNECTION))
               { 
                   mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                   mmr_service_ind.service_state = mm_serving_plmn.info;
                   mm_send_mmr_service_ind( &mmr_service_ind );
               }
               else if ((mm_pending_lu_request == LTE_IRAT_LU) && 
                   (mm_connection_type != EMERGENCY_MM_CONNECTION))
               {               
                   mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                   mmr_service_ind.service_state = mm_serving_plmn.info;
                   mm_send_mmr_service_ind( &mmr_service_ind );
               }
            }
            mm_confirm_mm_connection(MS_CC_MM);
            mm_stop_timer( TIMER_T3212 );
            break;

         case ACTION_22:     /* confirm mm connection */

            if ( (mm_location_information.location_update_status == UPDATED ) &&
                 ( memcmp( mm_serving_lai, mm_location_information.lai, LAI_SIZE ) == 0 ) &&
                 (mm_pending_lu_request != LTE_IRAT_LU) )
            {
               mm_stop_timer( TIMER_T3211);             
               mm_stop_timer( TIMER_T3213);
               mm_pending_lu_request = NO_LU;
               mm_idle_transition_timer = NULL;
            }
             
            mm_confirm_mm_connection(MS_CC_MM) ;
            break;

         case ACTION_23:     /* confirm_reestablishment */

            if ( (mm_location_information.location_update_status == UPDATED ) &&
                 ( memcmp( mm_serving_lai, mm_location_information.lai, LAI_SIZE ) == 0 ) &&
                 (mm_pending_lu_request != LTE_IRAT_LU) )
            {
               mm_stop_timer( TIMER_T3211);             
               mm_stop_timer( TIMER_T3213);
               mm_pending_lu_request = NO_LU;
               mm_idle_transition_timer = NULL;
            }

            mm_confirm_reestablish();
            break;
         case ACTION_24:     /* check_for_detach */
            mm_start_timer( TIMER_T3240, DEFAULT_TIMEOUT );
            mm_check_for_detach();
            break;

         case ACTION_25:     /* send_cnm_reestablishment_request */
            mm_reestablish_connection(
                      mm_get_current_security_context( RRC_CS_DOMAIN_CN_ID )) ;
            break;

         case ACTION_26:
            if (mm_cnm_rel_req_received)
            {
               mm_send_mmcnm_rel_cnf();
            }
            mm_stop_timer (TIMER_T3240);  //T3240 was not stopped upon moving from 9 to 19
            break;

         case ACTION_27:     /* check_ra_retry */
            mm_stop_timer( TIMER_T3210 ) ;
            mm_check_ra_retry();
            if (mm_held_cnm_service_req.present)
            {
              if ((( mm_location_information.location_update_status == UPDATED )&&
                   ( memcmp( mm_serving_lai,
                     mm_location_information.lai,
                     LAI_SIZE ) == 0 ))
                   ||
                  (mm_held_cnm_service_req.establishment_cause == (int)MM_RR_EMERGENCY_CALL))
              {
                mm_check_pending_requests();
              }
              else
              {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                mm_send_mmcnm_est_rej(
                    AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL ,MMCC_SMS_CS);
#else
                mm_send_mmcnm_est_rej(
                    AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL );
#endif
              }
            }

            break;

         case ACTION_28:
            if (mm_get_lu_cause() == MM_LU_CAUSE_IMSI_ATTACH)
            {
              mm_pending_lu_request = IMSI_ATTACH;
            }
            else if (mm_get_lu_cause() == MM_LU_CAUSE_PERIODIC)
            {
              mm_pending_lu_request = T3212_EXPIRED;
            }
            else if(mm_pending_lu_request != LTE_IRAT_LU)
            {
              mm_pending_lu_request = NORMAL_LU;
            }
            break;

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
         case ACTION_29:
            mm_send_cm_service_abort();
            break;

         case ACTION_30:     /* check_for_detach */
            mm_check_for_detach();
            break;

         case ACTION_31:
            mm_stop_timer( TIMER_T3241 );
            mm_start_timer( TIMER_T3240, DEFAULT_TIMEOUT );
            break;

         case ACTION_32:
            mm_lcs_active = TRUE;
            MSG_HIGH_DS_0(MM_SUB,"=MM= MM rec'd LCS_ACTIVE_IND - LCS started");
            break;

#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
#ifdef FEATURE_CCBS
         case ACTION_33:
            mm_start_timer( TIMER_T3240, DEFAULT_TIMEOUT );
            break;
#endif
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
         case ACTION_34:
            //restore CS as CCO failed
            if(mm_location_information.location_update_status == UPDATED)
            {
                mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                SYS_SRV_DOMAIN_CS_ONLY,
                                                FALSE,
                                                SYS_SRV_STATUS_SRV,
                                                FALSE );
            }
            mm_substate_control( MM_STD_NORMAL_SERVICE );
            break;
#endif

#ifdef FEATURE_LTE
         case ACTION_35:
            mm_reset_global_variables();
            break;          
#endif 
#ifdef FEATURE_FEMTO_CSG
         case ACTION_36:
            mm_start_timer( TIMER_T3240, DEFAULT_TIMEOUT );
            break;
#endif 

         default:
            MSG_ERROR_DS(MM_SUB, "=MM= Unexpected action code: %d", action, 0, 0);
            break;
      }

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
      if (((next_state == MM_IDLE) || (next_state == MM_NULL)) &&
          (mm_lcs_active))
      {
         mm_abort_lcs();
      }
      /*
      *  If transitioning into state 25, stop T3240 and start T3241
      */
      if (next_state == MM_RR_CONNECTION_RELEASE_NOT_ALLOWED)
      {
        mm_stop_timer(TIMER_T3240);

        if (mm_timer_status[TIMER_T3241] != TIMER_ACTIVE)
        {
           mm_start_timer(TIMER_T3241, DEFAULT_TIMEOUT);
        }
      }
      /* If transitioning out of state 25, stop T3241 */
      if (current_state == MM_RR_CONNECTION_RELEASE_NOT_ALLOWED)
      {
         mm_stop_timer(TIMER_T3241);
      }
#endif
#ifdef FEATURE_LTE
      if(is_gsm_mode())
      {
        if((mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG) ||
           (mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG))
        {
          if((current_state == MM_IDLE) && 
             ((next_state == MM_WAIT_FOR_RR_CONNECTION_MM) ||
              (next_state == MM_WAIT_FOR_NETWORK_COMMAND)))
          {
            mm_send_rr_csfb_blind_redir_to_lte_req(FALSE);
          }
          else if((current_state == MM_IDLE) && 
                  (next_state == MM_WAIT_FOR_RR_CONNECTION_LU ))
          {
            MSG_HIGH_DS_0(MM_SUB,"=MM= MM set mm_csfb_is_blind_redir_to_lte_req_to_be_sent flag");
            mm_csfb_is_blind_redir_to_lte_req_to_be_sent = TRUE;
            emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = FALSE;
          }
        }
        else if((mm_csfb_is_blind_redir_to_lte_req_to_be_sent == TRUE) &&
                (((current_state == MM_LOCATION_UPDATE_INITIATED) && 
                  (next_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION)) ||
                 ((current_state == MM_WAIT_FOR_NETWORK_COMMAND) && 
                  (next_state == MM_CONNECTION_ACTIVE))))
        {
          mm_send_rr_csfb_blind_redir_to_lte_req(FALSE);
        }
        else if( emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort == TRUE && 
                 current_state == MM_LOCATION_UPDATE_INITIATED && next_state == MM_WAIT_FOR_NETWORK_COMMAND)
        {
          mm_send_rr_csfb_blind_redir_to_lte_req(TRUE);
        }
      }
#endif


#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
      /*
       *  If transitioning to MM_IDLE or to MM_RR_CONNECTION_RELEASE_NOT_ALLOWED,
       *  check to see if a response to REG or to CNM is needed now
       */
      if (( current_state != MM_IDLE ) &&
          ((next_state == MM_IDLE) || (next_state == MM_RR_CONNECTION_RELEASE_NOT_ALLOWED)))
      {
#ifdef FEATURE_GSTK
         if (current_state != MM_WAIT_FOR_RR_ACTIVE && next_state == MM_IDLE)
         {
           mm_send_gstk_mm_idle_info();
         }
#endif /* FEATURE_GSTK */

#else
      if (( current_state != MM_IDLE ) && (next_state == MM_IDLE))
      {
#ifdef FEATURE_GSTK
         if (current_state != MM_WAIT_FOR_RR_ACTIVE) 
         {
           mm_send_gstk_mm_idle_info();
         }
#endif /* FEATURE_GSTK */
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

#ifdef FEATURE_DUAL_SIM
         /*
           Inform MCS module that MM connection which was brought up for CS page response
           has been released.
         */
           if(mm_last_paging_cause != 0xFF)
           {
              mt_call_subs_update.state = SP_TECH_MT_CALL_RELEASE;
            mt_call_subs_update.subscription_id = (sys_modem_as_id_e_type)mm_sub_id;
            mm_convert_rat_to_tech_id(mm_serving_plmn.info.active_rat, &mt_call_subs_update.tech_id);
            MSG_HIGH_DS_3( MM_SUB, "=MM= MCS being informed of SP_TECH_MT_CALL_RELEASE by NAS state = %d, subs_id = %d, tech_id = %d", mt_call_subs_update.state, 
                                                                             mt_call_subs_update.subscription_id, 
                                                                             mt_call_subs_update.tech_id );
              (void)sp_subscription_mt_call_info_update(mt_call_subs_update);
           }
#endif
         
         mm_last_paging_cause = 0xFF;
     /* MM releases control of resel decision when MM state becomes IDLE */
         mm_controls_resel_dec_mt_cs_call = FALSE;

         mm_is_cs_connected_due_to_srvcc  = FALSE;

         if (!mmsim_sim_valid( SYS_SRV_DOMAIN_CS_ONLY ))
         {
           /* -----------------------------------------------------------------------
           ** When returning to MM_IDLE, if no SIM is present, or if the inserted SIM
           ** is considered invalid by the MS, the state is NO_IMSI (24.008 4.2.3).
           ** ----------------------------------------------------------------------- */
           mm_substate_control( MM_STD_NO_IMSI );
         }

         if(mm_csfb_service_ind_status == MM_CSFB_SEND_STATUS_LIMITED_REGIONAL)
         {
           MSG_HIGH_DS_0(MM_SUB,"=MM= Force NO Service after limited regional service status");
           mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
           mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
           mmr_service_ind.service_state = mm_serving_plmn.info;
           mm_send_mmr_service_ind( &mmr_service_ind );    
           mm_csfb_service_ind_status = MM_CSFB_SEND_SERVICE_STATUS_NONE;
         }

         if((gmm_foreground_search_pending) &&
            (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
            (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))

         {
           gmm_foreground_search_pending = FALSE;
           if (mm_reg_waiting_for_reg_cnf )
           {
             mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH;
             mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
             mm_ready_to_send_reg_cnf = TRUE;
              mm_send_mmr_reg_cnf();
              mm_reg_waiting_for_reg_cnf = FALSE;
           }
           else
           {
             mmr_service_ind.cause = MMR_CAUSE_FAILURE_FOREGROUND_PLMN_SEARCH;
             mmr_service_ind.service_state = mm_serving_plmn.info;
             mm_send_mmr_service_ind( &mmr_service_ind );
           }
         }
//FR15492
         else if(mm_pending_no_service_to_reg &&
                 (reg_state_is_no_service_to_be_send_due_to_china_mcc(mm_serving_plmn.info.plmn)))
         {
           mm_pending_no_service_to_reg = FALSE;
           mm_substate_control( MM_STD_NO_SERVICE );
           gmm_set_substate(GMM_NO_CELL_AVAILABLE);
           mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
           mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
           mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                          mm_serving_plmn.info.reg_service_domain,
                                          TRUE,
                                          SYS_SRV_STATUS_NO_SRV,
                                          FALSE );
           
           if(mm_reg_waiting_for_reg_cnf)
           {
             mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
             /* Set the PLMN service stat information in the REG_CNF primitive */
             mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
             mm_send_mmr_reg_cnf();
             mm_reg_waiting_for_reg_cnf = FALSE;
             mm_ready_to_send_reg_cnf   = FALSE;
           }
         else
           {
             mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
             mmr_service_ind.service_state = mm_serving_plmn.info;
             mm_send_mmr_service_ind( &mmr_service_ind );
           }
         }
         else if(mm_reg_req_revd_active_call == TRUE)
         {
           mm_substate_control( MM_STD_LIMITED_SERVICE );

           /* Set the Serving PLMN service status to Limited */
           mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                           mm_serving_plmn.info.reg_service_domain,
                                           TRUE,
                                           SYS_SRV_STATUS_LIMITED,
                                           FALSE ); 

           gmm_set_substate( GMM_LIMITED_SERVICE );
           mm_reg_req_revd_active_call = FALSE;
           mmr_service_ind.service_state = mm_serving_plmn.info;
           mmr_service_ind.cause = MMR_CAUSE_PLMN_SEL_CALL_END;
           MSG_HIGH_DS_0( MM_SUB, "=MM= Sending MMR_SERVICE_IND" );
           mm_send_mmr_service_ind( &mmr_service_ind );
         }
         else
         if (mm_reg_waiting_for_reg_cnf && mm_ready_to_send_reg_cnf)
         {
            if (!mm_rat_change_pending && 
                !mm_serv_change_pending && 
                ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY) ||
                 ((mm_serving_plmn.network_op_mode !=  NETWORK_OP_MODE_2)
#ifdef FEATURE_CLASS_B_OPERATION_IN_NMO3
                &&(mm_serving_plmn.network_op_mode !=  NETWORK_OP_MODE_3)
#endif
                 )
                ))
            {
               mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
               mm_send_mmr_reg_cnf();
               mm_reg_waiting_for_reg_cnf = FALSE;
               mm_ready_to_send_reg_cnf   = FALSE;
            }
         }
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
         if((next_state == MM_RR_CONNECTION_RELEASE_NOT_ALLOWED) && (mm_cnm_rel_req_received))
         {
            mm_send_mmcnm_rel_cnf();
         }
#endif
#ifdef FEATURE_NAS_ECALL
         if ((mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY)&&
             (mm_state == MM_IDLE))
         {
            if (mm_need_to_perform_ecall_inactivity)
            {
               mm_perform_ecall_inactivity();
            }
            else
            {
               if (mm_call_est_cause == EMERGENCY_CALL_ESTABLISHMENT)
               {
                 mm_start_timer(TIMER_T3242,DEFAULT_TIMEOUT);
               }
               else if (mm_call_est_cause == MO_CALL_ESTABLISHMENT)
               {
                 mm_start_timer(TIMER_T3243,DEFAULT_TIMEOUT);
               }
   
               mm_call_est_cause = NO_CNM_SERVICE;
            }
         }
#endif        
        /*
         * If substate is normal service or attemptingto update,
         * timer T3211 or 3212 must be started
         * (see 4.4.4.8)
         */
         if ((mm_idle_substate == MM_NORMAL_SERVICE) ||
             (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE))
         {
            switch (mm_idle_transition_timer)
            {
               case TIMER_T3211:
                  /** --------------------------------------------------------
                   ** Start timer T3211. If the timer was already started and
                   ** paused earlier, resume it now.
                   ** -------------------------------------------------------- */
                  if ( mm_timer_status [ TIMER_T3211] == TIMER_PAUSED )
                  {
                     if (gs_modify_timer_ext( get_mm_timer_queue(),
                                              TIMER_T3211,
                                              TIMER_MODIFY_RESUME,
                                              TRUE) != GS_SUCCESS)
                     {
                        MSG_ERROR_DS( MM_SUB, "=MM= Failed to resume timer %d", TIMER_T3211, 0, 0);
                     }
                     else
                     {
                        MSG_HIGH_DS_1(MM_SUB ,"=MM= Resumed timer %d", TIMER_T3211);
                        mm_timer_status[ TIMER_T3211 ] = TIMER_ACTIVE;
                     }
                  }
                  else
                  {
                     if( mm_timer_status [ TIMER_T3211] == TIMER_STOPPED )
                     {
                       mm_start_timer( TIMER_T3211, DEFAULT_TIMEOUT );
                     }

                     if((mm_idle_substate == MM_NORMAL_SERVICE) ||
                        (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE))
                     {
#ifdef FEATURE_NAS_REL10
                        if(mm_is_per_ms_t3212_present)
                        {
                          if((mm_stored_per_ms_t3212_value != 0) &&
                             (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                          {
                             mm_start_timer( TIMER_T3212,
                                                        mm_stored_per_ms_t3212_value);
                          }
                        }
                        else
#endif
                        if ((mm_system_information.T3212_timer_value != 0 ) &&
                            (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                        {
                            mm_start_timer( TIMER_T3212,
                                                       mm_system_information.T3212_timer_value );
                        }
                    }
                  }
                  break;

#ifdef FEATURE_DUAL_SIM
               case TIMER_MM_RADIO_RETRY:
                  if(mm_lu_reject_info.mm_conn_rej_timer_value)
                  {
                    mm_start_timer( TIMER_MM_RADIO_RETRY, mm_lu_reject_info.mm_conn_rej_timer_value*1000);
                    mm_lu_reject_info.mm_conn_rej_timer_value = 0;
                  }
                  else
                  {
                    if ( mm_timer_status [ TIMER_MM_RADIO_RETRY] == TIMER_PAUSED )
                    {
                       if (gs_modify_timer(get_mm_timer_queue(),
                                         TIMER_MM_RADIO_RETRY,
                                         TIMER_MODIFY_RESUME) != GS_SUCCESS)
                       {
                         MSG_ERROR_DS( MM_SUB, "=MM= Failed to resume timer %d", TIMER_MM_RADIO_RETRY, 0, 0);
                       }
                       else
                       {
                        MSG_HIGH_DS_1(MM_SUB ,"=MM= Resumed timer %d", TIMER_MM_RADIO_RETRY);
                        mm_timer_status[ TIMER_MM_RADIO_RETRY ] = TIMER_ACTIVE;
                       }
                    }
                    else
                    {
                      mm_start_timer( TIMER_MM_RADIO_RETRY, DEFAULT_TIMEOUT );
                    }
                  }
                  break;
#endif

               case NULL:     /*FALLTHROUGH*/
               case TIMER_T3212:

#ifdef FEATURE_NAS_REL10
               if(mm_is_per_ms_t3212_present)
               {
                  if((mm_stored_per_ms_t3212_value != 0) &&
                     (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                  {
                      mm_start_timer( TIMER_T3212,
                                                   mm_stored_per_ms_t3212_value);
                  }
               }
               else
#endif
               if ((mm_system_information.T3212_timer_value != 0 ) &&
                   (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
               {
                   /* ------------------------------------------------------
                   ** If the (T3212) timer is not already started, the timer
                   ** is started each time the mobile enters the MM_IDLE
                   ** substate NORMAL_SERVICE or ATTEMPTING_TO_UPDATE
                   ** (24.008 section 4.2).
                   ** ------------------------------------------------------ */

                    mm_start_timer( TIMER_T3212,
                                    mm_system_information.T3212_timer_value );

               }
                  break;

               default:
                  MSG_ERROR_DS( MM_SUB, "=MM= Unexpected timer id %d", mm_idle_transition_timer,0,0);
                  break;
            }
         }
      }

      /*
       * Handle special case where we return to MM_IDLE when there is
       * a MMR_STOP_MODE_REQ outstanding
       */

	  if ((mm_state == MM_IDLE) && (mm_nreg_req_received || (mm_stop_mode_req_pended && (mm_timer_status[TIMER_MM_REL] == TIMER_STOPPED))) 
			  && ((gmm_state == GMM_NULL) || (gmm_state == GMM_DEREGISTERED)) 
			  && (mm_rat_handler_status != MM_RAT_STOP_TDSCDMA_IN_PROGRESS) && (mm_rat_handler_status != MM_RAT_STOP_WCDMA_IN_PROGRESS) &&
                 (mm_rat_handler_status != MM_RAT_STOP_GSM_IN_PROGRESS))

      {
        if ((mmcoord_pending_reg_message.cmd.hdr.message_set == (int) MS_MM_REG) &&
            (mmcoord_pending_reg_message.cmd.hdr.message_id == (int) MMR_STOP_MODE_REQ))
        {
          /* ---------------------------------------------------------------------
          ** Set ATT flag to FALSE so we don't attempt to perform multiple DETACHs
          ** --------------------------------------------------------------------- */
          mm_stop_mode_req_pended = FALSE;
          mm_system_information.ATT = FALSE;          
          mm_handle_message( &mmcoord_pending_reg_message );
        }

      }
      if ((mm_state == MM_IDLE) && 
          (is_gsm_mode() || is_umts_mode()))
      {
    
#ifdef FEATURE_TDSCDMA
        mm_resume_policyman();
#endif   
        if(mm_hard_abort_pending)
        {
          if((mm_rat_handler_status == MM_RAT_STOP_TDSCDMA_IN_PROGRESS) || (mm_rat_handler_status == MM_RAT_STOP_WCDMA_IN_PROGRESS) ||
             (mm_rat_handler_status == MM_RAT_STOP_GSM_IN_PROGRESS))
          {
            MSG_HIGH_DS_0(MM_SUB,"=MM= Alt Scan pended due to IRAT in progress "); 
          }
          else
          {
            mm_hard_abort_pending = FALSE;
            mmcoord_handle_deep_sleep_alt_scan();
          }
        }
#ifdef FEATURE_DUAL_SIM
        else if(mm_abort_dual_switch_pending == SYS_STOP_MODE_REASON_DSDS_SUSPEND)
        {
          if((mm_rat_handler_status == MM_RAT_STOP_TDSCDMA_IN_PROGRESS) || (mm_rat_handler_status == MM_RAT_STOP_WCDMA_IN_PROGRESS) ||
             (mm_rat_handler_status == MM_RAT_STOP_GSM_IN_PROGRESS))
          {
            MSG_HIGH_DS_0(MM_SUB,"=MM= DSDS Suspend pended due to IRAT in progress "); 
          }
          else
          {
            mm_abort_dual_switch_pending = SYS_STOP_MODE_REASON_NONE;
            mm_stop_timer(TIMER_MM_REL);
            MSG_HIGH_DS_0(MM_SUB, "=MM= DSDS Suspend. Pause all running timers (except periodic timers)");           
            mmcoord_handle_deep_sleep_alt_scan();
          }
        }
        else if(mm_abort_dual_switch_pending == SYS_STOP_MODE_REASON_DUAL_SWITCH)
        {
          if((mm_rat_handler_status == MM_RAT_STOP_TDSCDMA_IN_PROGRESS) || (mm_rat_handler_status == MM_RAT_STOP_WCDMA_IN_PROGRESS) ||
             (mm_rat_handler_status == MM_RAT_STOP_GSM_IN_PROGRESS))
          {
            MSG_HIGH_DS_0(MM_SUB,"=MM= DUAL SWITCH pended due to IRAT in progress "); 
          }
          else
          {
            mm_abort_dual_switch_pending = SYS_STOP_MODE_REASON_NONE;
            mm_stop_timer(TIMER_MM_REL);
            mmcoord_handle_deep_sleep_dual_switch();
          }
        }
        else if(mm_abort_dual_switch_pending == SYS_STOP_MODE_REASON_ALT_SCAN)
        {
          if((mm_rat_handler_status == MM_RAT_STOP_TDSCDMA_IN_PROGRESS) || (mm_rat_handler_status == MM_RAT_STOP_WCDMA_IN_PROGRESS))
          {
            MSG_HIGH_DS_0(MM_SUB,"=MM= ALT SCAN pended due to IRAT in progress "); 
          }
          else
          {
            mm_abort_dual_switch_pending = SYS_STOP_MODE_REASON_NONE;
            mm_stop_timer(TIMER_MM_REL);
            mmcoord_handle_deep_sleep_alt_scan();
          }
        }
#endif

#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE
        else if(mm_hard_abort_ue_sglte_mode_switch_pending)
        {
          mm_hard_abort_ue_sglte_mode_switch_pending = FALSE;
          mmcoord_handle_ue_sglte_mode_switch();
        }
#endif
#ifdef FEATURE_LTE
        else if(mm_stop_mode_local_detach_peding)
        {
          mmcoord_process_mmr_stop_mode_req(&mmcoord_pending_reg_message);
        }
#endif
        else if ((mm_psm_ready_req_rejected == TRUE) &&
                 (mm_check_ready_for_psm() == SYS_PSM_STATUS_READY))
        {
          mm_send_mmr_psm_ready_ind();
        }
        else if ((( mm_idle_substate == MM_NORMAL_SERVICE ) || ( mm_idle_substate == MM_ATTEMPTING_TO_UPDATE )) &&
                  (!mm_lu_pended_nmo_unknown) &&
#ifdef FEATURE_NAS_REL10
                  ((mm_timer_status[TIMER_T3246] == TIMER_STOPPED)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                      ||(mm_is_rrc_access_class_11_to_15() == TRUE)
#endif
                   ) &&
#endif
                  (mm_event != MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE) &&    
                  (mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE))
        {
           switch (mm_pending_lu_request)
           {
              case NORMAL_LU:
                 mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                 break;
  
              case T3211_EXPIRED:
              case T3213_EXPIRED:
                 /* --------------------------------------
                 ** Do location update with type last used
                 ** -------------------------------------- */
                 mm_initiate_lu( mm_get_lu_cause() );
                 break;
  
              case T3212_EXPIRED:
              case T3312_EXPIRED:
                 mm_initiate_lu( MM_LU_CAUSE_PERIODIC );
                 break;
  
              case IMSI_ATTACH:
                 mm_initiate_lu( MM_LU_CAUSE_IMSI_ATTACH );
                 break;
  
              case NO_LU:
                 if ( (mm_initiate_lu_upon_entry_into_new_cell == TRUE &&
                      (mm_serving_cell_valid) && is_umts_mode() &&
                       memcmp(&mm_lu_failed_cell_id,&mm_serving_cell_id,sizeof(uint32)) != 0))
                 {
                   mm_initiate_lu( mm_get_lu_cause() );
                 }
                 break;
  
              case LTE_IRAT_LU:
                 if ((memcmp( mm_serving_lai, mm_previous_serving_lai,  LAI_SIZE ) == 0 ) &&
                     (((mm_lu_attempt_counter >= 4)&&
                       (mm_timer_status[TIMER_T3212] == TIMER_ACTIVE))||
                      (mm_timer_status[TIMER_T3213] == TIMER_ACTIVE)||
                      (mm_timer_status[TIMER_T3211] == TIMER_ACTIVE)))
                 {
                   MSG_HIGH_DS_0(MM_SUB,"=MM= Do not initiate pended LU on attempted LAI ");
                 }
                 else
                 {
                   MSG_HIGH_DS_0(MM_SUB,"=MM= LU started due to IRAT from LTE");
                   mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                 }
                 break;
  
              default:
                 MSG_ERROR_DS( MM_SUB, "=MM= Invalid mm_pending_lu_request %d", mm_pending_lu_request, 0,0 );
                 break;
           }
        }
        else if((mm_lu_pended_nmo_unknown) &&
                (mm_pending_lu_request != NO_LU))
        {
          MSG_HIGH_DS_0(MM_SUB,"=MM= NMO UNKNOWN: MM_IDLE- Process LU after NMO is available ");
        }
      }
#ifdef FEATURE_LTE
      //Start LU which was pended due to CSFB call
      if(mm_state == MM_IDLE && 
         (is_gsm_mode() || is_umts_mode()
#ifdef FEATURE_WRLF_SYSTEM_SEL
          ||(mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
#endif
         ) &&
#ifdef FEATURE_NAS_REL10
        ((mm_timer_status[TIMER_T3246] == TIMER_STOPPED)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
            || (mm_is_rrc_access_class_11_to_15() == TRUE)
#endif
        ) &&
#endif 
         mm_idle_substate == MM_NORMAL_SERVICE &&
         mm_csfb_state == MM_CSFB_STATE_LU_PENDING && 
         !mm_lu_pended_nmo_unknown)                                      
      {
        mm_csfb_set_state(MM_CSFB_STATE_NULL);
        if((mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE)&&
           (mm_event != MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE))
        {
          mm_initiate_lu( MM_LU_CAUSE_NORMAL );       
        }
        else
        {
          mm_pending_lu_request = LTE_IRAT_LU;
        }
      }
#endif 
   }
   else
   {
      MSG_ERROR_DS( MM_SUB, "=MM= Unexpected event %d in state %d", mm_event, mm_state, 0);
   }
}


/*===========================================================================

FUNCTION MM_CHECK_RA_RETRY

DESCRIPTION
  This function deals with random access failure occurring during a
  location update procedure. The first time this happens, timer
  T3212 is started. When that matures, another LU attempt is made.
  If that attempt fails for the same reason, then the standard
  LU fail action is performed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_check_ra_retry( void )
{

   if ( mm_T3213_timed_out )  /* if second timeout for this LU attempt */
   {
      mm_check_lu_fail_action();
   }
   else                    /* first timeout */
   {
      mm_start_timer( TIMER_T3213, DEFAULT_TIMEOUT );
   }
}
