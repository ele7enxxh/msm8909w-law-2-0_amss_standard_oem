/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmconn.c_v   1.29   17 Jul 2002 12:30:00   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmconn.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/13   rajesh  Made code changes to perform LU when there is a change of LA even for emergency
                   CSFB call when NV item "mm_lai_change_force_lau_for_emergency" is set.
10/15/11   rajesh   Support to set CSMO bit in CM serv request when LAI is different
09/02/11   rajesh  Added code to set "is_periodic_lau" if rr_rest_req is for doing periodic LAU.
07/05/11  mrk      Support of Ciphering enabled Indication for CS Voice calls
05/14/01   jca     Combine establishment_cause in call to send_l3_message_to_rrc().
05/15/01   sbk     RRC_SYNC_IND hadling added
06/03/01   sbk     Global variables are renamed to use mm_ prefix
06/08/01   jca     RRC_OPEN_SESSION failure now is being handled.
06/15/01   sbk     Rearraged the CNM_SERVICE_REJECT processing
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
08/22/01   jca     Removed some errant CNM renames.
09/18/01   sbk     Intertask primitives are logged and QXDM logging is supported
11/06/01   jca     Removed call to set T3230 in incorrect MM state.
02/12/02   jca     Fixed a number of bugs in CNM-MM processing.
02/20/02   sbk     Route the LOOPBACK sync message to TC handler
02/15/02   jca     An indication from RRC that integrity protection is enabled
                   now treated as service acceptance indication (as per spec).
04/23/02   jca     Modifications to reflect changes to MM-CNM interface.
05/02/02   vdr     Now the CKSN is set correctly according to operation mode
                   and current domain id
05/28/02   jca     Now reject MMCNM_EST_REQ in state MM_WAIT_FOR_NETWORK_COMMAND.
06/05/02   ks      Added support for Dual mode operation.
06/26/02   ks      Added support for Sequence Numbering in NAS messages.
07/01/02   jca     Added processing for RR_ABORT_CNF.
07/12/02   ks      Corrected the Featurization for global_flag_cpdata_arriving
                   to support the Dual mode builds.
07/17/02   ks      Replaced read_coded_classmark_2() function calls with
                   gs_read_coded_classmark_2() that supports dual mode/band.
07/29/02   jca     Now transition MM state when RR_PAGING_IND received.  Also
                   removed errant call to start timer T3230.
08/09/02   jca     Now checking mm_idle_substate before clearing
                   LU attempt counter (as per spec).
09/04/02   vdr     Fixed determination of the CKSN
10/07/02   vdr     Changed error handling during CKSN determination
10/08/02   cd      Added mapping of RR/RRC sync reason in the mmcc_sync_ind
10/08/02   cd      Modified call to mm_send_mmcnm_est_rej to include passing
                   the reject cause.
                   Removed lint errors
                   Modified processing of T3230 expiration.  The mmcnm_est_rej is
                   sent by the state machine action 19
10/30/02   mks     Now storing the service establishment cause while processing
                   MMCNM_EST_REQ
11/08/02   cd      Added information to msg high
                   in mm_connection_establishment(), added checking the cell access
                   system information when a CNM establish request is received
11/12/02   jca     Added support for RRC_CLOSE_SESSION_CNF.
11/12/02   vdr     Added support for the PS connection establishment
11/15/02   mks     Pausing the timer T3211 (if it is running) before initiating
                   an RR connection.
11/18/02   ks      Added processing for MM_RR_WCDMA_GSM_HO in RR_SYNC_IND.
12/04/02   ab      Remapped the MM_RR_HANDOVER (inter GSM handover) reason to
                    GSM_CHANNEL_MODE_MODIFIED for CNM.
12/04/02   vdr     Fixed MM connection establishment
12/09/02   cd      Added more sync reasons to the mapping of RRC/RR_SYNC_IND
                   to MMCC_SYNC_IND reasons.  Removed lint indentation errors
12/12/02   ks      Replaced mm_sim_valid with function mmsim_sim_valid.
12/17/02   mks     Added a check to skip processing of pages if SIM is
                   invalidated for CS domain.
01/10/03   vdr     o Changes of CKSN determination
                   o Changed routing conditions for the MMCNM_EST_REQ primitive
01/10/03   mks     global_flag_cpdata_arriving variable is made an extern variable
                   to resolve link error.
01/16/03   vdr     Added GSM CS connection management in order to support dual mode features
01/30/02   jca     Now setting mm_integrity_protected to FALSE on receipt of
                   RRC_CLOSE_SESSION_IND or RRC_CLOSE_SESSION_CNF.
02/12/02   cd      Added cause info to MSG_HIGH for RR_SYNC_IND
02/21/03   vdr     Removed GSM CS connection management & added new criteria for routing
                   CNMMM_EST_REQ either to MM or GMM in order to support dual mode features
03/06/03   jca     Modified RRC_PAGE_IND and RR_PAGING_IND processign as per
                   24.008 V3.12.0 (2002-06) and spec CR 372.
03/07/03   mks     Corrected the handling of CM_SERVICE_REJECT with reject cause #6.
                   For compliance with TS24.008 spec CR #420, MS should consider the
                   SIM as invalid for non-GPRS services only. GPRS services & GMM
                   context should not be impacted by MM procedures (Except
                   authentication).
04/07/03   cd      Added call to start timer T3210 during a LU procedure in GSM
                   and msg to report that a LU_REQ has been sent
04/18/03   cd      Starting T3210 only for a establishment cause set to
                   MM_RR_LOCATION_UPDATE
04/30/03   jca     Perform a Combined RAU with IMSI Attach if a CM Service Reject
                   message with cause value #4 is received and mobile is operating
                   in CS/PS mode in network operation mode 1 (as per 24.008 CR 477).
05/08/03   cd      Corrected MM state transtion when receiving CM_SERVICE_REQ
                   for an emergency call in the MM_CONNECTION_ACTIVE state
07/18/03   cd      Added check to reject normal call while in limited service
                   Re-aligned code block
09/23/03   vdr     Changed routing scheme for MMCNM_EST_REQ primitive
10/08/03   cd      Now sending a CLOSE_SESSION_REQ instead of an ABORT REQ when
                   T3240 expires.
11/25/03   jca     Set rrc_conn_est_in_progress to TRUE when RR_EST_REQ sent.
03/10/04   cd      Reset the flag mm_cnm_is_active to TRUE upon request to
                   reestablish a connection
03/26/04   cd      When processing a CM_SERVICE_REJ, added check for the mm_state
                   before sending a mmcnm_est_rej
04/06/04   cd      LCS support
05/13/04   cd      Enhancements to CS FOR. Now setting the mm_follow_on_request flag
                   to true upon receiving a CNM request, only if it has not been
                   already set in a mmr_req_req.
                   Fixed Lint errors.
06/08/04   cd      LCS support
06/18/04   jca     Added variable gmm_pending_ra_update_type.
07/19/40   cd      Setting the mm_follow_on_request and mm_held_cnm_service_req.present
                   to false after a CM_SERVICE_REQ msg is sent in a EST_REQ
08/02/04   cd      Corrected processing of CM_SERVICE_REJ in state 5, 17, 20
                   based on reject cause
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
08/24/04   ab      CR40898, Added logging for OTA message that is transported
                     in rr_est_req primitives.
10/05/04   jca     Set mm_held_cnm_service_req.present to TRUE when a MMCNM_EST_REQ
                   is received when RAU in-progress and network op mode = NMO1.
10/06/04   jca     Reject MMCNM_EST_REQ (normal call) when MM substate set to NO_IMSI.
11/16/04   jca     Reject MMCNM_EST_REQ (emergency call) in state MM_WAIT_FOR_NETWORK_COMMAND.
12/23/04   jca     Now calling mm_connection_release() on receipt of RR_CLOSE_SESSION_CNF.
03/31/05   hj      Added support for sending CM_CIPHER_IND after receiving RR_SYNC_IND with
                   MM_RR_CIPHERING as the sync cause.
04/12/05   hj      Changed call to mm_send_cm_cipher_ind to reflect new
                   enum type for cipher_domain.
04/26/05   hj      Ignoring Page if the current LAI is forbidden for national roaming for
                   GCF Test case.
05/20/05   jca     Set emergency call request to pending status if received
                   while a Location Update is in-progress.
05/26/05   vdr     Modified routing criteria of the MMCNM_EST_REQ either to MM or GMM
05/30/05   abe     setting mm_sms_is_active to TRUE once it is decided to use SMS on PS domain.
                   Pass MMCNM_REL_REQ to GMM for handling if mm_sms_is_active set to TRUE.
06/06/05   abe     Added code for CCBS support
07/22/05   hj      Ignoring RR_PAGE_IND if the current LAI is forbidden for national 
                   roaming for GCF Test case.
09/16/05   vdr     Added mm_held_cnm_service_req activation for every Emerjency call establishment
09/19/05   hs      Treat HO as implicit CM service accept in state 20
10/03/05   hs      Modification to RR_SYNC_IND/RRC_SYNC_IND and T3230 handling
10/06/05   ajt     Upon receiving CM_SERVICE_ACCEPT or CM_SERVICE_REJECT, invalidate 
                   authentication cache
11/11/05   hj      Now not responding to CS page if UE is not registered for CS domain,
                   for GCF test cases.                   
12/05/05   hj      Setting mm_idle_transition_timer to NULL after stopping T3211.
01/30/06   hj      Added support for CS_ONLY and PS_ONLY routing options for SMS.
06/14/06   up      Called new mm_update_mm_serving_plmn_info for GSTK event.
06/15/06   nr      Passing proper cause in mm_send_mmcc_reest_rej.
07/14/06   pk     Updated for Feature DSAC
07/14/06   nr      removing redundant variables global_flag_cpdata_arriving & global_flag_ccrelease_arriving
09/21/06   pk     Updated for FEATURE_GSM_EDTM
11/27/06   vsm   Adding code to remember the LU procedure pre-emted by Emergency Call.
03/05/06   NR      Adding changes for WB_AMR
03/26/07   NR      Rejecting SMS during emergency call with integrity disabled
04/09/07   rd      SMS optimization changes are incorporated
12/13/07   jbk     Optimising FEATURE_SMS_ENHANCED_DOMAIN_SELECTION while non-preferred domains are active
11/10/11   pm     When access class is barred for all calls, allow only emergency call request to go to RRC
                   so that RRC can camp on a different cell where emergency calls can be placed.
11/11/11   gps    Initiated RRC establish reqquest with all zeros.
04/24/12   sla    send_sequence_number_mm_cc_ss is reset to zero when RR_EST_IND is received
===========================================================================*/


/*=========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mm_mm_v.h"
#include "mm_rr.h"
#include "mmrat.h"
#include "mm_v.h"
#include "rrcmmif.h"
#include "rrccmd.h"
#include "mm_sim.h"
#include "mm_umts_v.h"
#include "mm_coord.h"
#include "libprot.h"
#include "cause.h"
#include "err.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#include "mmauth.h"
#include "gmm_int_v.h"
#include "naslog_v.h"
#include "mm_iei.h"
#include "sys_eplmn_list.h"

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
#include "mmrat.h"  /* needed to call mm_send_rr_stop_mode_req() for deepsleep */
#endif

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
#include "subs_prio.h"
#endif

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK

extern void gs_read_coded_classmark_2_per_nas_stacks(
  classmark_2_ie_T  *classmark_2_ptr,
  classmark_params_T   *params_ptr,
  sys_modem_as_id_e_type stack_id_nas
);



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
byte mm_last_paging_cause_sim[MAX_NAS_STACKS];
#define   mm_last_paging_cause mm_last_paging_cause_sim[mm_as_id]
#else
byte mm_last_paging_cause;
#endif /* FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/
/* -------------------
** Function prototypes
** ------------------- */
void mm_notify_mt_rr_connection_establishment( mm_cmd_type * );
void mm_connection_establishment( mm_cmd_type *, auth_security_context_id );
void mm_flag_reestablishment_requested( mm_cmd_type * );

#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
static int8 mm_waiting_for_regs_complete_txn_sim[MAX_NAS_STACKS] = {0,0,0};
#define     mm_waiting_for_regs_complete_txn mm_waiting_for_regs_complete_txn_sim[mm_as_id]
#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
static int8 mm_waiting_for_regs_complete_txn_sim[MAX_NAS_STACKS] = {0,0};
#define     mm_waiting_for_regs_complete_txn mm_waiting_for_regs_complete_txn_sim[mm_as_id]
#else
static int8 mm_waiting_for_regs_complete_txn = 0;
#endif /* FEATURE_TRIPLE_SIM*/

extern boolean full_srv_req_in_emerg;
extern boolean emergency_call_after_lu_in_lpm;

void mm_clear_regs_complete_counter(void) 
{
  mm_waiting_for_regs_complete_txn = 0;
}

#ifdef FEATURE_LTE
extern boolean emm_bit_mask_check
(
  uint32 value,
  byte bit_mask
);
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
extern boolean sys_eplmn_per_nas_stacks_list_equivalent_plmn
(
  sys_plmn_id_s_type plmn,
  sys_modem_as_id_e_type      sub_id,
  sys_modem_as_id_e_type      stack_id_nas
);
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,(sys_modem_as_id_e_type)mm_sub_id,\
                                                                                            (sys_modem_as_id_e_type)mm_as_id)
#else
extern boolean sys_eplmn_list_equivalent_plmn
(
  sys_plmn_id_s_type plmn
);
#endif
extern boolean mm_check_forbidden_regional_roaming_nas_lai(void);
/*===========================================================================

FUNCTION MM_GET_CURRENT_SECURITY_CONTEXT

DESCRIPTION
  This function calculates current security context ID according to the
  CN domain ID that the MS is currently belonging to.


DEPENDENCIES
  None

RETURN VALUE
  auth_security_context_id value

SIDE EFFECTS
  None

===========================================================================*/
auth_security_context_id mm_get_current_security_context
(
  rrc_cn_domain_identity_e_type  cn_domain_identity
)
{
  auth_security_context_id security_context = AUTH_GSM_SECURITY_CONTEXT ;
  switch (cn_domain_identity)
  {
    case RRC_CS_DOMAIN_CN_ID :
      if (is_gsm_mode())
      {
        security_context = AUTH_GSM_SECURITY_CONTEXT ;
      }
      else
      {
        if (is_umts_mode())
        {
          security_context = AUTH_CS_GPRS_SECURITY_CONTEXT ;
        }
        else
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unknown MS operation mode",0,0,0) ;
        }
      }
      break ;
    case RRC_PS_DOMAIN_CN_ID :
      if (is_gsm_mode())
      {
        security_context = AUTH_GSM_GPRS_SECURITY_CONTEXT ;
      }
      else
      {
        if (is_umts_mode() || is_lte_mode())
        {
          security_context = AUTH_PS_GPRS_SECURITY_CONTEXT ;
        }
        else
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unknown MS operation mode",0,0,0) ;
        }
      }
      break ;
    default :
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid domain name ID ",0,0,0) ;
      break ;
  } /* switch (cn_domain_identity) */

  return security_context ;
} /* end mm_get_current_security_context() */


/*===========================================================================

FUNCTION MM_PERFORM_MM_CONNECTION_MANAGEMENT

DESCRIPTION
  This function routes messages relating to connection management
  to an appropriate function for detailed processing


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_perform_mm_connection_management(mm_cmd_type * msg_p)
{
   auth_security_context_id security_context ;
   boolean cs_registered;
   boolean  cs_active_on_other_sub = FALSE;

#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type local_sub_id = SYS_MODEM_AS_ID_1;
  boolean is_multisim_device = TRUE;
#endif

   if ((msg_p->cmd.hdr.message_set == MS_MM_RR) ||
       ((msg_p->cmd.hdr.message_set == MS_MM_TDSRRC)
#ifdef FEATURE_SEGMENT_LOADING
       &&get_tdscdma_interface()
#endif
))
   {
      switch (msg_p->cmd.hdr.message_id)
      {
         case RR_ABORT_IND:
            mm_report_low_level_failure(msg_p);
            break;
         case RRC_ABORT_IND:
            /* --------------------------------------------------------------------
            ** 24.008 4.4.4.9, 4.5.1.2: cases f.1 
            ** RR connection release in Iu mode before the completion of the procedure:
            ** The procedure shall be initiated again conditionally
            ** -------------------------------------------------------------------- */
            if(mm_cs_domain_possible_reest &&
              ((mm_state == MM_LOCATION_UPDATE_INITIATED) ||
               (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) ||
               (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
               (mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) ||
               (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION)))
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= Re-establishing after unintended abort during MM procedure ");
              mmrrcconn_send_L3_message_to_rrc(
                     pending_mm_msg, pending_mm_msg_cause, RRC_CS_DOMAIN_CN_ID );

              mm_state_control( MM_STD_REESTABLISHMENT_REQUIRED );
            }
            else
            {
              mm_report_low_level_failure(msg_p);
            }

            break;

         case RR_ABORT_CNF:
            MSG_HIGH_DS_0(MM_SUB, "=MM= MM received RR_ABORT_CNF");
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
            /* --------------------------------
            ** Abort a LCS session in progress
            ** -------------------------------- */
            if (mm_lcs_active)
            {
               mm_abort_lcs();
            }
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

#ifdef FEATURE_TC
            mm_tc_is_active = FALSE;
#endif
           
            if (mm_cnm_is_active &&
                (mm_held_cnm_service_req.present == FALSE) &&
                (mm_state == MM_WAIT_FOR_NETWORK_COMMAND) &&
                (mm_sms_is_active == FALSE))
            {
              if (mm_cnm_rel_req_received)
              {
                 mm_send_mmcnm_rel_cnf();
              }
              else
              {
                mm_send_mmcnm_rel_ind( OTHER_CAUSE, FALSE );
              }
            }

            mm_stop_timer( TIMER_T3240);
            mm_stop_timer( TIMER_T3230);
            mm_stop_timer( TIMER_T3210);
            mm_state_control( MM_STD_LOW_LEVEL_FAIL );
            if(mm_held_cnm_service_req.present)
            {
              MSG_HIGH_DS_0(MM_SUB, "=MM= Processing Pending call");
              mm_check_pending_requests();
            }
            mm_send_mmcc_sig_conn_rel_ind();

            if (mm_rat_handler_status == MM_RAT_STOP_GSM_PENDING_ABORT)
            {
               mm_rat_handler_status = MM_RAT_STOP_GSM_IN_PROGRESS;
               /* -------------------------
               ** Deactivate the GSM RAT
               ** ------------------------- */
               mm_send_rr_stop_mode_req(RR_MM_RAT_CHANGE);
            }
            break;

         case RRC_ABORT_CNF:
            if(msg_p->cmd.rrc_abort_cnf.conn_rel_cause == RRC_UE_INIT_DORMANCY_UE_IN_OOS || 
               msg_p->cmd.rrc_abort_cnf.conn_rel_cause == RRC_UE_INIT_DORMANCY_ABORT_SUCCESS
#ifdef FEATURE_DUAL_SIM
               || (msg_p->cmd.rrc_abort_cnf.conn_rel_cause == RRC_REL_DUAL_STANDBY_PS_ABORT_SUCCESS)
#endif
              )
            {
                /* Ignore - GMM will process this Abort Cnf */
            }
            else

            /* --------------------------------------------------------------------
            ** 24.008 4.4.4.9, 4.5.1.2: cases f.1 
            ** RR connection release in Iu mode before the completion of the procedure:
            ** The procedure shall be initiated again conditionally
            ** -------------------------------------------------------------------- */
            if(mm_cs_domain_possible_reest)
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= Re-establishing after MM procedure initiated while release in progress");
              mmrrcconn_send_L3_message_to_rrc(
                     pending_mm_msg, pending_mm_msg_cause, RRC_CS_DOMAIN_CN_ID );

              mm_state_control( MM_STD_REESTABLISHMENT_REQUIRED );
            }
            else if(pending_mm_msg_after_release)
            {
              pending_mm_msg_after_release = FALSE;
            }           
            else
            
            {
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
             /* --------------------------------
              ** Abort a LCS session in progress
              ** -------------------------------- */
              if (mm_lcs_active)
              {
                 mm_abort_lcs();
              }
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

#ifdef FEATURE_TC
              mm_tc_is_active = FALSE;
#endif

              /* -----------------------------------------------------
              ** Integrity protection on CS domain is no longer active
              ** ----------------------------------------------------- */
              mm_integrity_protected = FALSE;
              mm_stop_timer( TIMER_T3240);
              mm_stop_timer( TIMER_T3230);
              mm_state_control( MM_STD_LOW_LEVEL_FAIL );
              mm_send_mmcc_sig_conn_rel_ind();
              mm_send_cm_cipher_ind(SYS_SRV_DOMAIN_CS_ONLY,FALSE);
            }
            break;

         case RR_PAGING_IND:
            /* -------------------------------------------------------------
            ** If the MS receives a paging request for CS services during an
            ** ongoing MM procedure, and the MS has already requested the
            ** establishment of a radio connection, the MS shall ignore the
            ** paging request and the MS and the network shall continue the
            ** MM procedure.
            ** ------------------------------------------------------------- */
