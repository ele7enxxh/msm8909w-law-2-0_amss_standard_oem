/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmregister.c_v   1.7   03 Jul 2002 15:13:06   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmregister.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/01   jca     Added processing for RRC_ACT_CNF primitive.
06/08/01   jca     Added processing for RRC_DEACT_CNF primitive.
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
06/05/02   ks      Added support for Dual mode operation.
07/01/02   jca     Add processing for RR_STOP_GSM_MODE_CNF.
07/03/02   jca     Deleted timers TIMER_PLMN_SELECT and TIMER_PLMN_LIST.
07/22/02   ks      Added processing of dynamic RAT change procedure and
                   handling of RR_STOP_GSM_MODE_CNF & RRC_STOP_WCMDA_MODE_CNF
09/23/02   mks     Added processing for MMR_ACT_REQ from REG
11/12/02   jca     Modified prototype for function mm_abort_rr().
11/19/02   mks     Added support for deactivating the active RAT before
                   forwarding the PLMN search request to inactive RAT.
06/04/03   ks      Added support for RRC_MODE_CHANGE_CNF for MODE_CHANGE
                   support for UMTS.
10/14/03   cd      Added check for GPRS detach pending upon expiration of T3220
01/28/04   jca     Update mm_previous_serving_lai and mm_previous_idle_substate
                   when a MMR_REG_REQ is received.
08/09/04   ks      Now aborting the RR connection after the Timer T3220 expires
                   before MM moves back to IDLE state.
08/20/04   jca     After aborting RR connection on T3220 expiration, wait for
                   ABORT_CNF before initiating GPRS Detach procedure.
11/16/04   sv      Handling for the new primitive RRC_SERVICE_IND_FROM_OTA_MSG
02/24/05   sm      Added support for sending RAT change events
12/14/05   abe      Supported changes for FEATURE_GSM_DTM
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity 
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
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"
#include "reg_mm_v.h"
#include "mm_rr.h"
#include "mm_v.h"
#include "rrcmmif.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "err.h"
#include "rrccmd.h"
#include "mm_umts_v.h"
#include "gmm_int_v.h"
#include "mm_coord.h"
#include "mm_sim.h"


#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
static boolean mm_send_rat_change_event_sim[MAX_NAS_STACKS] = { FALSE, FALSE, FALSE } ;
#define  mm_send_rat_change_event mm_send_rat_change_event_sim[mm_as_id]

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

static boolean mm_send_rat_change_event_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;
#define  mm_send_rat_change_event mm_send_rat_change_event_sim[mm_as_id]

#else

static boolean mm_send_rat_change_event = FALSE;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/


/*===========================================================================

FUNCTION MM_HANDLE_REGISTRATION

DESCRIPTION
  This function routes message relating to registration to the function
  dealing with it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_handle_registration(mm_cmd_type* msg_p)
{
   switch ( msg_p->cmd.hdr.message_set )
   {
      case MS_MM_REG:
         switch ( msg_p->cmd.hdr.message_id )
         {
            case MMR_STOP_MODE_REQ:
            case MMR_PH_STATUS_CHANGE_REQ:
               mm_handle_deactivation(msg_p);
               break;

            case MMR_REG_REQ:


               if (mm_rat_change_pending || mm_serv_change_pending)
               {
                 /* Handle RAT change */
                 mm_handle_rat_change(msg_p);
               }
               else
               {
                 mm_select_plmn(msg_p);
               }
               break;

            case MMR_PLMN_SEARCH_REQ:
               /* -----------------------------------------------------------
               **  If REG requests PLMN search to be performed on a currently
               **  inactive RAT then MM needs to stop the active RAT before
               **  forwarding the PLMN search request to the inactive RAT.
               **  Please note that sending PLMN search request to an inactive
               **  RAT triggers its activation.
               ** ----------------------------------------------------------- */
               if (mm_rat_change_pending)
               {
                 mm_handle_rat_change(msg_p);
               }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
               else
               {
                 mm_get_plmn_list(msg_p);
               }
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
               break;

#ifdef FEATURE_NAS_ECALL
            case MMR_ECALL_MODE_IND:
               mm_perform_ecall_inactivity();
               break;