#ifdef FEATURE_DUAL_SIM
            if(MMCNM_IS_MODE_DUAL_STANDBY)      
            {
              sys_modem_as_id_e_type  mm_other_as_id = SYS_MODEM_AS_ID_1;
              cs_active_on_other_sub = FALSE;
              for(mm_other_as_id = SYS_MODEM_AS_ID_1; mm_other_as_id < MAX_NAS_STACKS ; mm_other_as_id++)
              {
                if((mm_state_sim[mm_other_as_id] == MM_CONNECTION_ACTIVE) || ( mm_state_sim[mm_other_as_id] == MM_WAIT_FOR_RR_CONNECTION_MM)||
                   (mm_state_sim[mm_other_as_id] == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) || 
                   (mm_state_sim[mm_other_as_id] == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) ||
                   (mm_state_sim[mm_other_as_id] == MM_WAIT_FOR_REESTABLISH_DECISION) ||
                   (mm_state_sim[mm_other_as_id] == MM_WAIT_FOR_RR_CONNECTION_REESTABLISHMENT)||
                   (mm_state_sim[mm_other_as_id] == MM_WAIT_FOR_NETWORK_COMMAND)
#ifdef FEATURE_LTE
                || ((mm_serving_plmn_sim[mm_other_as_id].info.active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
                    ((emm_db_get_ctrl_data_per_subs(mm_other_as_id)->emm_sms_active) ||
                     (mm_csfb_state_sim[mm_other_as_id] == MM_CSFB_STATE_MO_CALL_ORIG ||
                      mm_csfb_state_sim[mm_other_as_id] == MM_CSFB_STATE_MT_CALL_ORIG ||
                      mm_csfb_state_sim[mm_other_as_id] == MM_CSFB_STATE_LU_PENDING ||
                      mm_csfb_state_sim[mm_other_as_id] == MM_CSFB_STATE_MT_CALL_CCO)))                  
#endif 
                  )
                  {
                    cs_active_on_other_sub = TRUE;
                    break ;
                  }
               }
            }
            for(local_sub_id = SYS_MODEM_AS_ID_1;local_sub_id < MAX_AS_IDS; local_sub_id++)
            {
              if (mm_sub_config_sim[local_sub_id] != SYS_OVERALL_FEATURE_MODE_MULTISIM)
              {
                is_multisim_device = FALSE;
              }
            }
#if defined (FEATURE_LTE)
            for(local_sub_id = SYS_MODEM_AS_ID_1;local_sub_id < MAX_AS_IDS; local_sub_id++)
            {
              emm_ctrl_data_type        *emm_ctrl_data_ptr = NULL;
              boolean mmtel_vowifi_call_status = FALSE;
              if (local_sub_id != mm_sub_id)
              {
                emm_ctrl_data_ptr = emm_db_get_ctrl_data_per_subs(local_sub_id);
                mmtel_vowifi_call_status = mm_emm_get_vowifi_call_status( emm_ctrl_data_ptr);
              }
              if(mmtel_vowifi_call_status  == TRUE )
              {
                MSG_HIGH_DS_0(MM_SUB,"=MM= Ignoring page on current SUB as VoLTE Call is ongoing on other SUB");
                return;
              }
            }
#endif
#endif

            if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                (((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) &&
                  ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)||
                   (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
                   ((gmm_ra_update_type == GMM_PERIODIC_UPDATING) && (mm_gs_association_established)) ) ) 
                     ||
                 ((gmm_state == GMM_REGISTERED_INITIATED) && 
                  ((gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)||
                   (gmm_attach_type == GMM_GPRS_ATTACH_WHILE_IMSI_ATTACHED)) ) ))
            {
              MSG_HIGH_DS_2( MM_SUB, "=MM= MM ignored RR_PAGING_IND, combined NMO1 Reg in progress, gmm state is %d, gmm_ra_update_type - %d", gmm_state,gmm_ra_update_type);
            }
#ifdef FEATURE_DUAL_SIM
            else if(MMCNM_IS_MODE_DUAL_STANDBY && is_multisim_device && (cm_call_is_there_call_on_other_sub((sys_modem_as_id_e_type)mm_as_id,CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_EMERG_MASK ) == TRUE))
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= ignoring page on this sub as CM is starting a voice call on other sub");
            }
#endif
            else if ((mm_state == MM_IDLE) && (!cs_active_on_other_sub))
            {
              security_context = mm_get_current_security_context( RRC_CS_DOMAIN_CN_ID );
              
              cs_registered = (mm_serving_plmn.ms_op_mode== SYS_SRV_DOMAIN_CS_ONLY || 
                               mm_serving_plmn.ms_op_mode== SYS_SRV_DOMAIN_CS_PS);
              {
                if (cs_registered) 
                {
                  if((gmm_state == GMM_DEREGISTERED_INITIATED) && (gmm_power_down_active))
                  {
                    MSG_HIGH_DS_0(MM_SUB, "=MM= MM ignored RR_PAGING_IND as power down is initiated");
                  }
#if defined(FEATURE_LTE)
                  else if (mm_ps_detach_state != MM_PS_DETACH_STATE_NONE)
                  {
                    MSG_HIGH_DS_0(MM_SUB, "=MM= MM ignored RR_PAGING_IND as PS Detach is ongoing");
                  }
#endif
                  else if(((mmcoord_pending_reg_message.cmd.hdr.message_set == MS_MM_REG) &&
                           (mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
                           (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
#ifdef FEATURE_NAS_ECALL               
                     && mm_ecall_mode_type != MMR_ECALL_MODE_ECALL_ONLY
#endif
                  )
                  {
                    MSG_HIGH_DS_0(MM_SUB, "=MM= MM ignored RR_PAGING_IND as net_sel_mode is Limited");
                  } 
                  else
                  {
                    mm_connection_establishment(msg_p,security_context);
                  }
                }
                else
                {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= MM ignored RR_PAGING_IND, due to cs_registered is FALSE");
                }
              }
            }
            else
            {
              MSG_HIGH_DS_2( MM_SUB, "=MM= MM ignored RR_PAGING_IND mm state is %d or cs active on other sub %d", mm_state,cs_active_on_other_sub);
            }
            break;

#if defined (FEATURE_GSM_EDTM) || defined (FEATURE_GAN)
         case RR_EST_IND:
            if (mm_state == MM_IDLE)
            {
              if ( mmsim_sim_valid (SYS_SRV_DOMAIN_CS_ONLY) == FALSE )
              {
                MSG_ERROR_DS( MM_SUB, "=MM= Unexpected message when SIM is invalidated for CS domain", 0, 0, 0);
              }
              else
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= MM received RR_EST_IND");
                /*Timer 3211 is paused till the connection is released and MM state goes to Idle.*/
                if (mm_timer_status [ TIMER_T3211 ] == TIMER_ACTIVE)
                {
                  if (gs_modify_timer_ext( get_mm_timer_queue(),
                                           TIMER_T3211, 
                                           TIMER_MODIFY_PAUSE,
                                           TRUE) != GS_SUCCESS)
                  {
                    MSG_ERROR_DS( MM_SUB, "=MM= Failed to pause timer %d", TIMER_T3211,  0, 0);
                  }
                  else
                  {
                    MSG_HIGH_DS_1(MM_SUB ,"=MM= Pausing %d", TIMER_T3211);
                    mm_timer_status[ TIMER_T3211 ] = TIMER_PAUSED;
                  }
                }
                mm_last_paging_cause = (byte)CS_PAGING;
                rrc_connection_status = MMRRC_ACTIVE;

#ifdef FEATURE_NAS_ECALL
                if (mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY)
                {
                  if(mm_call_est_cause == NO_CNM_SERVICE)
                  {
                    mm_call_est_cause = EMERGENCY_CALL_ESTABLISHMENT;
                  }
                  mm_stop_timer(TIMER_T3242);
                  mm_stop_timer(TIMER_T3243);
                }
#endif
#ifdef FEATURE_SEND_SEQUENCE_NUMBER

                 /* Reset the send sequence number for the next run */
                send_sequence_number_mm_cc_ss = 0;

#endif
                /*MM goes to state WAIT for N/W Command*/
                mm_state_control( MM_STD_RR_CONNECTION_ESTABLISHED );
              }

            }
            else
            {
              MSG_ERROR_DS( MM_SUB, "=MM= MM ignored RR_EST_IND as another connection already exists", 0,0,0 );
            }
            break;
#endif

         case RRC_PAGE_IND:
            /* -------------------------------------------------------------
            ** If the MS receives a paging request for CS services during an
            ** ongoing MM procedure, and the MS has already requested the
            ** establishment of a radio connection, the MS shall ignore the
            ** paging request and the MS and the network shall continue the
            ** MM procedure.
            ** ------------------------------------------------------------- */
#ifdef FEATURE_DUAL_SIM
            if(MMCNM_IS_MODE_DUAL_STANDBY)
            {
               sys_modem_as_id_e_type  mm_other_as_id = SYS_MODEM_AS_ID_1;
               cs_active_on_other_sub = FALSE;
               for(mm_other_as_id = SYS_MODEM_AS_ID_1; mm_other_as_id < MAX_NAS_STACKS ; mm_other_as_id++)
               {
                 if((mm_state_sim[mm_other_as_id] == MM_CONNECTION_ACTIVE) || ( mm_state_sim[mm_other_as_id] ==MM_WAIT_FOR_RR_CONNECTION_MM)||
                    (mm_state_sim[mm_other_as_id] == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) || 
                    (mm_state_sim[mm_other_as_id] == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) ||
                    (mm_state_sim[mm_other_as_id] == MM_WAIT_FOR_REESTABLISH_DECISION) ||
                    (mm_state_sim[mm_other_as_id] == MM_WAIT_FOR_RR_CONNECTION_REESTABLISHMENT)        
#ifdef FEATURE_LTE
                 || ((mm_serving_plmn_sim[mm_other_as_id].info.active_rat== SYS_RAT_LTE_RADIO_ACCESS) &&
                     ((emm_db_get_ctrl_data_per_subs(mm_other_as_id)->emm_sms_active) ||
                      (mm_csfb_state_sim[mm_other_as_id] == MM_CSFB_STATE_MO_CALL_ORIG ||
                       mm_csfb_state_sim[mm_other_as_id] == MM_CSFB_STATE_MT_CALL_ORIG ||
                       mm_csfb_state_sim[mm_other_as_id] == MM_CSFB_STATE_LU_PENDING ||
                       mm_csfb_state_sim[mm_other_as_id] == MM_CSFB_STATE_MT_CALL_CCO)))                  
#endif 
                  )
                  {
                    cs_active_on_other_sub = TRUE;
                    break ;
                  }
               }
            }
#if defined (FEATURE_LTE)
			for(local_sub_id = SYS_MODEM_AS_ID_1;local_sub_id < MAX_AS_IDS; local_sub_id++)
			{
			  emm_ctrl_data_type		*emm_ctrl_data_ptr = NULL;
			  boolean mmtel_vowifi_call_status = FALSE;
			  if (local_sub_id != mm_sub_id)
			  {
				emm_ctrl_data_ptr = emm_db_get_ctrl_data_per_subs(local_sub_id);
				mmtel_vowifi_call_status = mm_emm_get_vowifi_call_status( emm_ctrl_data_ptr);
			  }
			  if(mmtel_vowifi_call_status  == TRUE && msg_p->cmd.rrc_page_ind.paging_cause != RRC_PAGE_CAUSE_SMS &&
			  	(msg_p->cmd.rrc_page_ind.paging_cause != RRC_PAGE_CAUSE_LOW_PRI_SIGNALLING))
			  {
				MSG_HIGH_DS_0(MM_SUB,"=MM= Ignoring page on current SUB as VoLTE Call is ongoing on other SUB");
				return;
			  }
			}
#endif
		

#endif

            if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                (((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) &&
                  ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)||
                   (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
                   ((gmm_ra_update_type == GMM_PERIODIC_UPDATING) && (mm_gs_association_established)) ) ) 
                     ||
                 ((gmm_state == GMM_REGISTERED_INITIATED) && 
                  ((gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)||
                   (gmm_attach_type == GMM_GPRS_ATTACH_WHILE_IMSI_ATTACHED)) ) ))
            {
              MSG_HIGH_DS_2( MM_SUB, "=MM= MM ignored RRC_PAGING_IND, combined NMO1 Reg in progress, gmm state is %d, gmm_ra_update_type - %d", gmm_state,gmm_ra_update_type);
            }
            else if ((mm_state == MM_IDLE) && (!cs_active_on_other_sub))
            {
              cs_registered = (mm_serving_plmn.ms_op_mode== SYS_SRV_DOMAIN_CS_ONLY || 
                               mm_serving_plmn.ms_op_mode== SYS_SRV_DOMAIN_CS_PS);
              if (cs_registered)
              {
               if((gmm_state == GMM_DEREGISTERED_INITIATED) && (gmm_power_down_active))
               {
                 MSG_HIGH_DS_0(MM_SUB, "=MM= MM ignored RRC_PAGING_IND as power down is initiated");
               }
#if defined(FEATURE_LTE)
               else if (mm_ps_detach_state != MM_PS_DETACH_STATE_NONE)
               {
                 MSG_HIGH_DS_0(MM_SUB, "=MM= MM ignored RRC_PAGING_IND as PS Detach is ongoing");
               }
#endif
               else if(((mmcoord_pending_reg_message.cmd.hdr.message_set == MS_MM_REG) &&
                        (mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
                        (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
#ifdef FEATURE_NAS_ECALL               
                && mm_ecall_mode_type != MMR_ECALL_MODE_ECALL_ONLY
#endif
               )
               {
                 MSG_HIGH_DS_0(MM_SUB, "=MM= MM ignored RRC_PAGE_IND as net_sel_mode is Limited");
               }     
               else
               {
                 mm_paging_response(msg_p);
                 if ( (mm_nas_feature_to_be_configured.UE_init_rel_on_spurious_page) &&
                      (mm_state == MM_WAIT_FOR_NETWORK_COMMAND))
                 {
                   mm_start_timer(TIMER_SCRI_PAGING, DEFAULT_TIMEOUT );
                 }
               }
              }
              else
              {
                 MSG_HIGH_DS_0(MM_SUB, "=MM= MM ignored RRC_PAGE_IND,due to cs_registered is FALSEd");
              }
            }
            else
            {
              MSG_HIGH_DS_2( MM_SUB, "=MM= MM ignored RRC_PAGE_IND MM state is %d  or cs is active on other sub %d ", mm_state,cs_active_on_other_sub);
            }
            break;

         case RR_REL_IND:
         case RRC_REL_IND:
         case RRC_CLOSE_SESSION_IND:
         case RRC_CLOSE_SESSION_CNF:
            mm_connection_release(msg_p);
            break;

         case RRC_OPEN_SESSION_CNF:
           mm_connection_establishment(
                  msg_p, mm_get_current_security_context(
                               msg_p->cmd.rrc_open_session_cnf.cn_domain_id)) ;
           break ;
         case RRC_SYNC_IND:
           mm_connection_establishment(
                  msg_p, mm_get_current_security_context(
                                       msg_p->cmd.rrc_sync_ind.cn_domain_id)) ;
           break ;
         case RR_EST_CNF:
         case RR_SYNC_IND:
         case RRC_EST_CNF:
            mm_connection_establishment(
                    msg_p,
                      mm_get_current_security_context( RRC_CS_DOMAIN_CN_ID )) ;
            break;
         case MM_DATA_IND:
         case RR_DATA_IND:
            if (get_OTA_msg_type(msg_p) == ABORT )
            {
               mm_connection_release(msg_p);
            }
            else if ( (get_OTA_msg_type(msg_p) == CM_SERVICE_ACCEPT ) ||
                      (get_OTA_msg_type(msg_p) == CM_SERVICE_REJECT ) )
            {
               mm_connection_establishment(
                        msg_p,mm_get_current_security_context(
                                                       RRC_CS_DOMAIN_CN_ID )) ;
            }
            
            #ifdef FEATURE_CCBS
            else if (get_OTA_msg_type(msg_p) == CM_SERVICE_PROMPT )
            {
               mm_connection_establishment(
                        msg_p,mm_get_current_security_context(
                                                       RRC_CS_DOMAIN_CN_ID )) ;
            }
            #endif /* FEATURE_CCBS */
            else
            {
               MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected message type %d from RR", get_OTA_msg_type(msg_p), 0,0);
            }
            break;

         default:
            MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected message id %d from RR", msg_p->cmd.hdr.message_id, 0,0);
            break;
      }
   }
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   else if (msg_p->cmd.hdr.message_set == MS_MM_LSM )
   {
      switch(msg_p->cmd.hdr.message_id)
      {
         case LCS_ACTIVE_IND:
            if(msg_p->cmd.lcs_active_ind.lcs_in_progress)
            {
               mm_state_control(MM_STD_LCS_STARTED);

            }
            else  /* if the LCS procedure has just finished */
            {
               MSG_HIGH_DS_0(MM_SUB,"=MM= MM rec'd LCS_ACTIVE_IND - LCS ended");

               mm_process_lcs_session_end();
            }
            break;
         default:
            MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected message id %d from LSM", msg_p->cmd.hdr.message_id, 0,0);
            break;
      }
   }
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
   else if ( msg_p->cmd.hdr.message_set == MS_CC_MM )
   {
      if (msg_p->cmd.hdr.message_id == MMCNM_REL_REQ)
      {
         mm_cnm_rel_req_received = TRUE;
         
          /* Send this SMS to GMM is no active CS call and 
             PS SMS is active */
         if ((mm_cnm_is_active != TRUE) &&
             (mm_sms_is_active == TRUE ))
         {
           gmm_handle_connection_management(msg_p) ;
         }        
         else
         {
         mm_connection_release(msg_p);
      }
      }
      else if (msg_p->cmd.hdr.message_id == MMCC_REEST_REQ )
      {
         mm_flag_reestablishment_requested(msg_p);
      }
      else if ( msg_p->cmd.hdr.message_id == MMCNM_EST_REQ )
      {
         mm_connection_establishment(msg_p,
                      mm_get_current_security_context( RRC_CS_DOMAIN_CN_ID )) ;
      }
      
      #ifdef FEATURE_CCBS 
      else if ( msg_p->cmd.hdr.message_id == MMCNM_PROMPT_RES )
      {
         mm_connection_establishment(msg_p,
                      mm_get_current_security_context( RRC_CS_DOMAIN_CN_ID )) ;
      }
      #endif 
      
      else
      {
         MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected CNM message id %d", msg_p->cmd.hdr.message_id, 0,0);
      }
   }
   else if ( msg_p->cmd.hdr.message_set == MS_TIMER )
   {
      switch (msg_p->cmd.timer_expiry.timer_id)
      {
         case TIMER_T3230:
            if ((mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
                (mm_state == MM_WAIT_FOR_RR_CONNECTION_MM))
            {
               mm_start_timer( TIMER_T3240, DEFAULT_TIMEOUT );
            }
            mm_state_control( MM_STD_T3230_TIMEOUT );

            break;
         case TIMER_SCRI_PAGING:
         case TIMER_T3240:
#ifdef FEATURE_WCDMA            
               if (is_umts_mode())
               {
                  rrc_cmd_type *rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
                  if (rrc_cmd_p == NULL)
                  {
                     MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                  }

                  rrc_cmd_p->cmd_hdr.cmd_id = RRC_CLOSE_SESSION_REQ;
                  rrc_cmd_p->cmd.close_sess_req.cn_domain_id = RRC_CS_DOMAIN_CN_ID;
                  rrc_cmd_p->cmd.close_sess_req.close_session_cause = RRC_OTHERS;
                  send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
               }
               else /* GSM mode */
#endif /* FEATURE_WCDMA */             
               {
                  mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );
               }

               mm_state_control( MM_STD_T3240_TIMEOUT );
            
            break;

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
         case TIMER_T3241:
            /* --------------------------------------------
            ** Abort the RR(C) connection.
            ** Upon receiving a RR(C)_ABORT_CNF, MM sends a
            ** LCS_ABORT_IND to LSM
            ** ------------------------------------------- */
            mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE);
            break;
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

         case TIMER_MM_REL:
#ifdef FEATURE_DUAL_SIM
            if((mm_abort_dual_switch_pending == SYS_STOP_MODE_REASON_DSDS_SUSPEND || 
                mm_abort_dual_switch_pending == SYS_STOP_MODE_REASON_DUAL_SWITCH || 
                mm_abort_dual_switch_pending == SYS_STOP_MODE_REASON_ALT_SCAN)&& is_umts_mode())
            {
              mm_send_rrc_hard_abort_req(mm_serving_plmn.info.active_rat);
            }
            else
#endif
            {
            if ((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
            {
              gmm_set_state( GMM_DEREGISTERED );
            }
            if (rrc_connection_status != MMRRC_IDLE)
            {
              if (is_gsm_mode())
              {
                 mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );
              }
#ifdef FEATURE_WCDMA          
              else if (is_umts_mode())
              {
                 rrc_cmd_type *rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );

                 if (rrc_cmd_p == NULL)
                 {
                    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                 }               
                 rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
                 rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
                 rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = FALSE;
                 rrc_cmd_p->cmd.rrc_abort_req.hard_abort = TRUE;
                 rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
                 send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
              }
#endif /* FEATURE_WCDMA */        
              if ((mm_state == MM_CONNECTION_ACTIVE) && mm_cnm_is_active)
              {
                mm_send_mmcnm_rel_ind( OTHER_CAUSE, FALSE );
              }
              mm_state_control( MM_STD_RR_CONNECTION_ABORTED );
              mm_nreg_req_received = TRUE;
              /* ---------------------------------------------------------------------
                ** Set ATT flag to FALSE so we don't attempt to perform multiple DETACHs
                ** --------------------------------------------------------------------- */
              mm_system_information.ATT = FALSE;
               
              mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                     SYS_SRV_DOMAIN_NO_SRV,
                                                     TRUE,
                                                     SYS_SRV_STATUS_LIMITED,
                                                     TRUE );
            }
            else
            {
              mm_state_control( MM_STD_MS_DEACTIVATED_NO_ATTACH );          
            }
            }
            break;

         default:
           MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected timer id %d", msg_p->cmd.timer_expiry.timer_id, 0,0);
           break;
      }

   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected message set %d received by MM CALL control",
                                    msg_p->cmd.hdr.message_set, 0,0);
   }
}


/*===========================================================================

FUNCTION MM_INITIATE_RR_CONNECTION

DESCRIPTION
  This function sends the request to RR requesting a connection
  establishment and sending a MM message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_initiate_rr_connection(establishment_cause_T establishment_cause,
                              IMH_T * message)
{
   rr_est_req_T         rr_est_req;
   uint32               length;

   byte pd = 0;
#ifdef FEATURE_DUAL_SIM
   mm_stop_timer( TIMER_MM_RADIO_RETRY );
#endif
   memset(&rr_est_req,0x00,sizeof(rr_est_req_T));
  /*
   * If in UMTS mode, handle the request by mm_initiate_rrc_connection
   */
   if (is_umts_mode())
   {
      rrc_establish_cause_e_type rrc_est_cause;

      rrc_est_cause = convert_est_cause_to_rrc(establishment_cause);

      if (gmm_cause_directed_signalling_reest &&
         (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) &&
         (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
      {
        mm_initiate_rrc_connection(RRC_EST_CALL_RE_ESTABLISH, message);
      }
      else
      {
        mm_initiate_rrc_connection(rrc_est_cause, message);
      }
   }
   else if(is_gsm_mode())
   {
      /** --------------------------------------------
       ** Pause timer T3211 if it is already running
       ** -------------------------------------------- */
      if (mm_timer_status [ TIMER_T3211 ] == TIMER_ACTIVE)
      {
         if (gs_modify_timer_ext( get_mm_timer_queue(),
                                  TIMER_T3211,
                                  TIMER_MODIFY_PAUSE,
                                  TRUE) != GS_SUCCESS)
         {
           MSG_ERROR_DS( MM_SUB, "=MM= Failed to pause timer %d", TIMER_T3211,  0, 0);
         }
         else
         {
           MSG_HIGH_DS_1(MM_SUB ,"=MM= Pausing %d", TIMER_T3211);
           mm_timer_status[ TIMER_T3211 ] = TIMER_PAUSED;
         }
      }

#ifdef FEATURE_LTE
      rr_est_req.is_periodic_lau = FALSE;
#endif

      if (establishment_cause == MM_RR_LOCATION_UPDATE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent LOCATION UPDATE REQUEST");

        mm_start_timer( TIMER_T3210, DEFAULT_TIMEOUT );
#ifdef FEATURE_LTE
        if(mm_get_lu_cause() == MM_LU_CAUSE_PERIODIC)
        {
          rr_est_req.is_periodic_lau = TRUE;
        }
#endif
      }

      rr_est_req.message_header.message_set = MS_MM_RR;
      rr_est_req.message_header.message_id  = (int)RR_EST_REQ;

      /* Get the length of the layer3_message */
      GET_IMH_LEN( length, message );


      PUT_IMH_LEN( sizeof(rr_est_req_T) - sizeof(IMH_T),
                        &rr_est_req.message_header );

      rr_est_req.l3_message_size = (byte) length;

      memset(rr_est_req.layer3_message,0x00,
             sizeof(rr_est_req.layer3_message));

#ifdef FEATURE_LTE
      if ((mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) && 
          ((mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG) || 
           (mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG) ||
           (mm_waiting_for_csmt_response == TRUE)))
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Changing the Establishment Cause to CSFB Cause for High Prio for GSM N/W");
        if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
        {
          switch(mm_csfb_service_req_message_ptr->cmd.mmcnm_est_req.CNM_service_type)
          {
            case EMERGENCY_CALL_ESTABLISHMENT:
              rr_est_req.establishment_cause = MM_RR_EMERGENCY_CALL;
              break;

            case SHORT_MESSAGE_SERVICE_ESTABLISHMENT:
              rr_est_req.establishment_cause = MM_RR_SHORT_MESSAGE;
              break;

            case SUPPLEMENTARY_SERVICE_ACTIVATION:
              rr_est_req.establishment_cause = MM_RR_SUPPLEMENTARY_SERVICE;
              break;

            case MO_CALL_ESTABLISHMENT_FULL_SPEECH:
              rr_est_req.establishment_cause = MM_RR_MO_FULL_SPEECH;
              break;

            case MO_CALL_ESTABLISHMENT_HALF_SPEECH:
              rr_est_req.establishment_cause = MM_RR_MO_HALF_SPEECH;
              break;

            case MO_CALL_ESTABLISHMENT_DATA_9600:
              rr_est_req.establishment_cause = MM_RR_MO_DATA_9600;
              break;

            case MO_CALL_ESTABLISHMENT_DATA_4800:
              rr_est_req.establishment_cause = MM_RR_MO_DATA_4800;
              break;

            case MO_CALL_ESTABLISHMENT_DATA_2400:
              rr_est_req.establishment_cause = MM_RR_MO_DATA_2400;
              break;

            default:
              MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected cnm_service_type %d", mm_csfb_service_req_message_ptr->cmd.mmcnm_est_req.CNM_service_type, 0,0);
              rr_est_req.establishment_cause = MM_RR_MO_FULL_SPEECH;  /* default to speech */
              break;
          }
          
        }
        else
        {
          rr_est_req.establishment_cause = MM_RR_ANSWER_TO_PAGING;
        }
      }
      else
#endif
      {
        rr_est_req.establishment_cause = (byte) establishment_cause;
      }

      (void)memscpy(rr_est_req.layer3_message,N201_MAX,
              (byte *)message + sizeof(IMH_T),
              length );

      if (rr_est_req.layer3_message[1] == CM_SERVICE_REQUEST)
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent CM_SERVICE_REQUEST");

        if (establishment_cause == MM_RR_SHORT_MESSAGE)
        {
          mm_start_timer( TIMER_T3230, DEFAULT_TIMEOUT );
        }

        mm_follow_on_request = FALSE;
        mm_held_cnm_service_req.present = FALSE;

#ifdef FEATURE_NAS_ECALL
        if(mm_call_est_cause == NO_CNM_SERVICE)
        {
          mm_call_est_cause = (rr_est_req.layer3_message[2]) & (0x0F);
        }
        if (mm_call_est_cause == EMERGENCY_CALL_ESTABLISHMENT)
        {
          mm_stop_timer(TIMER_T3242);
          mm_stop_timer(TIMER_T3243);
        }
#endif

      }

            /* Reset the send sequence number for the next run */
      send_sequence_number_mm_cc_ss = 0;

      if (establishment_cause != MM_RR_ANSWER_TO_PAGING)
      {
        /* PD is the lower nibble */
        pd = rr_est_req.layer3_message[0] & 0x0F;
      }

      /* Send seq num is only applicable to MM, CC, SS msgs */
      if( pd == PD_MM || pd == PD_CC || pd == PD_SS)
      {
         /*
          * The send seq number in a message type is 0 until it is changed
          * No need to update here as the send seq number is always 0 on
          * the first MM/CC/SS L3 message to the network
          */

         /* Increment the send sequence number
          * and do modulo 4 arithmetic for the next run
          */

         ++send_sequence_number_mm_cc_ss;
      }

      /* Log NAS OTA message */
      mm_send_ota_msg_log_packet(UE_TO_CN_DIRECTION, length, rr_est_req.layer3_message);

      MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RR_EST_REQ");

      mm_send_message( (IMH_T *)&rr_est_req, GS_QUEUE_RR );
      rrc_connection_status = MMRRC_EST_IN_PROGRESS;
   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= MM: Unknown mode (neither GSM nor UMTS)", 0,0,0);
   }
}


/*===========================================================================

FUNCTION MM_INITIATE_RRC_CONNECTION

DESCRIPTION
  This function sends the request to RRC requesting a connection
  establishment and the MM message to be sent is stored in pending_rrc_req_msg
  On a successful conn. establishment (RRC_EST_CNF), the stored NAS message
  is sent using the open_session_req primitive.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_initiate_rrc_connection(rrc_establish_cause_e_type establishment_cause,
                              IMH_T * message )
{
   /** --------------------------------------------
    ** Pause timer T3211 if it is already running
    ** -------------------------------------------- */
   if (mm_timer_status [ TIMER_T3211 ] == TIMER_ACTIVE)
   {
      if (gs_modify_timer_ext( get_mm_timer_queue(),
                               TIMER_T3211,
                               TIMER_MODIFY_PAUSE,
                               TRUE) != GS_SUCCESS)
      {
         MSG_ERROR_DS( MM_SUB, "=MM= Failed to pause timer %d", TIMER_T3211,  0, 0);
      }
      else
      {
         MSG_HIGH_DS_1(MM_SUB ,"=MM= Pausing %d", TIMER_T3211);
         mm_timer_status[ TIMER_T3211 ] = TIMER_PAUSED;
      }
   }
   mmrrcconn_send_L3_message_to_rrc( message, establishment_cause, RRC_CS_DOMAIN_CN_ID );
}


/*===========================================================================

FUNCTION MM_NOTIFY_MT_RR_CONNECTION_ESTABLISHMENT

DESCRIPTION
  This function notifies the establishment of the RR connection. Input
  parameter is pointer to RR_EST_IND message, though this is not accessed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

VOID_FUNC   mm_notify_mt_rr_connection_establishment(mm_cmd_type* message )
{
   (void) message;            /* avoid lint warning */
   mm_state_control( MM_STD_RR_CONNECTION_ESTABLISHED );
}
#ifndef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
/*===========================================================================

FUNCTION MM_SMS_GET_PREFERABLE_ROUTE

DESCRIPTION
  This function calculates preferable rout (either PS or CS) for MO SMS message

DEPENDENCIES
  None

RETURN VALUE
  Prefered PD identifier

SIDE EFFECTS
  None

===========================================================================*/
byte mm_sms_get_preferable_route( void )
{
  byte destination_pd = PD_MM ;

  if (mm_sms_preferred_domain_id == CS_PREFERRED)
  {
    if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
    {
      destination_pd = PD_GMM;
    }
    else if (mm_state == MM_IDLE) 
    {
      if(is_umts_mode() && (pmm_mode == PMM_CONNECTED))
        destination_pd = PD_GMM;
      }
    }
  }
  else if ((mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE) &&
                                  (mm_sms_preferred_domain_id == PS_PREFERRED))
  {
    if (mm_state == MM_IDLE)
    {
#ifdef FEATURE_GSM_GPRS
      if (is_gsm_mode())
      {
        if ((gmm_substate != GMM_SUSPENDED) && (gmm_state == GMM_REGISTERED))
        {
          destination_pd = PD_GMM ;
        }
      } else
#endif /* #ifdef FEATURE_GSM_GPRS */
      {
        destination_pd = PD_GMM ;
      }
    }
    else
    {
      if (gmm_state == GMM_REGISTERED)
      {
#ifdef FEATURE_GSM_GPRS
        if (is_gsm_mode())
        {
          if (gmm_substate != GMM_SUSPENDED)
          {
            destination_pd = PD_GMM ;
          }
        } else
#endif /* #ifdef FEATURE_GSM_GPRS */
        if (is_umts_mode())
        {
          if (pmm_mode == PMM_CONNECTED)
          {
            destination_pd = PD_GMM ;
          }
        }
        else
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unsupported operation mode",0,0,0) ;
        }
      }
    }
  }

  return destination_pd ;

} /* end mm_sms_get_preferable_route */
#ifdef FEATURE_DSAC
static byte mm_sms_get_route()
{
  byte route = PD_MM;
  mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_NONE);
  mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_NONE);
  
  if (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY &&
      mm_sms_preferred_domain_id == PS_ONLY)
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Cannot send SMS. Registered for CS Only while chosen domain is PS Only.");
    route = FALSE;    
  }
  
  else if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY ||
            mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
           (mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE))
  {
    if ((ps_session_status == MMRRC_ACTIVE) || gmm_check_if_ps_cell_access_allowed(ps_cell_access))
    {
      if ((mm_sms_preferred_domain_id == PS_PREFERRED) &&
          (cs_session_status == MMRRC_ACTIVE) &&
          (ps_session_status == MMRRC_IDLE))
      {
        route = PD_MM;
      }
      else if ((mm_sms_preferred_domain_id == PS_PREFERRED) || 
               (mm_sms_preferred_domain_id == PS_ONLY)      ||
               ((mm_sms_preferred_domain_id == CS_PREFERRED) &&
                (cs_session_status == MMRRC_IDLE) &&
                ((mm_check_if_cs_cell_access_barred(cs_cell_access)) ||
                 (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY)) ||
                 (is_umts_mode() && pmm_mode == PMM_CONNECTED))))
      {
        if (gmm_state == GMM_REGISTERED)
        {
#ifdef FEATURE_GSM_GPRS
          if (is_gsm_mode())
          {
            if (gmm_substate != GMM_SUSPENDED)
            {
              route = PD_GMM ;
            }
          } else
#endif /* #ifdef FEATURE_GSM_GPRS */
          if (is_umts_mode())
          {
              route = PD_GMM ;
          }
          else
          {
            MSG_ERROR_DS(MM_SUB, "=MM= Unsupported operation mode",0,0,0) ;
          }
        }        
      }      
    }
  }
  
  if (route == PD_MM && mm_sms_preferred_domain_id == PS_ONLY)
  {
    route = FALSE;
  }
  
  return route;            
}