#endif
            case MMR_SIM_STATE_UPDATE_IND:
               mm_update_sim_state(SYS_SIM_STATE_AVAILABLE);
               break;

            default:
               MSG_ERROR_DS(MM_SUB, "=MM= MM: REG message id %d unexpected", msg_p->cmd.hdr.message_id, 0,0);
               break;
         }
         break;

      case MS_MM_RR:
      case MS_MM_TDSRRC:
         switch ( msg_p->cmd.hdr.message_id )
         {
            case RR_DEACT_CNF:
            case RRC_DEACT_CNF:
            case RRC_MODE_CHANGE_CNF:
               mm_handle_deactivation(msg_p);
               break;

            case RR_STOP_GSM_MODE_CNF:
            case RRC_STOP_WCDMA_MODE_CNF:
              mm_handle_rat_change(msg_p);
              mm_send_rat_change_event = TRUE;
              break;

            case RR_PLMN_SELECT_CNF:
            case RRC_SERVICE_CNF:
               mm_select_plmn(msg_p);
#ifndef FEATURE_LTE
               if (mm_send_rat_change_event == TRUE)
               {
                  mmsend_rat_change_event();
                  mm_send_rat_change_event = FALSE;
               }
#else
               mmsend_rat_change_event(); // If LTE is defined, then call this function directly 
#endif
               break;
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
            case RR_PLMN_LIST_CNF:
            case RRC_PLMN_LIST_CNF:
               mm_get_plmn_list(msg_p);
               break;
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
            case RRC_SERVICE_IND:
            case RRC_SERVICE_IND_FROM_OTA_MSG:
            case RR_SERVICE_IND:
#ifdef FEATURE_GSM_DTM
            case RR_SERVICE_IND_FROM_OTA_MSG:
#endif
            case MM_DATA_IND:
            case RR_DATA_IND:
            /* assume only relevant messages get this far */

               mm_handle_location_updating(msg_p);
               break;

            default:
               MSG_ERROR_DS(MM_SUB, "=MM= MM: REG message id %d unexpected", msg_p->cmd.hdr.message_id,0,0);
               break;

         }
         break;

      case MS_TIMER:
         switch (msg_p->cmd.timer_expiry.timer_id)
         {
            case TIMER_T3220:
               /* Only close session or abort connection if there is no
                * pending GPRS detach
                */

               if (!gmm_gprs_detach_pending)
               {
                  if (is_umts_mode())
                  {
                     rrc_cmd_type *rrc_cmd_p = NULL;

#ifdef FEATURE_WCDMA
                     rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
#endif /* FEATURE_WCDMA */

                     if (rrc_cmd_p == NULL)
                     {
                        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                     }

                     rrc_cmd_p->cmd_hdr.cmd_id = RRC_CLOSE_SESSION_REQ;
                     rrc_cmd_p->cmd.close_sess_req.cn_domain_id = RRC_CS_DOMAIN_CN_ID;
                     rrc_cmd_p->cmd.close_sess_req.close_session_cause = RRC_OTHERS;
                     send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );

                     mm_state_control( MM_STD_RR_CONNECTION_ABORTED );
                  }
                  else /* GSM mode */
                  {
                     mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );

                     mm_state_control( MM_STD_RR_CONNECTION_ABORTED );
                  }
               }
               else
               {
                 mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );
                 mm_state_control( MM_STD_RR_CONNECTION_ABORTED );
               }

               break;

            case TIMER_T3210:
            case TIMER_T3211:
            case TIMER_T3212:
            case TIMER_T3213:
            case TIMER_MM_FORBIDDEN_LAI_CLEAR:
#ifdef FEATURE_DUAL_SIM
            case TIMER_MM_RADIO_RETRY:
#endif
#ifdef FEATURE_ENHANCED_NW_SELECTION
            case TIMER_FOREGROUND_SEARCH:
            case TIMER_FORCE_OOS:
#endif
#ifdef FEATURE_NAS_REL10
            case TIMER_T3246:
#endif
            case TIMER_CSMT_GUARD:
               mm_handle_location_updating(msg_p);
               break;
#ifdef FEATURE_NAS_ECALL
            case TIMER_T3242:
            case TIMER_T3243:
              mm_perform_ecall_inactivity();
              mmsend_timer_expiry_event(msg_p->cmd.timer_expiry.timer_id);
              break;
#endif
            default:
               MSG_ERROR_DS(MM_SUB, "=MM= MM: Timer message %d unexpected",
                  msg_p->cmd.timer_expiry.timer_id, 0,0);
               break;
         }
         break;

      default:
         MSG_ERROR_DS(MM_SUB, "=MM= MM: Message set %d unexpected",  msg_p->cmd.hdr.message_set, 0,0);
         break;
   }
}