#endif /*end of FEATURE_DSAC*/
#else  /*ifndef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION*/
/*===========================================================================

FUNCTION MM_SMS_GET_ROUTE

DESCRIPTION
  This function calculates preferable rout (either PS or CS) for MO SMS message

DEPENDENCIES
  None

RETURN VALUE
  Prefered PD identifier

SIDE EFFECTS
  None

===========================================================================*/
byte mm_sms_get_enhanced_sms_route(void)
{
   byte destination_pd = NULL;
   mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_NONE);
   mm_as_cell_access_status_T ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_NONE);

   MSG_HIGH_DS_2(MM_SUB, "=MM= UE mode is: %d\t SMS preferred domain is %d", mm_serving_plmn.ms_op_mode,mm_sms_preferred_domain_id);
   switch (mm_serving_plmn.ms_op_mode)
   {
      case SYS_SRV_DOMAIN_CS_ONLY:
         if(mm_sms_preferred_domain_id == PS_ONLY)
         {
            /*WMS must reject this */ 
            destination_pd = PS_PERMANENT; 
            MSG_HIGH_DS_0(MM_SUB, "=MM= Can not send SMS, op_mode - CS ONLY and preferred domain - PS ONLY");
         }
         else if(!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
         {
            destination_pd = CS_PERMANENT; 
            MSG_HIGH_DS_0(MM_SUB, "=MM= UE is CS_ONLY; SIM is invalid for CS domain. Sending Permanent error");
         }
         else if((mm_check_if_cs_cell_access_barred(cs_cell_access)) &&
                   (
#ifdef FEATURE_DSAC
                     ((cs_session_status == MMRRC_IDLE) && is_umts_mode() &&
                       (mm_system_information.dsac_present ))||
                     ((rrc_connection_status == MMRRC_IDLE) && (is_gsm_mode() || !mm_system_information.dsac_present ))
#else
                     (rrc_connection_status == MMRRC_IDLE)
#endif
                   ))
         {
            destination_pd = CS_TEMPORARY;  
            MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS: UE mode is CS Only and UE is access classed barred in CS.");
            
         }
         else
         {
            destination_pd = PD_MM;
         }
         break;

      case SYS_SRV_DOMAIN_PS_ONLY:
         if(mm_sms_preferred_domain_id == CS_ONLY)
         {
            /*WMS must reject this */ 
            MSG_HIGH_DS_0(MM_SUB, "=MM= Can not send SMS, op_mode - PS ONLY and preferred domain - CS ONLY");
            destination_pd = CS_PERMANENT;
            
         }
         else if(!mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY))
         {
            destination_pd = PS_PERMANENT; 
            MSG_HIGH_DS_0(MM_SUB, "=MM= UE is PS_ONLY; SIM is invalid for PS domain. Sending Permanent error");
         }
         else if (is_umts_mode() &&
                    (gmm_check_if_ps_cell_access_barred(ps_cell_access)) &&
                    (
#ifdef FEATURE_DSAC
                     ((ps_session_status == MMRRC_IDLE) && (mm_system_information.dsac_present ))||
                     ((rrc_connection_status == MMRRC_IDLE) && (!mm_system_information.dsac_present ))
#else
                     (rrc_connection_status == MMRRC_IDLE)
#endif
                   ))
         {
            destination_pd = PS_TEMPORARY;  
            MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS: UE mode is PS Only and UE is access classed barred in PS.");

         }
         else if(is_gsm_mode() && (gmm_substate == GMM_SUSPENDED))
         {
            destination_pd = PS_TEMPORARY;  
            MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS: UE mode is PS Only and GMM Substate is SUSPENDED.");
         }
         else
         {
            destination_pd = PD_GMM;
         }
         break;

      case SYS_SRV_DOMAIN_CS_PS:
         switch (mm_sms_preferred_domain_id)
         {
            case CS_ONLY:
               if(!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
               {
                  destination_pd = CS_PERMANENT; 
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS: UE mode is CS_PS, Pref is CS_ONLY; Sim is invalid for CS.");
               }
               else if((mm_check_if_cs_cell_access_barred(cs_cell_access)) &&
                   (
#ifdef FEATURE_DSAC
                     ((cs_session_status == MMRRC_IDLE) && is_umts_mode() && (mm_system_information.dsac_present ))||
                     ((rrc_connection_status == MMRRC_IDLE) && (is_gsm_mode() || !mm_system_information.dsac_present ))
#else
                     (rrc_connection_status == MMRRC_IDLE)
#endif
                   ))
               {
                  destination_pd = CS_TEMPORARY; 
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS: UE mode is CS_PS, Pref is CS_ONLY; UE is access classed barred in CS.");
               }
               else
               {

                  destination_pd = PD_MM;
               }
               break;

            case PS_ONLY:
               if(!mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY))
               {
                  destination_pd = PS_PERMANENT; 
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS: UE mode is CS_PS, Pref is PS_ONLY; Sim is invalid for PS.");
               }
               else if (is_umts_mode() &&
                         (gmm_check_if_ps_cell_access_barred(ps_cell_access)) &&
                         (
#ifdef FEATURE_DSAC
                         ((ps_session_status == MMRRC_IDLE) && (mm_system_information.dsac_present ))||
                         ((rrc_connection_status == MMRRC_IDLE) && (!mm_system_information.dsac_present ))
#else
                         ((rrc_connection_status == MMRRC_IDLE))
#endif
                         ))
               {
                  destination_pd = PS_TEMPORARY;  
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS: UE mode is CS_PS and Pref domain is PS Only; UE is access classed barred in PS.");
               }
               else if(is_gsm_mode() && (gmm_substate == GMM_SUSPENDED))
               {
                  destination_pd = PS_TEMPORARY;  
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS: UE mode is CS_PS and Pref domain is PS Only; GMM substate is SUSPENDED.");
               }
               else
               {

                  destination_pd = PD_GMM;
               }
               break;

            case CS_PREFERRED:
               /* If there is an existing PS connection, then use it
                 * Else, CS domain.
                 */
               if(!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
               {
                  if((!mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY)))
                  {
                     destination_pd = CS_PS_PERMANENT; 
                     MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS; UW is CS_PS, Pref is CS_PREF and Sim is invalid in both the domains ");
                  }
                  else if (is_umts_mode() &&
                            (gmm_check_if_ps_cell_access_barred(ps_cell_access)) &&
                            (
#ifdef FEATURE_DSAC
                            ((ps_session_status == MMRRC_IDLE) && (mm_system_information.dsac_present ))||
                            ((rrc_connection_status == MMRRC_IDLE) && (!mm_system_information.dsac_present ))
#else
                            ((rrc_connection_status == MMRRC_IDLE))
#endif
                            ))
                  {
                     destination_pd = PS_TEMPORARY;  
                     MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS: UE mode is CS_PS, Pref domain is CS Pref; Sim is invalid in CS, UE is access classed barred in PS.");
                  }
                  else if(is_gsm_mode() && (gmm_substate == GMM_SUSPENDED))
                  {
                     destination_pd = PS_TEMPORARY;  
                     MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS: UE mode is CS_PS and Pref domain is CS Pref;Sim invalid in CS; GMM substate is SUSPENDED.");
                  }
                  else
                  {
                     /*PS*/
                     destination_pd = PD_GMM;
                  }

               }
               else if((mm_check_if_cs_cell_access_barred(cs_cell_access)) &&
                         (
#ifdef FEATURE_DSAC
                         ((cs_session_status == MMRRC_IDLE) && is_umts_mode() && (mm_system_information.dsac_present ))||
                         ((rrc_connection_status == MMRRC_IDLE) && (is_gsm_mode() || !mm_system_information.dsac_present ))
#else
                         (rrc_connection_status == MMRRC_IDLE)
#endif
                        ))
               {
                  if((!mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY)) ||
                      (is_gsm_mode() && (gmm_substate == GMM_SUSPENDED)) ||
                      (is_umts_mode() &&
                        (gmm_check_if_ps_cell_access_barred(ps_cell_access)) &&
                        (
#ifdef FEATURE_DSAC
                        ((ps_session_status == MMRRC_IDLE) && (mm_system_information.dsac_present ))||
                        ((rrc_connection_status == MMRRC_IDLE) && (!mm_system_information.dsac_present ))
#else
                        ((rrc_connection_status == MMRRC_IDLE))
#endif
                        )))
                  {
                     destination_pd = CS_PS_TEMPORARY; 
                     MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS;UE is CS_PS, Pref is CS_PREF; Sim is valid in CS, CS and PS are access class barred");
                  }
                  else
                  {
                     /*PS*/
                     destination_pd = PD_GMM;
                  }
               }
               /* ((PS Session exists and allowed) and (CS Session does not exists) and (WCDMA RAT)) */
               else if(!(mm_nas_feature_to_be_configured.CS_Domain_absolutely_preferred) &&
                           (mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY)) &&
               (is_umts_mode() && (ps_session_status == MMRRC_ACTIVE) &&
                (cs_session_status != MMRRC_ACTIVE) &&
               (gmm_update_status == GMM_GU1_UPDATED) &&
               (!(gmm_rai_changed( gmm_stored_rai.location_area_code,
                                    gmm_stored_rai.routing_area_code,
                                    mm_serving_plmn.lac, mm_serving_plmn.rac ) ||
                   gmm_plmn_changed(gmm_stored_rai.plmn_id,mm_serving_plmn.info.plmn)))))
               {
                  destination_pd = PD_GMM;
               }
               /* If CS is definitely not available for this try and PS is definitely available, try on PS*/
               else if ((((mm_serving_plmn.info.reg_service_domain != SYS_SRV_DOMAIN_CS_ONLY) &&
                         (mm_serving_plmn.info.reg_service_domain != SYS_SRV_DOMAIN_CS_PS)) ||
                        (mm_serving_plmn.mm_service_state == SERVICE_NOT_AVAILABLE)) &&
                        (gmm_state == GMM_REGISTERED && (gmm_update_status == GMM_GU1_UPDATED &&
                          (!gmm_rai_changed(gmm_stored_rai.location_area_code, gmm_stored_rai.routing_area_code, 
                              mm_serving_plmn.lac, mm_serving_plmn.rac)) &&
                          (!gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn))) &&
                         (is_umts_mode() || !gmm_is_suspended ) && 
                         (!gmm_check_if_ps_cell_access_barred(ps_cell_access)) ))
               {
                     /*PS*/
                  destination_pd = PD_GMM;
               }               
               else
               {
                  destination_pd = PD_MM;
               }
               break;

            case PS_PREFERRED:
               if(!mmsim_sim_valid(SYS_SRV_DOMAIN_PS_ONLY))
               {
                  if(!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
                  {
                     destination_pd = CS_PS_PERMANENT;
                  }
                  else if((mm_check_if_cs_cell_access_barred(cs_cell_access)) &&
                             (
#ifdef FEATURE_DSAC
                             ((cs_session_status == MMRRC_IDLE) && is_umts_mode() &&
                               (mm_system_information.dsac_present ))||
                             ((rrc_connection_status == MMRRC_IDLE) && (is_gsm_mode() || !mm_system_information.dsac_present ))
#else
                             (rrc_connection_status == MMRRC_IDLE)
#endif
                             ))
                  {
                     destination_pd = CS_PS_TEMPORARY;
                     MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS;UE is CS_PS, Pref is PS_PREF; Sim is invalid in PS, CS is access class barred");
                  }
                  else
                  {
                     /*CS*/
                     destination_pd = PD_MM;
                  }
               }
               else if((gmm_check_if_ps_cell_access_barred(ps_cell_access)) && (ps_session_status == MMRRC_IDLE))
               {
                  if((!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY)) ||
                      ((mm_check_if_cs_cell_access_barred(cs_cell_access)) &&
                             (
#ifdef FEATURE_DSAC
                             ((cs_session_status == MMRRC_IDLE) && is_umts_mode() &&
                               (mm_system_information.dsac_present ))||
                             ((rrc_connection_status == MMRRC_IDLE) && (is_gsm_mode() || !mm_system_information.dsac_present ))
#else
                             (rrc_connection_status == MMRRC_IDLE)
#endif
                             )))
                  {
                     destination_pd = CS_PS_TEMPORARY;
                     MSG_HIGH_DS_0(MM_SUB, "=MM= Can't send SMS;UE is CS_PS, Pref is PS_PREF; Sim is invalid in PS, CS and PS are access class barred");
                  }
                  else
                  {
                     /*CS*/
                     destination_pd = PD_MM;
                  }
               }
               else  if((mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY)) &&
                          ((cs_session_status == MMRRC_ACTIVE) || (is_gsm_mode() && (rrc_connection_status == MMRRC_ACTIVE))) && 
                          (mm_location_information.location_update_status == UPDATED) && 
                          (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0) &&
                          (ps_session_status == MMRRC_IDLE))
               {
                  destination_pd = PD_MM;
               }
               else if (((mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE) ||
                         ( (mm_serving_plmn.info.reg_service_domain != SYS_SRV_DOMAIN_PS_ONLY) &&
                           (mm_serving_plmn.info.reg_service_domain != SYS_SRV_DOMAIN_CS_PS) ) ||
#ifdef FEATURE_DUAL_SIM
                         (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id ))) &&
#else
                         (reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn ))) &&
#endif
                         (mm_location_information.location_update_status == UPDATED && 
                         (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)) &&
                         ((!mm_check_if_cs_cell_access_barred(cs_cell_access)) ))
               {
                /* If PS is definitely not available for this try and CS is definitely available to send SMS , try on CS*/
                  destination_pd = PD_MM;
               }               
               else
               {
                  destination_pd = PD_GMM;
               }

               break;

            default:
               MSG_HIGH_DS_0(MM_SUB, "=MM= Wrong SMS preference domain specified .");
         }
         break;

      default:
         MSG_HIGH_DS_0(MM_SUB, "=MM= Wrong UE mode.");
   }

   return destination_pd;
}
#endif /*FEATURE_SMS_ENHANCED_DOMAIN_SELECTION*/
/*===========================================================================

FUNCTION MM_CONNECTION_ESTABLISHMENT

DESCRIPTION
  This function handles the messages relating to connection establishment

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_connection_establishment
(
   mm_cmd_type* msg_p,
   auth_security_context_id security_context
)
{
   boolean                    error_indication;
   cm_service_request_msg_T   cm_service_request_msg;
   mmcc_sync_ind_T            mmcc_sync_ind;
   boolean                    send_sync;
   byte                       *write_ptr = NULL;
   establishment_cause_T      establishment_cause = MM_RR_NO_CNM_SERVICE; /* Default to an unknown value */
   byte                       cnm_service_request_service_type;
   byte                       ciph_key_seq_num;
   byte                       reject_cause = REJ_UNSPECIFIED;
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
   byte                       preferred_route = CS_PS_TEMPORARY;
   boolean                    sms_connection_complete = FALSE;
                                               /*Set to true when SMS domain is chosen to be PS or some error occurs*/
#else
#ifdef FEATURE_DSAC
   byte                       outgoing_route = FALSE;   
#else
   byte                       preferred_route = PD_MM;

#endif
#endif
#ifdef FEATURE_NAS_ECALL
   boolean                    routine_man_selection_success = TRUE;
   boolean                    is_cell_forbidden = FALSE;
   inter_task_lai_T           current_lai;
   mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_NONE); 
#endif
   byte temp_lu_cause = mm_get_lu_cause();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_LTE
   emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
   emm_ctrl_data_ptr = emm_db_get_ctrl_data();
   ASSERT(emm_ctrl_data_ptr != NULL);
#endif

   if ( ( msg_p->cmd.hdr.message_set == MS_CC_MM ) &&
        ( msg_p->cmd.hdr.message_id  == MMCNM_EST_REQ ) )
   {
      /*
      * Sort out the values of establishment cause and CNM_service_type
      *
      *
      * store CNM_service_type
      */
      call_type_for_rrc_est_req = msg_p->cmd.mmcnm_est_req.mm_rrc_call_type;
#if defined(FEATURE_LTE)
      mm_held_cnm_service_req.include_additional_update_ie = FALSE;
#endif
      cnm_service_request_service_type = msg_p->cmd.mmcnm_est_req.CNM_service_type;;

      /* Reject MMCNM_EST_REQ when STOP_MODE command pending */
      if (mm_nreg_req_received
#ifdef FEATURE_DUAL_SIM
                 || mm_abort_dual_switch_pending == SYS_STOP_MODE_REASON_DUAL_SWITCH
#endif
         )
      {
         MSG_HIGH_DS_0(MM_SUB, "=MM= mm_nreg_req_received OR Call request came during stop_mode_dual_switch in progress:Reject MMCNM_EST_REQ");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                                             /*Final argument is added as a default*/
         mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE,MMCC_SMS_CS);
#else
         mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
         return;
      }

      /* Reject SMS when in ALT SCAN */
      if(cnm_service_request_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT &&
          mm_deep_sleep_alt_scan == TRUE)
      {
         MSG_HIGH_DS_0(MM_SUB, "=MM= ALT SCAN: Reject SMS");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                                             /*Final argument is added as a default*/
         mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE,MMCC_SMS_CS);
#else
         mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
         return;
      }
#if defined(FEATURE_LTE)      
      /* Reject SMS when PS Detach is ongoing */
      if(((cnm_service_request_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT) ||
          (cnm_service_request_service_type == SUPPLEMENTARY_SERVICE_ACTIVATION)) &&
         (mm_ps_detach_state != MM_PS_DETACH_STATE_NONE))
      {
         MSG_HIGH_DS_0(MM_SUB, "=MM= PS Detach Ongoing: Reject SMS/Supplement Service");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                                             /*Final argument is added as a default*/
         mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE,MMCC_SMS_CS);
#else
         mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
         return;
      }
#endif

      if((cnm_service_request_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT) &&
         (mm_cnm_srv_est_cause == MM_RR_EMERGENCY_CALL) &&
         ((mm_integrity_protected == FALSE)
#ifdef FEATURE_TDSCDMA
          && (!is_tdscdma_mode() || (is_tdscdma_mode() && gmm_anite_gcf_flag))
#endif /* FEATURE_TDSCDMA */
          ) && 
         (mm_cnm_is_active == TRUE) &&  is_umts_mode())
      {
         MSG_HIGH_DS_2( MM_SUB, "=MM= CNM establish req is rejected, type %d,value %d",
                                         MM_REJECT_CAUSE,MM_REJ_NO_SERVICE) ;

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                                             /*Final argument is added as a default*/
         mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE,MMCC_SMS_CS);
#else
         mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
         return;         
      }


#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
      
      if(cnm_service_request_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT)
      {
         preferred_route = mm_sms_get_enhanced_sms_route();
         MSG_HIGH_DS_1(MM_SUB ,"=MM= Domain on which current SMS is tried is %x, %d, %d",
                                         preferred_route) ;
         switch (preferred_route)
         {
            case PD_GMM:
               mm_sms_is_active = TRUE;
               gmm_handle_connection_management(msg_p) ;
               sms_connection_complete = TRUE;
               break;

            case PD_MM:
               /*Intialize some flag to false.*/
               sms_connection_complete = FALSE;
               break;
            default:
               /*Calculate the domain and whether it is permanent error, and send a rej ind*/
               reject_cause = ((preferred_route & 0x10)>>4) ? MM_REJ_INVALID_SIM : MM_REJ_ACCESS_CLASS_BLOCKED;
               MSG_HIGH_DS_2( MM_SUB, "=MM= CNM establish req is rejected, type %d,value %d",
                                         MM_REJECT_CAUSE,reject_cause) ;

               mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, reject_cause , ((mmcc_domain_name_T)(preferred_route & 0x0F)));
               return;
         }
      }

      if(sms_connection_complete == FALSE)/*This is FALSE iff the current domain to send SMS is CS*/
#else
      /*
       * If PS core network is not mandated, find if message can be sent using 
       * CS core network.
       */
#ifndef FEATURE_DSAC       
      if( mm_sms_preferred_domain_id == PS_PREFERRED || 
          mm_sms_preferred_domain_id == CS_PREFERRED)
      { 
        preferred_route = mm_sms_get_preferable_route();
      }
      if ((cnm_service_request_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT) &&
          ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
           (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)) &&
           (mm_sms_preferred_domain_id == PS_ONLY || preferred_route == PD_GMM))
      {
        mm_sms_is_active = TRUE;
        gmm_handle_connection_management(msg_p) ;
      }
#else
      if (cnm_service_request_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT)
      {
        outgoing_route = mm_sms_get_route();
        if (outgoing_route == FALSE)
        {
          MSG_HIGH_DS_2( MM_SUB, "=MM= CNM establish req is rejected, type %d,value %d",
                                         MM_REJECT_CAUSE,reject_cause) ;

          mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, reject_cause);
          return;
        }
      }

      if (cnm_service_request_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT &&
          outgoing_route == PD_GMM)
      {
        mm_sms_is_active = TRUE;
        gmm_handle_connection_management(msg_p) ;
      }

#endif 
      else
#endif
      {
        /* record possible need to send REL_IND later */
        mm_cnm_is_active = TRUE;

        /*
        * check that its at all possible to make a call anyway
        */
        if ((mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE) ||
            ((mm_serving_plmn.mm_service_state == SERVICE_NOT_AVAILABLE) &&
             (mm_follow_on_request) &&
             ((cnm_service_request_service_type != SHORT_MESSAGE_SERVICE_ESTABLISHMENT) &&
               (cnm_service_request_service_type != SUPPLEMENTARY_SERVICE_ACTIVATION))))
        {
          switch  ( cnm_service_request_service_type )
          {
            case EMERGENCY_CALL_ESTABLISHMENT:
              establishment_cause = MM_RR_EMERGENCY_CALL;
              break;

            case SHORT_MESSAGE_SERVICE_ESTABLISHMENT:
              establishment_cause = MM_RR_SHORT_MESSAGE;
              break;

            case SUPPLEMENTARY_SERVICE_ACTIVATION:
              establishment_cause = MM_RR_SUPPLEMENTARY_SERVICE;
              break;

            case MO_CALL_ESTABLISHMENT_FULL_SPEECH:
              establishment_cause = MM_RR_MO_FULL_SPEECH;

              /*
              * replace with value to be used in CNM_SERVICE_REQUEST message
              */

              cnm_service_request_service_type = MO_CALL_ESTABLISHMENT;
              break;

            case MO_CALL_ESTABLISHMENT_HALF_SPEECH:
              establishment_cause = MM_RR_MO_HALF_SPEECH;
              cnm_service_request_service_type = MO_CALL_ESTABLISHMENT;
              break;

            case MO_CALL_ESTABLISHMENT_DATA_9600:
              establishment_cause = MM_RR_MO_DATA_9600;
              cnm_service_request_service_type = MO_CALL_ESTABLISHMENT;
              break;

            case MO_CALL_ESTABLISHMENT_DATA_4800:
              establishment_cause = MM_RR_MO_DATA_4800;
              cnm_service_request_service_type = MO_CALL_ESTABLISHMENT;
              break;

            case MO_CALL_ESTABLISHMENT_DATA_2400:
              establishment_cause = MM_RR_MO_DATA_2400;
              cnm_service_request_service_type = MO_CALL_ESTABLISHMENT;
              break;

            default:
              MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected cnm_service_type %d", cnm_service_request_service_type, 0,0);

              establishment_cause = MM_RR_MO_FULL_SPEECH;  /* default to speech */
              cnm_service_request_service_type = MO_CALL_ESTABLISHMENT;
              break;
          }

          /*
          * Processing to handle the request from CNM for the establishment of
          * an MM connection. This may be either the first, or a subsequent
          * connection. In both cases, a CNM SERVICE REQUEST is sent to RR,
          * which, for the initial establishment, is via RR_EST_REQ, and for
          * subsequent MM connections, is via RR_DATA_REQ.
          */

          cm_service_request_msg.msg_head.IMH.message_set = MS_MM_RR;
          cm_service_request_msg.msg_head.IMH.message_id  = (int)RR_DATA_REQ;

          cm_service_request_msg.msg_head.skip_pd = PD_MM;
          cm_service_request_msg.msg_head.type = CM_SERVICE_REQUEST;

          ciph_key_seq_num =
                   auth_get_ciphering_key_sequence_number( security_context ) ;

          cm_service_request_msg.cipher_key_seq_no_service_type =
                  (byte)( ((byte)(ciph_key_seq_num) << (byte)(4)) +
                                           cnm_service_request_service_type ) ;

          write_ptr = &cm_service_request_msg.data[0];

          error_indication = FALSE;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
          {
            classmark_params_T classmark_params;
            classmark_params.gsm_supported   = mm_gsm_supported;
            classmark_params.wcdma_supported = (mm_wcdma_supported || mm_tdscdma_supported);
            classmark_params.wcdma_in_use    = is_umts_mode();
            classmark_params.MSCR99          = mm_system_information.mscr;
            classmark_params.as_id           = (sys_modem_as_id_e_type)mm_sub_id;
            gs_read_coded_classmark_2_per_nas_stacks( (classmark_2_ie_T *)write_ptr, &classmark_params,(sys_modem_as_id_e_type)mm_as_id );
          }
#else
          gs_read_coded_classmark_2(
           (classmark_2_ie_T *)write_ptr, mm_gsm_supported,
             (mm_wcdma_supported || mm_tdscdma_supported), is_umts_mode(), mm_system_information.mscr ) ;
#endif

          /* note, NO IEI present */
          /*
          * point write_ptr to the next free byte after the classmark data
          */

          write_ptr += cm_service_request_msg.data[0] + 1;

          /* TO BE IMPLMENTED - Priority Level */
          /* IN REL6 TMSI should be included under extra conditions for EMERGENCY CALLs 
           ( No LU pending && MM state is not LU initiated or Waiting for RR Conn LU ) And 
            update status is UPDATED */
            
          if ( has_valid_tmsi() 
              && ( cnm_service_request_service_type != EMERGENCY_CALL_ESTABLISHMENT ||
                   (  
                     ( mm_location_information.location_update_status == UPDATED)
                     && (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)
                     && ( mm_state != MM_WAIT_FOR_RR_CONNECTION_LU || 
                          temp_lu_cause != MM_LU_CAUSE_NORMAL )
                     && (mm_state != MM_LOCATION_UPDATE_INITIATED )
                     && (mm_pending_lu_request != NORMAL_LU)
                   )
                 )  
             )
          {
            write_ptr += mm_pack_tmsi(write_ptr);
          }
          else
          {
            /*
            * Get IMSI instead
            */

            if (mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
            {
              /* Copy IMSI and advance the pointer */
              write_ptr += mm_pack_imsi(write_ptr);
            }
            else
            {
              if (cnm_service_request_service_type == EMERGENCY_CALL_ESTABLISHMENT)
              {
                    mm_read_imei( write_ptr );
    
                    write_ptr += ( *write_ptr ) + 1;
              }
              else
              {
                error_indication = TRUE;
                reject_cause = MM_REJ_INVALID_SIM;
              }
            }
          }
#if defined(FEATURE_LTE)
          if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
          {          
            *write_ptr = (IEI_ADDITIONAL_UPDATE_PARAM << 4) & 0xF0;
            *write_ptr = *write_ptr | 0x02;
            write_ptr = write_ptr+1;
          }
#endif
        }
        else
        {
          error_indication = TRUE;
          reject_cause = MM_REJ_NO_SERVICE;
        }

        /* If there is no error so far, check the access class restrictions */
        if (!error_indication)
        {
          if (((cs_session_status == MMRRC_ACTIVE) &&
              ((mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
               (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) || 
               (mm_state == MM_CONNECTION_ACTIVE)
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
               ||
               (mm_lcs_active)
#endif
               )) ||
              (is_umts_mode() && 
#ifdef FEATURE_DSAC               
              ((mm_system_information.dsac_present == FALSE) && 
               (ps_session_status != MMRRC_IDLE))
#else
               ( rrc_connection_status == MMRRC_ACTIVE)
#endif
               )
             )
          {
            /* ------------------------------------------------------------- 
            ** Do nothing. By pass access restriction check as CS connection 
            ** for call is already up.
            ** ------------------------------------------------------------- */
          }
          else 
          {    
            mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_NONE);
            switch (cs_cell_access)
            {
              case MM_AS_ACCESS_NORMAL_ONLY:
                if (cnm_service_request_service_type == EMERGENCY_CALL_ESTABLISHMENT)
                {
                  if((is_gsm_mode()) && (!mm_retry_em_call_on_access_blocked_cell))
                  {
                    error_indication = TRUE;
                    reject_cause = MM_REJ_ACCESS_CLASS_BLOCKED;
                    MSG_HIGH_DS_0(MM_SUB, "=MM= Access blocked for Emergency calls");
                  }
                  else
                  {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Access blocked for emergency calls, but allowing");
                }
                }
                break;

              case MM_AS_ACCESS_EMERGENCY_ONLY:
                if (cnm_service_request_service_type != EMERGENCY_CALL_ESTABLISHMENT)
                {
                  error_indication = TRUE;
                  reject_cause = MM_REJ_ACCESS_CLASS_BLOCKED;
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Access blocked for normal calls");
                }
                break;

              case MM_AS_ACCESS_NO_CALLS:
                if ((cnm_service_request_service_type != EMERGENCY_CALL_ESTABLISHMENT) ||
                    ((is_gsm_mode()) && (!mm_retry_em_call_on_access_blocked_cell)))
                {
                  error_indication = TRUE;
                  reject_cause = MM_REJ_ACCESS_CLASS_BLOCKED;
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Access blocked for all calls");
                }
                else
                {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= Access blocked for all calls, but RRC will try to camp on a cell where Em. call can be placed");
                }
                break;

              case MM_AS_ACCESS_ALL_CALLS:
                /* do nothing*/
                break;

              default:
                MSG_ERROR_DS( MM_SUB, "=MM= Wrong cell access value %d in system info",
                                          cs_cell_access,0,0) ;
                break;
            }
          }
        }

        /*
        * Check that we can proceed with connection request
        */

        if ((error_indication == FALSE) && (write_ptr != NULL))
        {
           mm_as_cell_access_status_T cs_cell_access_for_reg = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);

           dword ota_msg_length = (dword)( (byte *)write_ptr -
                            (byte *)&cm_service_request_msg ) - sizeof(IMH_T) ;

           PUT_IMH_LEN( ( unsigned int )( ( ( ( byte * )write_ptr ) -
                                 ( ( byte * ) &cm_service_request_msg ) ) ) -
                                 sizeof( IMH_T ),
                                       &cm_service_request_msg.msg_head.IMH ) ;

           /********************************************
           *** Emergency call establishment request ***
           ********************************************/
           // If NV full_srv_req_in_emerg is enabled, then registration needs to be triggered before emergency call can be placed
           // In that case, do not enter this block, registration will be triggered from following else block.
           // Only if there's a limited acquisition on-going for emergency call after one round of search for full service, then
           // place the emergency call in limited state
           if ((cnm_service_request_service_type == EMERGENCY_CALL_ESTABLISHMENT) && ((full_srv_req_in_emerg == FALSE) ||
                (((mmcoord_pending_reg_message.cmd.hdr.message_set == MS_MM_REG &&
                        (mmcoord_pending_reg_message.cmd.hdr.message_id  ==  (int)MMR_REG_REQ) &&
                        (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)
#ifdef FEATURE_LTE
                    &&  (emm_bit_mask_check(mmcoord_pending_reg_message.cmd.mmr_reg_req.additional_info,
                                        (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE)
#endif
                   ) ||
                        (mm_check_if_cs_cell_access_barred(cs_cell_access_for_reg) == TRUE)) && 
                        (full_srv_req_in_emerg == TRUE))|| 
                 ((mm_serving_plmn.info.sim_state ==SYS_SIM_STATE_CS_INVALID ||  mm_serving_plmn.info.sim_state ==SYS_SIM_STATE_CS_PS_INVALID ||
                    mm_check_forbidden_regional_roaming_nas_lai()) &&
                        (emergency_call_after_lu_in_lpm == TRUE))))
           {
              /* See 24.008 section 4.5.1.5 */
              /* Emergency call is rejected only when there is no cell available */

              if ((mm_state == MM_IDLE) && (mm_idle_substate == MM_NO_CELL_AVAILABLE))
              {
                 MSG_HIGH_DS_0(MM_SUB,"=MM= MM-REJ No cell for emergency call");

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                 mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_NO_CELL_AVAILABLE, MMCC_SMS_CS);
#else
                 mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_NO_CELL_AVAILABLE);
#endif
              }
#if defined(FEATURE_LTE)             
              else if( (mm_ps_detach_type == SYS_PS_DETACH_TYPE_MODE_CHANGE) && ( mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ ))
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= MM-REJ MM is waiting for reg req");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                               /*Final argument is added as a default*/
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE,MMCC_SMS_CS);
#else
                mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
 
               return;
              }
#endif        
              else if(cs_session_status == MMRRC_RELEASE_IN_PROGRESS)
              {
                MSG_HIGH_DS_0(MM_SUB, "=MM= Call rejected as Connection release in progress");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_CS);
#else
                mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
              }
              else if ((mm_state == MM_IDLE) &&(mm_idle_substate == MM_PLMN_SEARCH))
              {
                 if (mm_follow_on_request)
                 {
                    (void)memscpy( &mm_held_cnm_service_req.message, sizeof(cm_service_request_msg_T),
                                  &cm_service_request_msg,
                                  sizeof( cm_service_request_msg ) );

                    mm_held_cnm_service_req.present = TRUE;
                    mm_held_cnm_service_req.establishment_cause = (int)establishment_cause;
                    mm_held_cnm_service_req.security_context = security_context ;

                    /* Cache the establishment cause */
                    mm_cnm_srv_est_cause = establishment_cause;
                 }
                 else
                 {
                    MSG_HIGH_DS_0(MM_SUB,"=MM= MM-REJ No service for emergency call");

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_NO_SERVICE , MMCC_SMS_CS);
#else
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_NO_SERVICE );
#endif
                 }
              }
#ifdef  FEATURE_NO_EMCALL_UNTIL_COMBINED_REG_SUCCESS
              else if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                           (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                           (((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) &&
                               ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)||
                                (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
                                ((gmm_ra_update_type == GMM_PERIODIC_UPDATING) && (mm_gs_association_established)) )) 
                                ||
                             ((gmm_state == GMM_REGISTERED_INITIATED) && 
                               ((gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)||
                                (gmm_attach_type == GMM_GPRS_ATTACH_WHILE_IMSI_ATTACHED))) ))
              {
                 /* ------------------------------------------------------------
                 ** Need to complete the Attach/RAU before attempting CS call request -
                 ** store message in "mm_held_cnm_service_req"
                 ** ---------------------------------------------------------- */
                 (void)memscpy( &mm_held_cnm_service_req.message,sizeof(cm_service_request_msg_T),
                               &cm_service_request_msg,
                               sizeof( cm_service_request_msg ));

                 mm_held_cnm_service_req.present = TRUE;
                 mm_held_cnm_service_req.establishment_cause = (int) establishment_cause;
                 mm_held_cnm_service_req.security_context = security_context ;

                 /* Cache the establishment cause */
                 mm_cnm_srv_est_cause = establishment_cause;
              }
#endif
              /* If LAU should be performed for an emergency call whenever there is
               change of LA then store the call request and proceed with LU */
              else if (mm_state == MM_LOCATION_UPDATE_INITIATED
#if defined(FEATURE_LTE)
                      ||(mm_lai_change_force_lau_for_emergency == TRUE &&
                         mm_state == MM_WAIT_FOR_RR_CONNECTION_LU &&
                         mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
#endif
                      )
              {
                 /* Store message in "mm_held_cnm_service_req" */

                 (void)memscpy( &mm_held_cnm_service_req.message,sizeof(cm_service_request_msg_T),
                               &cm_service_request_msg,
                               sizeof( cm_service_request_msg ) );

                 mm_held_cnm_service_req.present = TRUE;
                 mm_held_cnm_service_req.establishment_cause = (int)establishment_cause;
                 mm_held_cnm_service_req.security_context = security_context;
                 
                 /* Cache the establishment cause */
                 mm_cnm_srv_est_cause = establishment_cause;      
#if defined(FEATURE_LTE)
                 /* If LU is initiated for emergency call then set additional update IE 
                    field to TRUE so as to include this IE in service request after LU is 
                    completed in mm_check_pending_requests function */
                 if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
                 {
                   mm_held_cnm_service_req.include_additional_update_ie = TRUE;
                 }
#endif
              }
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
              else if ((mm_state == MM_CONNECTION_ACTIVE) ||
                       (mm_state == MM_RR_CONNECTION_RELEASE_NOT_ALLOWED))
#else
              else if (mm_state == MM_CONNECTION_ACTIVE)
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
              {
                 /* Cache the establishment cause */
                 mm_cnm_srv_est_cause = establishment_cause;  
                 
                 MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent CM_SERVICE_REQUEST");

                 /* Send the message to RR(C) layer */
                 mm_send_ota_message( (byte*)&cm_service_request_msg, ota_msg_length);

                 mm_state_control( MM_STD_SERVICE_REQUEST_SENT );
              }
              
#ifdef FEATURE_CCBS 
              else if (mm_state == MM_PROCESS_CM_SERVICE_PROMPT)
              {
                 /* MM take the state transition from 8 -> 5 and
                   send CM_SERVICE_REQUEST to n/w */

                 /* Cache the establishment cause */
                 mm_cnm_srv_est_cause = establishment_cause;

                 MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent CM_SERVICE_REQUEST");

                 /* Send the message to RR(C) layer */
                 mm_send_ota_message( (byte*)&cm_service_request_msg, ota_msg_length);

                 mm_state_control( MM_STD_SERVICE_REQUEST_SENT );    
              }
              
#endif /* FEATURE_CCBS */
              else if ((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) ||
                       (mm_state == MM_LOCATION_UPDATE_REJECTED))  
              {
                 MSG_HIGH_DS_1(MM_SUB ,"=MM= EST_REJ: MM state %d unsuitable for EST_REQ", mm_state);

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                 mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_CS);
#else
                 mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_WRONG_STATE );
#endif
              }
              /* CM SERVICE REQ for ecall may be ignored by lower layers and LU continued */
              else if ((mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) &&
                       ((is_gsm_mode()) || (rrc_connection_status == MMRRC_ACTIVE)))
              {
                 MSG_HIGH_DS_0(MM_SUB, "=MM= EST_REJ: MM state 13 and RR(C) state unsuitable");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                 mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_CS);
#else
                 mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE );
#endif
              }
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM 
              else if ((is_gsm_mode()) &&
                        (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                        (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                        (((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)||
                           (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
                           ((gmm_ra_update_type == GMM_PERIODIC_UPDATING) && (mm_gs_association_established)))
                          ||
                          ((gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)||
                           (gmm_attach_type == GMM_GPRS_ATTACH_WHILE_IMSI_ATTACHED))) &&
                        ((gmm_last_signalling_message_type == GMM_ATTACH_COMPLETE) || 
                         (gmm_last_signalling_message_type == GMM_ROUTING_AREA_UPDATE_COMPLETE)) &&
                        (mm_waiting_for_regs_complete_txn  ==  0))
              {
                 MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMCNM_EST_REJ - NAS waiting for RAU/Attach complete txn ");
                 mm_waiting_for_regs_complete_txn++;
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                 mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE, MMCC_SMS_CS);
#else
                 mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
              }
#endif
              else
              {
#if defined(FEATURE_LTE)          
                 if((mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG) &&
                    (mm_timer_status [ TIMER_T3211] != TIMER_STOPPED) &&
                    (temp_lu_cause == MM_LU_CAUSE_NORMAL))
                 {
                   /* If after CSFB call end while performing LU and T3211 is running if user initiates 
                      another emergency call then NAS should perform LU after call end.(CR 343445)*/
                   if((mm_location_information.location_update_status == UPDATED) &&
                    (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0))
                   {
                     mm_pending_lu_request = LTE_IRAT_LU;
                   }
                   /* If LA is changed UE should do LAU after emergency call end */
                   else
                   {
                     mm_pending_lu_request =  NORMAL_LU;
                   }
                 }
                 else
#endif
                 if (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
                 {
                    MSG_HIGH_DS_0(MM_SUB, "=MM= MM marking LU as pending ");
                    if (temp_lu_cause == MM_LU_CAUSE_IMSI_ATTACH)
                    {
                       mm_pending_lu_request =  IMSI_ATTACH;
                    }
                    else if (temp_lu_cause == MM_LU_CAUSE_PERIODIC) 
                    {
                       mm_pending_lu_request =  T3212_EXPIRED;
                    }
                    else if((mm_location_information.location_update_status == UPDATED) &&
                            (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0))
                    {
                       mm_pending_lu_request =  LTE_IRAT_LU;
                    }
                    else
                    {
                       mm_pending_lu_request =  NORMAL_LU;
                    }
                 }
                 MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent CM_SERVICE_REQUEST");

                 if (!mm_follow_on_request)
                 {
                    mm_follow_on_request = TRUE;
                 }

                 (void)memscpy( &mm_held_cnm_service_req.message,sizeof(cm_service_request_msg_T),
                                    &cm_service_request_msg,
                                            sizeof( cm_service_request_msg ) );

                 mm_held_cnm_service_req.present = TRUE;
                 mm_held_cnm_service_req.establishment_cause = (int)establishment_cause;
                 mm_held_cnm_service_req.security_context = security_context ;

                 /* Cache the establishment cause */
                 mm_cnm_srv_est_cause = establishment_cause;                    
#ifdef FEATURE_NAS_ECALL
                 if (mm_ecall_mode_type == MMR_ECALL_MODE_ECALL_ONLY)
                 {
                   if (!(PLMN_MATCH( mm_serving_lai,
                                     mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity ) ||
                          (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
                           sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn))) &&
                          (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                                     SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
                   {
                     routine_man_selection_success = FALSE;
                     MSG_HIGH_DS_0(MM_SUB,"=MM= Different PLMN found during MAN_LIMITED_ACQ");
                   }
                   if ((mm_check_forbidden_national_roaming(&current_lai)) ||
                       (mm_check_forbidden_regional_roaming(&current_lai)) ||
#ifdef FEATURE_DUAL_SIM
                       (reg_sim_per_subs_plmn_forbidden(mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id))
#else
                       (reg_sim_plmn_forbidden(mm_serving_plmn.info.plmn))
#endif
                       )
                   {
                     is_cell_forbidden = TRUE;
                     MSG_HIGH_DS_0(MM_SUB,"=MM= Cell is forbidden");
                   }
                                        
                   if (mm_idle_substate == MM_NO_IMSI)
                   {
                     mm_initiate_rr_connection( MM_RR_EMERGENCY_CALL,
                                                &cm_service_request_msg.msg_head.IMH );

                     mm_state_control( MM_STD_MM_CONNECTION_REQUEST );
                   }
                   else if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED)||
                            (!routine_man_selection_success)||
                            (((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                     SYS_NETWORK_SELECTION_MODE_MANUAL)||
                                (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                     SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))&&
                                (!mm_manual_mode_user_selection_procedure)) ||
                               ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                     SYS_NETWORK_SELECTION_MODE_AUTOMATIC)||
                                (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                                     SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))) &&
                               (is_cell_forbidden)))
                   {
                     mm_initiate_rr_connection( MM_RR_EMERGENCY_CALL,
                                                &cm_service_request_msg.msg_head.IMH );

                     mm_state_control( MM_STD_MM_CONNECTION_REQUEST );

                   }
                   else if (mm_check_if_cs_cell_access_barred(cs_cell_access) == TRUE)
                   {
                     mm_initiate_rr_connection( MM_RR_EMERGENCY_CALL,
                                                &cm_service_request_msg.msg_head.IMH );

                     mm_state_control( MM_STD_MM_CONNECTION_REQUEST );
                   }
                   else if (mm_location_information.location_update_status == UPDATED)
                   {
                     mm_initiate_rr_connection( MM_RR_EMERGENCY_CALL,
                                                &cm_service_request_msg.msg_head.IMH );
                     mm_state_control( MM_STD_MM_CONNECTION_REQUEST );
                   }
                   else if ((mm_location_information.location_update_status == NOT_UPDATED) ||
                           (mm_location_information.location_update_status == UPDATING_DISABLED))
                   {
                     mm_substate_control( MM_STD_UPDATE_ATTEMPTED );

                     mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                   }
                 }
                 else
                 {
#endif
                 mm_initiate_rr_connection( MM_RR_EMERGENCY_CALL,
                                                &cm_service_request_msg.msg_head.IMH );

                 mm_state_control( MM_STD_MM_CONNECTION_REQUEST );

#ifdef FEATURE_NAS_ECALL
                 }
#endif
              }
           }
           /********************************************
           *** Normal call establishment request ***
           ********************************************/
           else
           {
              switch (mm_state)
              {
                 case MM_IDLE:
                    if ((mmcoord_pending_reg_message.cmd.hdr.message_set == MS_MM_REG) &&
                        (mmcoord_pending_reg_message.cmd.hdr.message_id  ==  (int)MMR_REG_REQ) &&
                        (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
                     {
                       MSG_HIGH_DS_0(MM_SUB,"=MM= MM-REJ No service for Normal Call since net sel mode is limited");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                       mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_NO_SERVICE , MMCC_SMS_CS);
#else
                       mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_NO_SERVICE );
#endif
                    }
                    else if(mm_idle_substate == MM_NO_CELL_AVAILABLE)
                    {                    
                       MSG_HIGH_DS_0(MM_SUB,"=MM= MM-REJ No cell for normal call");

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                       mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_NO_CELL_AVAILABLE , MMCC_SMS_CS);
#else
                       mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_NO_CELL_AVAILABLE );
#endif
                    }
#if defined(FEATURE_LTE)             
                    else if( (mm_ps_detach_type == SYS_PS_DETACH_TYPE_MODE_CHANGE) && ( mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ ))
                    {
                        MSG_HIGH_DS_0(MM_SUB, "=MM= MM-REJ MM is waiting for reg req");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                                                                       /*Final argument is added as a default*/
                        mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE,MMCC_SMS_CS);
#else
                        mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
                        return;
                    }
#endif
                    else if(cs_session_status == MMRRC_RELEASE_IN_PROGRESS || (is_umts_mode() && mm_reject_call_on_rai_change == TRUE))
                    {
                      MSG_HIGH_DS_0(MM_SUB, "=MM= Call rejected as Connection release in progress or LAI has changed and waiting for service indication");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_CS);
#else
                      mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
                    }
                    else if ((mm_idle_substate == MM_LIMITED_SERVICE) ||
                             (mm_idle_substate == MM_PLMN_SEARCH))
                    {
                       if (mm_follow_on_request)
                       {
                          (void)memscpy( &mm_held_cnm_service_req.message,sizeof(cm_service_request_msg_T),
                                        &cm_service_request_msg,
                                        sizeof( cm_service_request_msg ) );

                          mm_held_cnm_service_req.present = TRUE;
                          mm_held_cnm_service_req.establishment_cause = (int)establishment_cause;
                          mm_held_cnm_service_req.security_context = security_context ;

                          /* Cache the establishment cause */
                          mm_cnm_srv_est_cause = establishment_cause;                             
                       }
                       else
                       {
                          MSG_HIGH_DS_0(MM_SUB,"=MM= MM-REJ No service for normal call");

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                          mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_NO_SERVICE , MMCC_SMS_CS);
#else
                          mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_NO_SERVICE );
#endif
                       }
                    }
                    else if (mm_idle_substate == MM_NO_IMSI)
                    {
                       MSG_HIGH_DS_0(MM_SUB, "=MM= MM-REJ No service for normal call");

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                       mm_send_mmcnm_est_rej( MM_REJECT_CAUSE,MM_REJ_NO_SERVICE ,MMCC_SMS_CS);
#else
                       mm_send_mmcnm_est_rej( MM_REJECT_CAUSE,MM_REJ_NO_SERVICE );
#endif
                    }
#ifdef FEATURE_NAS_REL10
                   else if ((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                              &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif 
                            )
                   {
                       MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMCNM_EST_REJ due to T3246 Running");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                       mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE, MMCC_SMS_CS);
#else
                       mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
                   }
#endif
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                   else if ((is_gsm_mode()) &&
                             (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                             (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                             (((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)||
                               (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
                               ((gmm_ra_update_type == GMM_PERIODIC_UPDATING) && (mm_gs_association_established)))
                              ||
                              ((gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)||
                               (gmm_attach_type == GMM_GPRS_ATTACH_WHILE_IMSI_ATTACHED))) &&
                             ((gmm_last_signalling_message_type == GMM_ATTACH_COMPLETE) || 
                              (gmm_last_signalling_message_type == GMM_ROUTING_AREA_UPDATE_COMPLETE)) &&
                            (mm_waiting_for_regs_complete_txn  ==  0))
                   {
                      MSG_HIGH_DS_0(MM_SUB, "=MM= Sending MMCNM_EST_REJ - NAS waiting for RAU/Attach complete txn ");
                      mm_waiting_for_regs_complete_txn++;
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE, MMCC_SMS_CS);
#else
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
                   }
#endif
                    else if  ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                                  (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
                                  (((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) &&
                                      ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING)||
                                        (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH) ||
                                        ((gmm_ra_update_type == GMM_PERIODIC_UPDATING) && (mm_gs_association_established)) ) ) 
                                       ||
                                   ((gmm_state == GMM_REGISTERED_INITIATED) && 
                                    ((gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)||
                                    (gmm_attach_type == GMM_GPRS_ATTACH_WHILE_IMSI_ATTACHED)) ) ))
                    {
                       /* ------------------------------------------------------------
                       ** Need to complete the RAU before attempting CS call request -
                       ** store message in "mm_held_cnm_service_req"
                       ** ---------------------------------------------------------- */
                       (void)memscpy( &mm_held_cnm_service_req.message,sizeof(cm_service_request_msg_T),
                                       &cm_service_request_msg,
                                       sizeof( cm_service_request_msg ));

                       mm_held_cnm_service_req.present = TRUE;
                       mm_held_cnm_service_req.establishment_cause = (int) establishment_cause;
                       mm_held_cnm_service_req.security_context = security_context ;

                       /* Cache the establishment cause */
                       mm_cnm_srv_est_cause = establishment_cause;                          
                    }
                    else if ( mm_location_information.location_update_status == UPDATED )
                    {
                      /*
                      ** Reject the Voice Call if UE in not updated in current LAI.
                      */
                      if (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)
                      {
                        MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent CM_SERVICE_REQUEST");

                        /* Cache the establishment cause */
                        mm_cnm_srv_est_cause = establishment_cause;                           

                        if((mm_timer_status [ TIMER_T3211] != TIMER_STOPPED) &&
                            (temp_lu_cause == MM_LU_CAUSE_NORMAL))
                        {
                          mm_pending_lu_request = LTE_IRAT_LU;
                        }

                        mm_initiate_rr_connection( establishment_cause,
                                                        &cm_service_request_msg.msg_head.IMH );

                        mm_state_control( MM_STD_MM_CONNECTION_REQUEST );
                      }
                      else if(cnm_service_request_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT) 
                      {
                        /* need to do a location update attempt first */

                        /* store message in "mm_held_cnm_service_req"  */

                        (void)memscpy( &mm_held_cnm_service_req.message,sizeof(cm_service_request_msg_T),
                                      &cm_service_request_msg,
                                      sizeof( cm_service_request_msg ) );

                        mm_held_cnm_service_req.present = TRUE;
                        mm_held_cnm_service_req.establishment_cause = (int)establishment_cause;
                        mm_held_cnm_service_req.security_context = security_context ;

                        /* Cache the establishment cause */
                        mm_cnm_srv_est_cause = establishment_cause;                          

                        if (!mm_follow_on_request)
                        {
                           mm_follow_on_request = TRUE;
                        }

                       mm_substate_control( MM_STD_UPDATE_ATTEMPTED );

                        if (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE)
                        {
                           /* -----------------------------------------------------------
                           ** The attempt counter is reset when (in case of service state
                           ** ATTEMPTING TO UPDATE) location update is triggered by CM
                           ** sublayer requests (3GPP TS 24.008 4.4.4.5)
                           ** ----------------------------------------------------------- */
                           mm_clear_lu_attempt_counter();
                        }

                        mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                      }
                      else
                      {
                        MSG_HIGH_DS_0(MM_SUB,"=MM= MM-REJ UE not updated in current LA");

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                        mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE, MMCC_SMS_CS);
#else
                        mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
                      }
                    }
                    else if
#ifdef FEATURE_NAS_ECALL
(
#endif
                    (mm_location_information.location_update_status == NOT_UPDATED)
#ifdef FEATURE_NAS_ECALL
                    ||
                    (mm_location_information.location_update_status == UPDATING_DISABLED)
)
#endif                  
                    {
                       /* need to do a location update attempt first */

                       /* store message in "mm_held_cnm_service_req"  */

                       (void)memscpy( &mm_held_cnm_service_req.message,sizeof(cm_service_request_msg_T),
                                     &cm_service_request_msg,
                                     sizeof( cm_service_request_msg ) );

                       mm_held_cnm_service_req.present = TRUE;
                       mm_held_cnm_service_req.establishment_cause = (int)establishment_cause;
                       mm_held_cnm_service_req.security_context = security_context ;

                       /* Cache the establishment cause */
                       mm_cnm_srv_est_cause = establishment_cause;                          

                       if (!mm_follow_on_request)
                       {
                           mm_follow_on_request = TRUE;
                       }

                       mm_substate_control( MM_STD_UPDATE_ATTEMPTED );

                       if (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE)
                       {
                          /* -----------------------------------------------------------
                          ** The attempt counter is reset when (in case of service state
                          ** ATTEMPTING TO UPDATE) location update is triggered by CM
                          ** sublayer requests (3GPP TS 24.008 4.4.4.5)
                          ** ----------------------------------------------------------- */
                          mm_clear_lu_attempt_counter();
                       }

                       mm_initiate_lu( MM_LU_CAUSE_NORMAL );
                    }
                    else
                    {
                       MSG_HIGH_DS_1(MM_SUB,"=MM= MM-REJ, UE update status = %d", mm_location_information.location_update_status);
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                       mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE, MMCC_SMS_CS);
#else
                       mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
                    }
                    break;

                 case MM_CONNECTION_ACTIVE:
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
                 case MM_RR_CONNECTION_RELEASE_NOT_ALLOWED:
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
                    if( (mm_location_information.location_update_status == UPDATED) &&
                         (((mm_cnm_srv_est_cause == MM_RR_EMERGENCY_CALL) &&
                            (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)) ||
                         (mm_cnm_srv_est_cause != MM_RR_EMERGENCY_CALL)))
                    {

                      MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent CM_SERVICE_REQUEST");

                      /* Cache the establishment cause */
                      mm_cnm_srv_est_cause = establishment_cause;   

                      /* Send the message to RR(C) layer */
                      mm_send_ota_message( (byte*)&cm_service_request_msg, ota_msg_length);

                      mm_state_control( MM_STD_SERVICE_REQUEST_SENT );
                    }
                    else 
                    {
                      MSG_HIGH_DS_0(MM_SUB,"=MM= MM-REJ UE not updated in current LA");

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE, MMCC_SMS_CS);
#else
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_NO_SERVICE);
#endif
                    }
                    break;

                 case MM_WAIT_FOR_RR_CONNECTION_LU:
                    /*For UMTS buffer the call request so as to set the FOR bit in open session
                    request when sending LU. In GSM we already sent the LU request 
                    as part of establishing connection so reject the call to CM 
                    so that CM will start silent redial.
                    */
                    if ((cnm_service_request_service_type != SUPPLEMENTARY_SERVICE_ACTIVATION) &&
                        ((is_gsm_mode()) || (rrc_connection_status == MMRRC_ACTIVE)) &&
                        (mm_follow_on_request == FALSE))
                    {
                      MSG_HIGH_DS_0(MM_SUB, "=MM= EST_REJ: MM state 13 and RR state unsuitable");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_CS);
#else
                      mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE );
#endif
                    }
                    else
                    {
                      if (!mm_follow_on_request)
                      {
                         mm_follow_on_request = TRUE;
                      }
  
                      /* store message in "mm_held_cnm_service_req" */
  
                      (void)memscpy( &mm_held_cnm_service_req.message,sizeof(cm_service_request_msg_T),
                                    &cm_service_request_msg,
                                    sizeof( cm_service_request_msg ) );
  
                      mm_held_cnm_service_req.present = TRUE;
                      mm_held_cnm_service_req.establishment_cause = (int)establishment_cause;
                      mm_held_cnm_service_req.security_context = security_context ;
  #if defined(FEATURE_LTE)
                      /* set additional update IE field to TRUE so as to include this IE in service
                                      request after LU is completed in mm_check_pending_requests function */
                      if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
                      {
                        mm_held_cnm_service_req.include_additional_update_ie = TRUE;
                      }
  #endif
  
                      /* Cache the establishment cause */
                      mm_cnm_srv_est_cause = establishment_cause;                       
                    }
                    break;

                 case MM_LOCATION_UPDATE_INITIATED:
                    /* store message in "mm_held_cnm_service_req" */

                    (void)memscpy( &mm_held_cnm_service_req.message,sizeof(cm_service_request_msg_T),
                                  &cm_service_request_msg,
                                  sizeof( cm_service_request_msg ) );

                    mm_held_cnm_service_req.present = TRUE;
                    mm_held_cnm_service_req.establishment_cause = (int)establishment_cause;
                    mm_held_cnm_service_req.security_context = security_context ;

                    /* Cache the establishment cause */
                    mm_cnm_srv_est_cause = establishment_cause;                       
                    break;
                    
#ifdef FEATURE_CCBS
                 case MM_PROCESS_CM_SERVICE_PROMPT:
                    /* Spec 24.008, Sec 4.5.1.3.2 
                    In the state PROCESS CM SERVICE PROMPT the MM sublayer waits for 
                    either the rejection or confirmation of  the recall by the identified 
                    CM entity. Any other requests from the CM entities shall either be 
                    rejected or delayed until this state is left. 
                  
                    We are rejecting the upper layer request */
                       
                    MSG_HIGH_DS_0(MM_SUB,"=MM= Processing of CM SERVICE PROMPT going on:Rejecting EST_REQ");
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_WRONG_STATE , MMCC_SMS_CS);
#else
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE,MM_REJ_WRONG_STATE );
#endif
                    break;
#endif /*FEATURE_CCBS*/

                 default:
                    MSG_HIGH_DS_1(MM_SUB ,"=MM= EST_REJ: MM state %d unsuitable for EST_REQ", mm_state);

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE, MMCC_SMS_CS);
#else
                    mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif
                    break;
              }
           }
        }
        else
        {
           MSG_HIGH_DS_2( MM_SUB, "=MM= CNM establish req is rejected, type %d,value %d",
                                             MM_REJECT_CAUSE,reject_cause) ;

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
           mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, reject_cause, MMCC_SMS_CS);
#else
           mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, reject_cause);
#endif
        } /*  +++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      }
   }
   
   #ifdef FEATURE_CCBS 
   
   else if ( ( msg_p->cmd.hdr.message_set == MS_CC_MM ) &&
             ( msg_p->cmd.hdr.message_id  == MMCNM_PROMPT_RES ) )
   {
      if (msg_p->cmd.mmcnm_prompt_res.supported == FALSE)
      {
        /* spec 24.008 sec 4.5.1.3.2 
        If the mobile station supports "Network Initiated MO 
        CM Connection Request" but the identified CM entity in the 
        mobile station does not provide the associated support, then 
        the mobile station shall send an MM STATUS message with cause 
        "Service option not supported". 
        */
        MSG_HIGH_DS_0(MM_SUB, "=MM= CNM does not support NIMO call");
        mm_send_status(MM_SERVICE_OPTION_NOT_SUPPORTED);
      }
      else 
      {
        if (msg_p->cmd.mmcnm_prompt_res.accept == FALSE)
        {
          /* spec 24.008 sec 4.5.1.3.2
          In the case of a temporary CM problem 
          (eg lack of transaction identifiers) then the mobile station 
          shall send an MM STATUS message with cause 
          "Service option temporarily out of order".  
          */
          MSG_HIGH_DS_0(MM_SUB, "=MM= CNM has temporary problem for supporting NIMO call");
          mm_send_status(MM_SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER);
        }     
        else 
        {
          mm_state_control(MM_STD_CM_SERVICE_PROMPT_ACCEPT);
            
          /* record possible need to send REL_IND later */
          mm_cnm_is_active = TRUE;
        }
      }
   }
   
   #endif /* FEATURE_CCBS */
   
   else if ((msg_p->cmd.hdr.message_set == MS_MM_RR) &&
            (msg_p->cmd.hdr.message_id  == (int)RR_EST_CNF))
   {
   
#ifdef FEATURE_LTE
   /*If connection is established successfully then L2GCCO is over.
     If LU needs to be done after call end, then set the CSFB state 
     to LU pending, otherwise set it to none.*/
   mm_reset_mt_csfb_cco_state();
 
   /* Once we get est_cnf from RR, that means CCO is over. If we havent set 
      the SG association flag on LU start because of CCO then set it to TRUE now.*/
   if((mm_state == MM_WAIT_FOR_RR_CONNECTION_LU &&
       emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_NOT_REQ
#ifdef FEATURE_SGLTE
       && MM_IS_SGLTE_ON_PS_STACK_ID)
#else
      )
#endif
     )
   {
     emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
     MSG_HIGH_DS_0(MM_SUB,"=MM= Set SGs assosicaiton flag - On Recieving EST CNF on RR");
   }
#endif
      MSG_HIGH_DS_0(MM_SUB, "=MM= MM received RR_EST_CNF");

      /*
       * Confirmation of the establishment of an RR connection
       */

      mm_state_control( MM_STD_RR_CONNECTION_ESTABLISHED );

   }
   else if (((msg_p->cmd.hdr.message_set == MS_MM_RR) ||
             ((msg_p->cmd.hdr.message_set == MS_MM_TDSRRC)
#ifdef FEATURE_SEGMENT_LOADING
       &&get_tdscdma_interface()
#endif
)) &&
            (msg_p->cmd.hdr.message_id  == (int)RRC_EST_CNF))
   {
     /* This msg is handled by the coord layer - noop here */
   }

   else if ((msg_p->cmd.hdr.message_set == MS_MM_RR ) &&
            (msg_p->cmd.hdr.message_id  == (int)RR_PAGING_IND))
   {
      MSG_HIGH_DS_0(MM_SUB, "=MM= MM received RR_PAGING_IND");
      
      if ( mm_idle_substate == MM_NORMAL_SERVICE)
      {
        /* 24.008 4.2.2.1 */
        mm_send_paging_response(
          #ifdef FEATURE_LTE_TO_GSM
             msg_p->cmd.rr_paging_ind.paging_type       
          #endif
        );
      }
      else if( mm_idle_substate == MM_ATTEMPTING_TO_UPDATE)
      {
        /* 24.008 4.2.2.2 */
        /* Also respond to TMSI page when MM is UPDATED (in case of LR abort after TMSI reallocation)*/
        if (( msg_p->cmd.rr_paging_ind.paging_type == CS_PAGING_WITH_IMSI) ||
            ((mm_location_information.location_update_status == UPDATED) && 
            (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0)))
        {
          mm_send_paging_response(
          #ifdef FEATURE_LTE_TO_GSM
             msg_p->cmd.rr_paging_ind.paging_type       
          #endif
          );
        }
      }
      else if( mm_idle_substate == MM_LIMITED_SERVICE)
      {
        /* In MM_LIMITED_SERVICE state, 24.008 4.2.2.3 says when paged by IMSI,
               MM may respond to paging */
        if (msg_p->cmd.rr_paging_ind.paging_type == CS_PAGING_WITH_IMSI)
        {
          mm_send_paging_response(
          #ifdef FEATURE_LTE_TO_GSM
             msg_p->cmd.rr_paging_ind.paging_type       
          #endif
          );
        }
      }
      else if( mm_idle_substate == MM_NO_IMSI)
      {
        /* 24.008 4.2.2.4*/
        MSG_HIGH_DS_0(MM_SUB,"=MM= Unexpected Paging message when SIM is invalidated for CS domain");
      }
      else if( mm_idle_substate == MM_PLMN_SEARCH_NORMAL_SERVICE)
      {
        /* 24.008 4.2.2.5 */
        mm_send_paging_response(
          #ifdef FEATURE_LTE_TO_GSM
             msg_p->cmd.rr_paging_ind.paging_type       
          #endif
        );
      }
      else if( mm_idle_substate == MM_PLMN_SEARCH)
      {
        /* 24.008 4.2.2.6 */
        MSG_HIGH_DS_0(MM_SUB,"=MM= Ignoring page");
      }
      else 
      {
        MSG_HIGH_DS_2( MM_SUB, "=MM= Ignoring page in state %d substate %d", mm_state, mm_idle_substate);
      }
   }

   else if (((msg_p->cmd.hdr.message_set == MS_MM_RR) ||
             ((msg_p->cmd.hdr.message_set == MS_MM_TDSRRC)
#ifdef FEATURE_SEGMENT_LOADING
       &&get_tdscdma_interface()
#endif
))&&
            ( msg_p->cmd.hdr.message_id  == (int)RRC_OPEN_SESSION_CNF ) )
   {

      if(msg_p->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_SUCCESS)
      {
         if(msg_p->cmd.rrc_open_session_cnf.cn_domain_id == RRC_CS_DOMAIN_CN_ID)
         {
#ifdef FEATURE_LTE                              
            if((mm_state == MM_WAIT_FOR_RR_CONNECTION_LU &&
               ((emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_NOT_REQ) ||
                (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_FOR_CSFB_RAU))
#ifdef FEATURE_SGLTE
                 && MM_IS_SGLTE_ON_PS_STACK_ID)
#else
              )
#endif
              )
             {
                emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
                MSG_HIGH_DS_0(MM_SUB,"=MM= Set SGs assosicaiton flag - On Recieving Open Session CNF for CS domain on RRC");
             }
#endif
            /*
             * Confirmation of the establishment of an RRC connection
             */
            mm_state_control( MM_STD_RR_CONNECTION_ESTABLISHED );
         }
         else
         {
            MSG_FATAL_DS(MM_SUB, "=MM= Incorrect CS Session Domain ID", 0,0,0);
         }
      }
      else /* RRC_OPEN_SESS_FAILURE */
      {
         if (mm_state == MM_WAIT_FOR_RR_CONNECTION_MM)
         {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_RRC_OPEN_SESSION_FAILURE , MMCC_SMS_CS);
#else
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_RRC_OPEN_SESSION_FAILURE );
#endif
         }
         mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );
         mm_state_control( MM_STD_RR_CONNECTION_ABORTED );
      }
   }
   else if (((msg_p->cmd.hdr.message_set == MS_MM_RR) ||
             ((msg_p->cmd.hdr.message_set == MS_MM_TDSRRC)
#ifdef FEATURE_SEGMENT_LOADING
       &&get_tdscdma_interface()
#endif
)) &&
        ( (msg_p->cmd.hdr.message_id  == (int)RR_SYNC_IND) ||
          (msg_p->cmd.hdr.message_id  == (int)RRC_SYNC_IND)) )

   {

      /*
       * Processing to deal with the SYNC indication received from RR. This may
       * indicate start_ciphering, in which case this event is flagged to MM
       * control, and and establish confirmation is sent to CM. For all the
       * other cases, a SYNC indication is sent to CM.
       */

      if (msg_p->cmd.hdr.message_id == (int)RR_SYNC_IND)
      {
#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
         #error code not present
#else
         MSG_HIGH_DS_2( MM_SUB, "=MM= MM rec'd RR_SYNC_IND reason %d, Ciphering enabled %d",
             msg_p->cmd.rr_sync_ind.sync_reason, TRUE);
#endif


#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
         #error code not present
#endif
         /* if ciphering has started */
         if (msg_p->cmd.rr_sync_ind.sync_reason == MM_RR_CIPHERING  )
         {
            /* In state 5, RR_SYNC_IND with MM_RR_CIPHERING implies CM service accept */
            if ( mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION )
            {
              mm_stop_timer( TIMER_T3230 );
            }

            mm_state_control( MM_STD_CIPHERING_HAS_STARTED );
            /* Send CM_CIPHER_IND indication to CM*/
#ifndef FEATURE_UMTS_VOICE_CIPHERING_IND
            mm_send_cm_cipher_ind(SYS_SRV_DOMAIN_CS_ONLY,TRUE);
#endif
         }
         else
         {
            mmcc_sync_ind.header.message_set = MS_CC_MM;
            mmcc_sync_ind.header.message_id  = MMCC_SYNC_IND;
            PUT_IMH_LEN( sizeof(mmcc_sync_ind_T) - sizeof(IMH_T),
                        &mmcc_sync_ind.header );

            mmcc_sync_ind.channel_mode = msg_p->cmd.rr_sync_ind.channel_mode;

            /* Map the sync reason */
            if (msg_p->cmd.rr_sync_ind.sync_reason == MM_RR_RES_ASS)
            {
               mmcc_sync_ind.reason = (int)GSM_RES_ASSIGNED;
            }
            else if (msg_p->cmd.rr_sync_ind.sync_reason == MM_RR_CHANNEL_MODE_MODIFIED)
            {
               mmcc_sync_ind.reason = (int)GSM_CHANNEL_MODE_MODIFIED;
            }
            else if (msg_p->cmd.rr_sync_ind.sync_reason == MM_RR_HANDOVER)
            {
               mmcc_sync_ind.reason = (int)GSM_GSM_HANDOVER;
            }
            else if (msg_p->cmd.rr_sync_ind.sync_reason == MM_RR_WCDMA_GSM_HO)
            {
               mmcc_sync_ind.reason = (int)WCDMA_GSM_HANDOVER;
            }

            send_sync = TRUE;

            if ((msg_p->cmd.rr_sync_ind.channel_type == FULL_RATE_TRAFFIC) &&
                (msg_p->cmd.rr_sync_ind.channel_mode != SIGNALLING_ONLY))
            {
               mmcc_sync_ind.tch_rate = TCH_FULL_RATE;
               mmcc_sync_ind.subchannel = 0;
            }
            else if ((msg_p->cmd.rr_sync_ind.channel_type == HALF_RATE_TRAFFIC) &&
                     (msg_p->cmd.rr_sync_ind.channel_mode != SIGNALLING_ONLY))
            {
               mmcc_sync_ind.tch_rate = TCH_HALF_RATE;
               mmcc_sync_ind.subchannel = msg_p->cmd.rr_sync_ind.subchannel;
            }
            /*
             * When MM receives RR_SYNC_IND with reason MM_RR_RES_ASS and channel_mode
             * SIGNALLING_ONLY, MM passes it to CNM so that CNM can re-transmit the SMS
             * if required. CNM has to ensure that it doesn't handle this SYNC_IND for 
             * CC protocol.
             */    
            else if((msg_p->cmd.rr_sync_ind.sync_reason == MM_RR_RES_ASS) &&
                   (msg_p->cmd.rr_sync_ind.channel_mode == SIGNALLING_ONLY))
            {
               mmcc_sync_ind.tch_rate      = msg_p->cmd.rr_sync_ind.channel_type;
               mmcc_sync_ind.subchannel    = msg_p->cmd.rr_sync_ind.subchannel;
               mmcc_sync_ind.channel_mode  = SIGNALLING_ONLY;
            }
            else
            {
               send_sync = FALSE;
            }

            /*
             * check that RR isn't requesting a silly rate
             */

            if ( send_sync )
            {
               MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMCC_SYNC_IND");
               mm_send_message( &mmcc_sync_ind.header, GS_QUEUE_MN_CM );
            }
         }
      }/* RR_SYNC_IND */
      else if(msg_p->cmd.hdr.message_id == (int)RRC_SYNC_IND)
      {
         /* --------------------------------------------------------------------
         ** 24.008 4.4.4.9, 4.5.1.2: cases f.1 
         ** The procedure shall be initiated again, if 
         ** ii)No SECURITY MODE COMMAND message and no NAS messages relating 
         ** to the RR connection were received after the OTA msg was transmitted
         ** -------------------------------------------------------------------- */
         if((mm_cs_domain_possible_reest) &&
            ((msg_p->cmd.rrc_sync_ind.cause == RRC_CIPHERING_ESTABLISHED) ||
             (msg_p->cmd.rrc_sync_ind.cause == RRC_INTEGRITY_ESTABLISHED) ||
             (msg_p->cmd.rrc_sync_ind.cause == RRC_INTEGRITY_CIPHERING_ESTABLISHED)))
         {
           mm_cs_domain_possible_reest = FALSE;
         }

         /* if ciphering has started */
         if (msg_p->cmd.rrc_sync_ind.cause == RRC_CIPHERING_ESTABLISHED  )
         {
            /* In state 5, this implies CM service accept */
            if ( mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION )
            {
              mm_stop_timer( TIMER_T3230 );
            }

            mm_state_control( MM_STD_CIPHERING_HAS_STARTED );
         }
         else if ((msg_p->cmd.rrc_sync_ind.cause == RRC_INTEGRITY_ESTABLISHED) ||
                  (msg_p->cmd.rrc_sync_ind.cause == RRC_INTEGRITY_CIPHERING_ESTABLISHED))
         {
            /* In state 5, this implies CM service accept */
            if ( mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION )
            {
              mm_stop_timer( TIMER_T3230 );
            }

            mm_state_control( MM_STD_CIPHERING_HAS_STARTED );
            mm_integrity_protected = TRUE;
         }
#ifdef FEATURE_TC
         else if (msg_p->cmd.rrc_sync_ind.cause == RRC_LOOPBACK_ESTABLISHED  )
         {
            mm_handle_test_control_data(msg_p);
         }
#endif
         else if ((msg_p->cmd.rrc_sync_ind.cause == RRC_RAB_ESTABLISHED) ||
                  (msg_p->cmd.rrc_sync_ind.cause == RRC_HANDOVER))
         {
            if((msg_p->cmd.rrc_sync_ind.rab_id_present == FALSE) ||
               (mm_state == MM_IDLE))       /* Added extra check i.e, MM will ignore sync_ind if MM_state is IDLE */
            {
               MSG_ERROR_DS(MM_SUB, "=MM= RRC_SYNC_IND cause %d rab_id not present or mm_state =%d is IDLE ",
                   msg_p->cmd.rrc_sync_ind.cause,mm_state,0);
            }
            else
            {
                mmcc_sync_ind.header.message_set = MS_CC_MM;
                mmcc_sync_ind.header.message_id  = MMCC_SYNC_IND;
                PUT_IMH_LEN( sizeof(mmcc_sync_ind_T) - sizeof(IMH_T),
                           &mmcc_sync_ind.header );
                /* Map the sync cause */
                if (msg_p->cmd.rrc_sync_ind.cause == RRC_RAB_ESTABLISHED)
                {
                    switch(msg_p->cmd.rrc_sync_ind.rab_info.action)
                    {
                    case RAB_ESTABLISHED:
                       mmcc_sync_ind.reason = (int)WCDMA_RAB_ESTABLISHED;
                       break;

                    case RAB_RELEASED:
                       mmcc_sync_ind.reason = (int)WCDMA_RAB_RELEASED;
                       break;

                    case RAB_RECONFIGURED:
                       mmcc_sync_ind.reason = (int)WCDMA_RAB_RECONFIGURED;
                       break;

                    default:
                        /* Msg is validated, so we never get here */
                       break;

                    }
                }
                else /* RRC_HANDOVER */
                {
                   mmcc_sync_ind.reason = (int)GSM_WCDMA_HANDOVER;
                }

                /* these attributes are only used in GSM */
                mmcc_sync_ind.tch_rate = 0;
                mmcc_sync_ind.subchannel = 0;
                mmcc_sync_ind.channel_mode = DEFAULT_INVALID_CHANNEL_MODE;
#ifdef FEATURE_VOC_AMR_WB 
                if( msg_p->cmd.rrc_sync_ind.rab_info.codec == RRC_CODEC_NB_AMR )
                {
                  mmcc_sync_ind.umts_codec = UMTS_NB_AMR;
                }
                else if( msg_p->cmd.rrc_sync_ind.rab_info.codec == RRC_CODEC_WB_AMR )
                {
                  mmcc_sync_ind.umts_codec = UMTS_WB_AMR;
                }
                else
                {
                  mmcc_sync_ind.umts_codec = UMTS_CODEC_NONE;
                }
#endif   /* end of feature FEATURE_VOC_AMR_WB */
#ifdef FEATURE_CS_VOICE_OVER_HSPA
                if( msg_p->cmd.rrc_sync_ind.rab_info.cs_voice_type == RRC_CS_VOICE_OVER_DCH )
                {
                  mmcc_sync_ind.cs_voice_type = UMTS_WCDMA_CS_VOICE_OVER_DCH;
                }
                else if( msg_p->cmd.rrc_sync_ind.rab_info.cs_voice_type == RRC_CS_VOICE_OVER_HSPA )
                {
                  mmcc_sync_ind.cs_voice_type = UMTS_WCDMA_CS_VOICE_OVER_HSPA;
                }
                else
                {
                  mmcc_sync_ind.cs_voice_type = UMTS_WCDMA_CS_VOICE_OVER_INVALID;
                }
#endif
                /* downcasting a 32bit RRC rab id into a byte sized rab id for CC */
                mmcc_sync_ind.radio_access_bearer_id = (uint8)msg_p->cmd.rrc_sync_ind.rab_info.rab_id;

                MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MMCC_SYNC_IND");
                mm_send_message( &mmcc_sync_ind.header, GS_QUEUE_MN_CM );
            }
         }
      }/* RRC_SYNC_IND */
   }
   else
   {
      if (is_OTA_message(msg_p))
      {
         switch (get_OTA_msg_type(msg_p))
         {

            case CM_SERVICE_ACCEPT:

               /*
                * Processing to deal withe message notifying acceptance
                * of the service request. This is either for the first
                * or subsequent MM connections.  The response is to flag
                * the event
                */

               if ( ( mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION ) ||
                    ( mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION ) ||
                    ( mm_state == MM_REESTABLISHMENT_INITIATED  ) )
               {

                  mm_stop_timer( TIMER_T3230 );

                  mm_state_control( MM_STD_CNM_SERVICE_ACCEPT );
               }
               else
               {
                  mm_send_status( MESSAGE_TYPE_NOT_COMP_PRT_ST );
               }
  
               auth_reset_cache_memory(FALSE, NO_TIMER_EXPIRED, RRC_CS_DOMAIN_CN_ID);
               
               break;

            case CM_SERVICE_REJECT:
            {
               /*
                * Processing to handle a service request rejection. The
                * action depends on the reason for rejection, as detailed
                * in 04.08 4.5.1.1 and 4.5.1.2
                */

               byte event = MM_STD_SERVICE_REJECT;
               
#ifdef FEATURE_NAS_REL10
               byte msg_index;
#endif
               reject_cause =
                  ((cm_service_reject_msg_T *)(&msg_p->cmd))->reject_cause;

#ifdef FEATURE_NAS_REL10
               msg_index = 3;
               mm_t3246_backoff_timer_value = 0;

               if((msg_p->cmd.rr_data_ind.layer3_message[msg_index] == IEI_PER_T3246_VALUE) && (reject_cause == CONGESTTION))
               {
                  mm_t3246_backoff_timer_value = gmm_convert_timer_value_ie_to_usecs(
                                                       (msg_p->cmd.rr_data_ind.layer3_message[msg_index+2]));

                  msg_index = msg_index + msg_p->cmd.rr_data_ind.layer3_message[msg_index+1] + 2;
               }
#endif

#ifdef FEATURE_FEMTO_CSG
               /*if UE is not camped on UMTS on CSG cell then drop the message*/
               if(reject_cause == CSG_NOT_AUTHORIZED && mm_serving_csg_id == SYS_CSG_ID_INVALID)
               {
                 MSG_HIGH_DS_0(MM_SUB,"Dropping service reject #25 received when not camped on femto");
               }
#endif 
 
               mm_stop_timer( TIMER_T3230 );

               if((mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
                  (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION))
               {

                  /* Send a establish rejection to CNM and include the rejection cause */
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                  mm_send_mmcnm_est_rej(OTA_REJECT_CAUSE,reject_cause, MMCC_SMS_CS);
#else
                  mm_send_mmcnm_est_rej(OTA_REJECT_CAUSE,reject_cause);
#endif
               }
               /* reject cause is #4 */
               if ( reject_cause == IMSI_UNKNOWN_IN_VLR )
               {
                  mm_delete_lu_status( NOT_UPDATED );

                  /*------------------------------------------------------------
                   need to schedule a location update when we get back to idle
                   ------------------------------------------------------------*/
                  if ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
                      (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) && 
                      (gmm_state != GMM_NULL && gmm_state != GMM_DEREGISTERED) &&
                         (mm_gs_association_established == TRUE))
                  {
                     gmm_pending_ra_update_type = GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH;
                     gmm_ra_update_pending      = TRUE;
                  }
                  else
                  {
                     mm_pending_lu_request = NORMAL_LU;
                  }
                  event = MM_STD_SERVICE_REJECT_CAUSE_4_6;
               }
               /* reject cause is #6 */
               else if ( reject_cause == ILLEGAL_ME )
               {
                  /* ---------------------------------------------------------
                  ** TS 24.008 4.5.1.1 Reject cause #6: MS deletes any TMSI,
                  ** LAI and ciphering key sequence number in the SIM, changes
                  ** the update status to ROAMING NOT ALLOWED, stores it in the
                  ** SIM and enters MM sublayer state WAIT_FOR_NETWORK_COMMAND.
                  ** The mobile station shall consider the SIM as invalid for
                  ** non-GPRS services until switch-off or the SIM is removed.
                  ** ---------------------------------------------------------*/
                  mm_delete_lu_status( ROAMING_NOT_ALLOWED );
                  mm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_INVALID,
                                                  SYS_SRV_DOMAIN_NO_SRV,
                                                  FALSE,
                                                  SYS_SRV_STATUS_LIMITED,
                                                  TRUE );

                  event = MM_STD_SERVICE_REJECT_CAUSE_4_6;
                  if (mm_reg_waiting_for_reg_cnf)
                  {
                    mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
                    mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                    mm_send_mmr_reg_cnf();
                    mm_reg_waiting_for_reg_cnf = FALSE;
                    mm_ready_to_send_reg_cnf   = FALSE;
                  }
                  else
                  {
                    mmr_service_ind_s_type mmr_service_ind;
                    mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                    mmr_service_ind.service_state = mm_serving_plmn.info;
                    mm_send_mmr_service_ind( &mmr_service_ind );
                  }
               }
#ifdef FEATURE_NAS_REL10
               else if ( reject_cause == CONGESTTION && mm_t3246_backoff_timer_value != 0)
               {           
                     /**----------------------------------------------------------------------------------------
                  **If the CM SERVICE REJECT message is integrity protected, the MS shall start timer T3246 with the value
                  **provided in the T3246 value IE else start the timer T3246 with a random value in the default range of 15-30 mins. 
                  **(24.008 section 4.5.1.1) ------------------------------------------------*/                   
                  if (mm_integrity_protected == TRUE)
                  {
                    mm_start_timer(TIMER_T3246, mm_t3246_backoff_timer_value);
                  }
                  else
                  {
                    mm_t3246_backoff_timer_value = ((15 + mm_get_random_value(15)) * 60 * 1000);
                    mm_start_timer(TIMER_T3246, mm_t3246_backoff_timer_value);
                  }
                  /*saves congestion plmn associated with the timer*/
                  memscpy((void *)&mm_3gpp_t3246_plmn_id, sizeof(sys_plmn_id_s_type),(void *)&(mm_serving_plmn.info.plmn), sizeof(sys_plmn_id_s_type));
                  mm_t3246_backoff_timer_value = 0;
                    
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                  mm_send_mmcnm_est_rej(
                     AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED ,MMCC_SMS_CS);
#else
                  mm_send_mmcnm_est_rej(
                     AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED );
#endif 
               }
#endif

#ifdef FEATURE_FEMTO_CSG
               else if (reject_cause == CSG_NOT_AUTHORIZED)
               {
                 mm_delete_csg_from_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info);
                 event = MM_STD_SERVICE_REJECT_CAUSE_25;
               }
#endif

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
               if (mm_lcs_active)
               {
                  /*-----------------------------------------
                  ** If an LCS is in progress, and the result of the
                  ** CM_SERVICE_REJ is to abort the MM connection,
                  ** MM transitions to state 25 to let the LCS session
                  ** finish
                  **----------------------------------------- */
                  if ((mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) &&
                      (event == MM_STD_SERVICE_REJECT_CAUSE_4_6))
                  {
                     event = MM_STD_LCS_IN_PROGRESS;

                     /*--------------------------------------
                     ** All other MM connections are aborted
                     **--------------------------------------*/
                     mm_send_mmcnm_rel_ind( OTHER_CAUSE, FALSE );
                  }
                  else if ((mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
                           (mm_state == MM_REESTABLISHMENT_INITIATED))
                  {
                     event = MM_STD_LCS_IN_PROGRESS;
                  }
               }
#endif
               mm_state_control( event);
               
               auth_reset_cache_memory(FALSE, NO_TIMER_EXPIRED, RRC_CS_DOMAIN_CN_ID);
               
               break;
            }
#ifdef FEATURE_CCBS
            case CM_SERVICE_PROMPT:
            {
               mm_handle_cm_service_prompt(msg_p);
               break;
            } 
#endif

            default:
               MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected RR_DATA_IND message type %d",
                     msg_p->cmd.rr_data_ind.layer3_message[1], 0,0);
               break;
         }
      }
      else
      {
         MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected message received (set/id) %d",
              msg_p->cmd.hdr.message_set*0x100 + msg_p->cmd.hdr.message_id, 0,0);
      }
   }
}


/*===========================================================================

FUNCTION MM_REESTABLISH_CONNECTION

DESCRIPTION
  This function sends the reestablishment request to the network. A
  check is made that reestablishment is allowed first.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_reestablish_connection(  auth_security_context_id security_context )
{
   boolean                       status_ok;
   cm_reestablishment_req_msg_T  cm_reestablishment_req_msg;
   byte                          *write_ptr;

   status_ok = TRUE;

   if ( mm_system_information.reestablishment_allowed != FALSE )
   {

      cm_reestablishment_req_msg.msg_head.IMH.message_set = MS_MM_RR;
      cm_reestablishment_req_msg.msg_head.IMH.message_id = (int)RR_DATA_REQ;
      cm_reestablishment_req_msg.msg_head.skip_pd = PD_MM;
      cm_reestablishment_req_msg.msg_head.type = CM_REESTABLISHMENT_REQUEST;

      cm_reestablishment_req_msg.cipher_key_seq_no =
                     auth_get_ciphering_key_sequence_number(security_context) ;

      write_ptr = &cm_reestablishment_req_msg.data[0];
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
      {
        classmark_params_T classmark_params;
        classmark_params.gsm_supported   = mm_gsm_supported;
        classmark_params.wcdma_supported = (mm_wcdma_supported || mm_tdscdma_supported);
        classmark_params.wcdma_in_use    = is_umts_mode();
        classmark_params.MSCR99          = mm_system_information.mscr;
        classmark_params.as_id           = (sys_modem_as_id_e_type)mm_sub_id;
        gs_read_coded_classmark_2_per_nas_stacks( (classmark_2_ie_T *)write_ptr, &classmark_params, (sys_modem_as_id_e_type)mm_as_id);
      }
#else
      gs_read_coded_classmark_2( (classmark_2_ie_T *)write_ptr, mm_gsm_supported,
                                  (mm_wcdma_supported || mm_tdscdma_supported), is_umts_mode(), mm_system_information.mscr );
#endif

      write_ptr += cm_reestablishment_req_msg.data[0] + 1;

      if(has_valid_tmsi() 
            && ((mm_cnm_srv_est_cause != MM_RR_EMERGENCY_CALL) ||
                ((mm_location_information.location_update_status == UPDATED) &&
                 (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0))
               )  
        )
      {
         /* Copy TMSI and advance the pointer */
         write_ptr += mm_pack_tmsi(write_ptr);

         /*------------------------------------------------
          If TMSI is used as Mobile Identity, LAI from SIM
          shall be included (Format TV). TS 24.008 9.2.4.1
          ------------------------------------------------*/
         *write_ptr = IEI_LAI;
         write_ptr++;

         (void)memscpy( write_ptr,sizeof( ie_lai_T )-1, mm_location_information.lai, LAI_SIZE );
         write_ptr += LAI_SIZE;
      }
      else
      {
         /*
          * Get IMSI from SIM instead
          */
         if (mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
         {
            /* Copy IMSI and advance the pointer */
            write_ptr += mm_pack_imsi(write_ptr);
         }
         else  if (mm_cnm_srv_est_cause == MM_RR_EMERGENCY_CALL)
         {
            mm_read_imei( write_ptr );
            write_ptr += ( *write_ptr ) + 1;
         }        
         else
         {
            mm_send_mmcc_reest_rej(MM_REJECT_CAUSE,MM_REJ_INVALID_SIM);
            mm_state_control(MM_STD_MMCC_REEST_REJ);
            status_ok = FALSE;
         }
      }


      if (status_ok)
      {

         PUT_IMH_LEN( ( unsigned int)((byte *)write_ptr -
                             (byte *)&cm_reestablishment_req_msg) -
                          sizeof(IMH_T),
                        &cm_reestablishment_req_msg.msg_head.IMH );

         MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent CM_REESTABLISHMENT_REQUEST");

         mm_initiate_rr_connection( MM_RR_CALL_REEST,
                                    (IMH_T *)&cm_reestablishment_req_msg );
        
      }
      else
      {
         MSG_ERROR_DS(MM_SUB, "=MM= MM: Unable to get TMSI or IMSI", 0,0,0);
      }
   }
   else
   {
      /*EMPTY*/
      /* do nothing, reestablishment not allowed */
   }
}


/*===========================================================================

FUNCTION MM_FLAG_REESTABLISHMENT_REQUESTED

DESCRIPTION
  This function flags the request from CNM to reestablish the last
  connection(s). Input parameter is a pointer to MMCNM_REEST_REQ message,
  though not used


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_flag_reestablishment_requested(mm_cmd_type* message)
{
   byte cnm_service_request_service_type = message->cmd.mmcc_reest_req.CNM_service_type;
   MSG_HIGH_DS_1(MM_SUB, "=MM= mm_flag_reestablishment_requested with cnm_service_request_service_type %d ", cnm_service_request_service_type);

   if ( mm_state == MM_WAIT_FOR_REESTABLISH_DECISION )
   {
      mm_cnm_is_active = TRUE;

      switch  ( cnm_service_request_service_type )
      {
        case EMERGENCY_CALL_ESTABLISHMENT:
          mm_cnm_srv_est_cause = MM_RR_EMERGENCY_CALL;
          break;
      
        case SHORT_MESSAGE_SERVICE_ESTABLISHMENT:
          mm_cnm_srv_est_cause = MM_RR_SHORT_MESSAGE;
          break;
      
        case SUPPLEMENTARY_SERVICE_ACTIVATION:
          mm_cnm_srv_est_cause = MM_RR_SUPPLEMENTARY_SERVICE;
          break;
      
        case MO_CALL_ESTABLISHMENT_FULL_SPEECH:
          mm_cnm_srv_est_cause = MM_RR_MO_FULL_SPEECH;
          break;
      
        case MO_CALL_ESTABLISHMENT_HALF_SPEECH:
          mm_cnm_srv_est_cause = MM_RR_MO_HALF_SPEECH;
          break;
      
        case MO_CALL_ESTABLISHMENT_DATA_9600:
          mm_cnm_srv_est_cause = MM_RR_MO_DATA_9600;
          break;
      
        case MO_CALL_ESTABLISHMENT_DATA_4800:
          mm_cnm_srv_est_cause = MM_RR_MO_DATA_4800;
          break;
      
        case MO_CALL_ESTABLISHMENT_DATA_2400:
          mm_cnm_srv_est_cause = MM_RR_MO_DATA_2400;
          break;
      
        default:
          mm_cnm_srv_est_cause = MM_RR_MO_FULL_SPEECH;  /* default to speech */
          break;
      }
      mm_state_control( MM_STD_REESTABLISHMENT_REQUESTED );
   }
   else
   {
      /*EMPTY*/
      /* ignore */
   }
}

#ifdef FEATURE_CCBS 
/*===========================================================================

FUNCTION MM_HANDLE_CM_SERVICE_PROMPT

DESCRIPTION
  This function handles the CM SERVICE REQUEST received from the network.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

 void mm_handle_cm_service_prompt(mm_cmd_type *msg_p)
 {
    byte pd_sapi;

    if ( !mm_supports_NIMO_CM_connection ) 
    {
      MSG_HIGH_DS_0(MM_SUB,"=MM= MS does not support NIMO");
      mm_send_status( MM_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED);
    }
    else 
    {
      if (( mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION ) ||
          ( mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION ))
      {
         mm_send_status( MM_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE ); 
      }
      
      else if ((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) ||
               (mm_state == MM_CONNECTION_ACTIVE))
      {
         pd_sapi = ((cm_service_prompt_msg_T *)(&msg_p->cmd))->pd_sapi;
         mm_send_mmcnm_prompt_ind( pd_sapi);
         mm_state_control( MM_STD_CM_SERVICE_PROMPT_RECEIVED );
      }
      else 
      {
         /* This should not happen */               
         MSG_HIGH_DS_0(MM_SUB,"=MM= CM SERVICE PROMPT received in wrong state - Ignoring");
      }
    }
 }

#endif /* FEATURE_CCBS */

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION  mm_csfb_process_pending_call

DESCRIPTION - This function starts the pending CSFB Calls

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_csfb_process_pending_call(void)
{
  boolean la_pending_after_call = FALSE;
    
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  
  //No CSFB call in pending
  if(mm_csfb_state != MM_CSFB_STATE_MO_CALL_ORIG &&
     mm_csfb_state != MM_CSFB_STATE_MT_CALL_ORIG )
  {
     return;
  }

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  
  ASSERT(emm_ctrl_data_ptr != NULL);

  if(mm_serving_plmn.network_op_mode != NETWORK_OP_MODE_1)
  {
    /* For NMO2 network if ISR is off and LAU not started before call/page 
         response, mark LA pending*/
     if(mm_state == MM_IDLE && 
         mm_idle_substate == MM_NORMAL_SERVICE &&
         emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI
#ifdef FEATURE_SGLTE
                            &&     MM_IS_SGLTE_ON_PS_STACK_ID
#endif
         )
     {
       la_pending_after_call = TRUE; 
     }
  }
  
  /*Start pending MO Call */
  if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
  {      
    MSG_HIGH_DS_0(MM_SUB,"=MM= CSFB: Starting MO call");
#ifdef FEATURE_MODEM_HEAP
    mm_connection_establishment(mm_csfb_service_req_message_ptr,
                             mm_get_current_security_context( RRC_CS_DOMAIN_CN_ID ));     

#else
    mm_connection_establishment(&mm_csfb_service_req_message,
                             mm_get_current_security_context( RRC_CS_DOMAIN_CN_ID ));     
#endif
  }
  /* Send page response only is LAU is not started*/
  else if(mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)
  {
    if(mm_serving_plmn.info.active_rat== SYS_RAT_GSM_RADIO_ACCESS) 
    {
      mm_last_paging_cause = (byte)CS_PAGING;
    }
    else if(mm_serving_plmn.info.active_rat== SYS_RAT_UMTS_RADIO_ACCESS)
    {
     mm_last_paging_cause =(byte)RRC_PAGE_CAUSE_TERM_CONV_CALL;
    }
    if(mm_state == MM_IDLE && 
       mm_idle_substate == MM_NORMAL_SERVICE)
    {
      mm_csfb_send_paging_response();
    }
    else
    {
      mm_start_timer(TIMER_CSMT_GUARD, DEFAULT_TIMEOUT);
      if (is_umts_mode())
      {
        gmm_extend_connection_due_to_mt_csfb = TRUE;
      }
      mm_waiting_for_csmt_response = TRUE;
      MSG_HIGH_DS_0(MM_SUB,"=MM= CSFB: Ignoring MT Page, Setting PS following request");  
    }
  }

  // Extend the CSFB state for CCO if it is for MT call.
  if(emm_ctrl_data_ptr->l2gcco_state == EMM_LTOG_CCO_IN_PROGRESS &&
     mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG &&
     mm_state == MM_WAIT_FOR_NETWORK_COMMAND)
  {
    mm_csfb_set_state(MM_CSFB_STATE_MT_CALL_CCO);
  }
  else if(la_pending_after_call)
  { 
    if(mm_state != MM_IDLE)
    {
      mm_csfb_set_state(MM_CSFB_STATE_LU_PENDING);
    }
    else
    {
      mm_initiate_lu( MM_LU_CAUSE_NORMAL );  
      mm_csfb_set_state(MM_CSFB_STATE_NULL);
    }
  }
  else
  {
    mm_csfb_set_state(MM_CSFB_STATE_NULL);
  }
}
#endif /*FEATURE_LTE*/

