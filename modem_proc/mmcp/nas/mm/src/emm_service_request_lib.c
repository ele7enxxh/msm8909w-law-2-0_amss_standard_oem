
/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_service_request_lib.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/06/12   us      If TIMER_T3346 is running & we get CS emergency call req then return FALSE, so that 
                    it is rejected later for CM to initiate pure limited call. 
12/01/11   SAR     populate emm_failure_type to ESM->CM in failure ind  
08/01/11   Rajesh  T3442 timer related code changes
08/01/11   Rajesh  Support to include EPS bearer context in extended service request
08/22/11   MNK     CR: 301219 Handle CSFB call if TAU is rejected/failed  
07/10/10   MNK     Fix CR  295108 - 1xCSFB:  Load balance connection release received after sending ESR not treated as soft failure 
06/30/11   MNK     Terminate ESR procedure upon HO_FAILED indication from RRC 
04/07/2011 MNK     Add 1xCSFB abort handling 
04/01/2011 MNK     Enhance error handling for CSFB 
12/27/10   MNK     Fix CR: 268979  
12/02/10   MNK     Fix ESR type issue for UL data cnf failures 
11/29/10   MNK     More 1XCSFB changes including adversarial scenarios
11/01/10   MNK     Introduce e1XCSFB 
10/07/10   MNK     1XCSFB introduction 
09/30/10   MNK     pended_esr_msg changed to a pointer 
09/24/10   MNK     Handle ESR & TAU collision
09/23/10   MNK     Handle Extended service reject access startum failures for CSFB flows 
09/20/10   MNK     ESR validation for MT calls added 
09/17/10   MNK     CS_NOTIFICATION for 3GPPCSFB added
08/09/10   MNK     Make code compatible for 1xCSFB & 3GPPCSFB
08/30/10   MNK     CSFB changes
06/29/10   zren    Updated EMM to handle service request procedure and unsent 
                   messages correctly for redirection and reselection  
05/11/10   RI      Mainlined spec changes upto Dec 09.
04/22/10   zren    Updated to use global structure for NAS outgoing message
04/02/10   zren    Added failure handling for RRC connection release causes of
                   CRE_FAILURE and OOS_DURING_CRE 
02/05/10   zren    Updated processing of connection abort in waiting for RRC 
                   confirmation state
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
01/22/10   RI      Added support for handling multiple EPS contexts.
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
11/10/09   RI      Moved the state transition before sending the 
                   Service request message
10/30/09   zren    Updated to not send connection abort request to RRC upon 
                   receipt of non-success rrc conn est cnf in service request 
                   procedure
                   Updated to only send service request failure indication for
                   LTE_RRC_EST_CAUSE_MO_DATA type
09/17/09   zren    Updated to cache esm data req after service request procedure
                   is started in registered state w/o connection
                   Updated to only use ESM transaction ID in service request
                   procedure/send service request failure ind for MO Data type 
                   service request procedure
08/31/09   RI      Added  sim update to RRC with NAS count info if security is valid
08/14/09   zren    Modified procedure connection release handling
07/27/09   zren    Modified EMM to notify ESM the service request failure in 
                   the case of EMM barring timer is still active
07/15/09   zren    Modified message list handling functions to use message 
                   pointer as parameter
06/13/09   RI      Added support for NAS security.
05/27/09   zren    Changed code to not send connection abort req to RRC when 
                   RRC_REL_IND received during Service Request procedure
05/19/09   hnam    Added support for transmission failure scenario - when 
                   connection is released by RRC during any UL procedure failures
05/14/09   zren    Changed to include "emm_esm_handler.h" instead of using 
                   extern function prototype
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
04/14/09   RI      Added cell barring flag to emm_rrc_abort_connection().
04/06/09   zren    Fixed a bug of sending incorrect est cause in service request
04/02/09   zren    Added support for new trnsaction ID format 
03/26/09   RI      Added support for GS Timers in LTE.
03/24/09   zren    Created for service request and paging feature

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "emm.h"
#include "rex.h"
#include "assert.h"
#include "emm_update_lib.h"
#include "emm_database.h"
#include "emm_connection_handler.h"
#include "emm_utility.h"
#include "emm_timer_sigs.h"
#include "lte_nas_common_v.h"
#include "lte_nas.h"
#include "lte_nas_msg_parse.h"
#include "msgr.h"
#include "msgr_lte.h"
#include "sys.h"
#include "sys_plmn_selection_v.h"
#include "emm_rrc_if.h"
#include "emm_sim_handler.h"
#include "esm_emm_msg_if.h"
#include "esm_cm_msg_if.h"
#include "emm_service_request_lib.h"
#include "lte_nas_local_cause.h"
#include "emm_esm_handler.h"
#include "err.h"
#include "mm_v.h"
#include "emm_security.h"
#include "emm_mem.h"
#include "emm_cnm_handler.h"
#include "mm_multimode_common.h"
#include "emm_csfb_handler.h"
#include "stringl.h"

#ifdef FEATURE_DUAL_SIM
#include "subs_prio.h"
#endif

void emm_process_multimode_csfb_failure(emm_ctrl_data_type* emm_ctrl_data_ptr, emm_csfb_failure_cause_type failure_cause_type);
extern emm_pended_msg_list_type *emm_find_pended_esr
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);

rrc_connection_failure_type emm_service_request_procedure_failed ;
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/*===========================================================================
FUNCTION    EMM_SERVICE_REQUEST_FAILED

DESCRIPTION
  This function processes following failures in sending service request message
  - RRC_UL_DATA_CNF with failure causes
  - RRC_CONN_EST_FAILURE
  - RRC_CONN_REL

DEPENDENCIES
  None

RETURN VALUE
  TRUE: TAU is required
  FALSE: TAU is not required

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_service_request_failed
(
  lte_emm_connection_cause_type    cause,
  dword                            trans_id,
  emm_failure_type                 emm_failure_cause,
  lte_nas_tai_lst1_type            tac,
  struct emm_pended_msg_list_tag   *message_ptr,
  emm_ctrl_data_type               *emm_ctrl_data_ptr
)
{
  emm_csfb_rsp                     csfb_rsp;
  ASSERT(message_ptr != (emm_pended_msg_list_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  /*initialize it*/
  memset((void *)&csfb_rsp,0,sizeof(emm_csfb_rsp));

  if( EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED ) 
  {
    switch (cause)
    {
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
        /* 5.6.1.6 Abnormal cases in the UE: h, i */
        MSG_ERROR_DS_0(MM_SUB, "=EMM= Service request failed due to transmission failure");
        /* Note: TAI change indication is not supported so far */
        /* If TAI is changed and it is not in TAI list, abort service request and restart service request after TAU */
        /* Abort and restart service request procedure. Keep the connection and
           do not notify ESM since service request will be restarted */
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE,
                                  FALSE, ESM_LOCAL_CUASE_NONE,
                                  emm_failure_cause);
        if(emm_check_esr_for_ps(emm_ctrl_data_ptr) == TRUE)
        {
          /* Start extended service request procedure for PS */
          emm_start_nas_extended_service_request_procedure(emm_ctrl_data_ptr->service_req_cause,LTE_PACKET_SERVICES_VIA_S1,
                                                         csfb_rsp, NAS_EMM_CSFB_NONE);
        }
        {
          emm_start_nas_service_request_procedure(emm_ctrl_data_ptr, 
                                         emm_ctrl_data_ptr->service_req_cause);
        }
        break ;
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ABORTED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
#ifdef FEATURE_NAS_EAB
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
#endif
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CELL_RESEL:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_SI_FAILURE:
        /* 5.6.1.6 Abnormal cases in the UE: a, b */
        MSG_HIGH_DS_0(MM_SUB, "=EMM= SR failed due to conn est failure");
        /* Abort service request. Keep the connection and notify ESM if 
           necessary */
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
        break ;
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TAI_CHANGED:
        /* 24.301 5.6.1.6 Abnormal cases in the UE i)	
           i)   Transmission failure of SERVICE REQUEST or EXTENDED SERVICE REQUEST message indication with TAI change from lower layers
           If the current TAI is not in the TAI list, the service request procedure shall be aborted to perform the tracking area updating procedure. The "active" flag shall be set in the TRACKING AREA UPDATE REQUEST message. If the service request was initiated for CS fallback or 1xCS fallback, the UE shall send the EXTENDED SERVICE REQUEST message to the MME by using the existing NAS signalling connection after the completion of the tracking area updating procedure.
           If the current TAI is still part of the TAI list, the UE shall restart the service request procedure
        */ 
        if(emm_ctrl_data_ptr->service_req_cause == LTE_RRC_EST_CAUSE_MT_ACCESS)
        {
          emm_ctrl_data_ptr->restart_mt_service_req = TRUE;
        }
        MSG_HIGH_DS_1(MM_SUB, "=EMM= SR failed due to TAI CHANGED, If MT procedure to be restarted = %d",emm_ctrl_data_ptr->restart_mt_service_req);
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
        break;
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
        /* 5.6.1.6 Abnormal cases in the UE: a, b */
        MSG_HIGH_DS_2(MM_SUB,"=EMM= SR failed - conn est rej or failure timer expired, lte_connection_control = %d, ext_wait_time = %d",
                      emm_ctrl_data_ptr->lte_connection_control, emm_ctrl_data_ptr->extended_wait_time);

#ifdef FEATURE_LTE_REL10
        /* 5.6.1.6: abnormal case l
           Process extended wait time from lower layers only when connection establishment
           reject cause is CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT */
        if(cause == CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT)
        {
          (void)emm_handle_extended_wait_time(emm_ctrl_data_ptr);
        }
#endif

        /* Check if RRC DAM feature bit 0 is enabled*/
        if( (emm_ctrl_data_ptr->lte_connection_control & 0x01) == TRUE )
        {
          /* Abort service request with NAS local cause 
             LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE. Keep the connection and
             notify ESM if necessary */
          emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                    TRUE, LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                    emm_failure_cause);
        }
        else
        {
          /* Abort service request. Keep the connection and notify ESM if 
             necessary */
          emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                    TRUE, LTE_NAS_SERVICE_REQ_FAILED,
                                    emm_failure_cause);
        }

        break ;
      case EMM_SRV_REQ_FAILURE_NO_THROTTLE:
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
        break;
      case EMM_SRV_REQ_FAILURE_THROTTLE:
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, LTE_NAS_SERVICE_REQ_FAILED,
                                  emm_failure_cause);
        break ;

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE,
                                  emm_failure_cause);
        break;
#endif 
      case CONN_REL_LTE_RRC_CONN_REL_NORMAL:
      case CONN_REL_LTE_RRC_CONN_REL_RLF:
      case CONN_REL_LTE_RRC_CONN_REL_CRE_FAILURE:
      case CONN_REL_LTE_RRC_CONN_REL_OOS_DURING_CRE:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED:
      case CONN_REL_LTE_RRC_CONN_REL_SIB_READ_ERROR:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS:
        /* Connection release is handled by procedure release indication client 
           for this case. Do nothing here. */
        break;
      default :
        MSG_FATAL_DS(MM_SUB,"=EMM= Illegal SR failure cause %d", cause,0,0) ;
        break ;
    }
  }

  message_ptr->message_sent_indicator = TRUE ;

  return FALSE ;
} /* end of emm_service_request_failed() */

/*===========================================================================
FUNCTION    EMM_PROCESS_ESR_COMPLETE

DESCRIPTION
           ESR procedure completetion

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_esr_complete(emm_ctrl_data_type* emm_ctrl_data_ptr)
{
  ASSERT(emm_ctrl_data_ptr != NULL); 
  if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM=ESR is complete. Stop the T3417_EXT timer");
    emm_ctrl_data_ptr->csfb_mt_call_info_ptr->is_mt_call_recv_in_conn_state = FALSE;
    mm_stop_timer(TIMER_T3417_EXT);
  }
  else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM=ESR is complete. Stop the T3417 timer");
    mm_stop_timer(TIMER_T3417);
    /* Force TAU when UE comes back to LTE after 1XCSFB*/
    //emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
  }

  EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
  if(emm_ctrl_data_ptr->pended_esr_msg != NULL)
  {
   /* Free the memory block */
   emm_modem_mem_free((void *)emm_ctrl_data_ptr->pended_esr_msg, emm_ctrl_data_ptr);
   emm_ctrl_data_ptr->pended_esr_msg = NULL;
  }
  emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_CSFB_NONE;

  /* Reset service request cause and esm trans ID */
  emm_ctrl_data_ptr->service_req_cause = LTE_RRC_EST_CAUSE_MAX;
}

/*===========================================================================
FUNCTION    EMM_SERVICE_REQUEST_RELEASE_INDICATION

DESCRIPTION
  This function aborts service request procedure and sends failure indication 
  to ESM if needed upon connection release during service request procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_service_request_release_indication
(
  lte_emm_connection_cause_type  release_cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr,
  lte_rrc_conn_rel_redir_info_s  redir_info,
  lte_nas_tau_cause_type tau_cause
)
{

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  if( EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED ) 
  {
    if(((emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE)||
        (emm_ctrl_data_ptr->ps_resumption_status == EMM_PS_RESUMPTION_ESR_STATE)) &&
       (redir_info.is_redir == TRUE) &&
       (redir_info.redir_rat == LTE_RRC_REDIR_RAT_GERAN) &&
       (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI))
    {
      emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_PENDING;
      MSG_HIGH_DS_0(MM_SUB, "=EMM= redirection to GSM for ESR set PS data resumption pending");
    }
    else
    {
      emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
    }

#ifdef FEATURE_LTE_REL10
    /* 5.6.1.6: abnormal case l */
    (void)emm_handle_extended_wait_time(emm_ctrl_data_ptr);
#endif

    if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)
    {
      /* Abort service request.Keep the connection and notify ESM if necessary */
      emm_abort_service_request(emm_ctrl_data_ptr, FALSE, TRUE, 
                                LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                emm_failure_cause);
    }
    else
    {
      /* Is this release due to redirection */
      if((redir_info.is_redir == FALSE) || (redir_info.redir_rat == LTE_RRC_REDIR_RAT_LTE))
      {
        if(tau_cause != TAU_CAUSE_LOAD_BALANCING)
        {
          /* Connection release happened while ESR is in progress. This connection release is not a response to the ESR. So, ESR needs to be failed*/
          emm_abort_service_request(emm_ctrl_data_ptr, FALSE, TRUE,
                                    NAS_SIG_CONN_RELEASED, emm_failure_cause);
#ifdef FEATURE_1XSRLTE
          if(((emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == TRUE)
              ||(emm_ctrl_data_ptr->partial_esr == TRUE))
              &&(emm_ctrl_data_ptr->srlte_tau_needed == TRUE))
          {
            emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
          }
#endif
        }
        else
        {
          emm_abort_service_request(emm_ctrl_data_ptr, FALSE, FALSE,
                                    ESM_LOCAL_CUASE_NONE, emm_failure_cause);
          if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB)
          {
#ifdef FEATURE_1XSRLTE
          if(((emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == TRUE)
              ||(emm_ctrl_data_ptr->partial_esr == TRUE))
              &&(emm_ctrl_data_ptr->srlte_tau_needed == TRUE))
            {
              MSG_HIGH_DS_0(MM_SUB, "=EMM= Abort 1X CSFB call due to load balancing TAU");
              emm_abort_service_request(emm_ctrl_data_ptr,FALSE,TRUE,
                                        NAS_SIG_CONN_RELEASED,emm_failure_cause);
            }
            else
#endif
            { 
              emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_1X_CSFB_BUFFERED;
              MSG_HIGH_DS_0(MM_SUB, "=EMM= 1X CSFB call is buffered due to load balancing TAU");
            }
          }
          else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB)
          {
            emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_3GPP_CSFB_BUFFERED;
            MSG_HIGH_DS_0(MM_SUB, "=EMM= 3GPP CSFB call is buffered due to load balancing TAU");
          }
        }
      }
      else
      {
        switch(emm_ctrl_data_ptr->esr_type_in_progress)
        {
          case NAS_EMM_1xCSFB:
            if(redir_info.redir_rat == LTE_RRC_REDIR_RAT_CDMA2000_1xRTT)
            {
              MSG_HIGH_DS_0(MM_SUB, "=EMM=Redirection to 1XRTT received for 1X CSFB");
#ifdef FEATURE_1XSRLTE
              if(((emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == TRUE)
                  &&(emm_ctrl_data_ptr->srlte_tau_needed == TRUE))
                  ||(emm_ctrl_data_ptr->partial_esr == TRUE))
              {
                emm_abort_service_request(emm_ctrl_data_ptr,FALSE,TRUE, 
                                  NAS_SIG_CONN_RELEASED,
                                  emm_failure_cause);
              }
              else
#endif
              {
                emm_process_esr_complete(emm_ctrl_data_ptr);
              }
            }
            else if(redir_info.redir_rat == LTE_RRC_REDIR_RAT_CDMA2000_HRPD)
            {
              MSG_HIGH_DS_0(MM_SUB, "=EMM=Redirection to HRPD received when 1X CSFB call is pending. Aborting ESR procedure for 1XCSFB");
              emm_abort_service_request(emm_ctrl_data_ptr,FALSE,TRUE,
                                        NAS_SIG_CONN_RELEASED,emm_failure_cause);
            }
            break;

          case NAS_EMM_3GPP_CSFB:
            if( (redir_info.redir_rat == LTE_RRC_REDIR_RAT_UTRA_FDD) || 
                (redir_info.redir_rat == LTE_RRC_REDIR_RAT_UTRA_TDD))
            {
#ifdef FEATURE_LTE_REL10
              if((redir_info.cs_fallback_highPriority == TRUE) &&
                 (emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10))
              {
                emm_ctrl_data_ptr->csfb_mt_call_info_ptr->mt_csfb_highPriority = TRUE;
              }
#endif
              MSG_HIGH_DS_2(MM_SUB,"=EMM=Redir to UTRA for 3GPP CSFB high pri bit in redir info %d mm_cs_fallback_highPriority %d",
                            redir_info.cs_fallback_highPriority,
                            emm_ctrl_data_ptr->csfb_mt_call_info_ptr->mt_csfb_highPriority);
            }
            mm_stop_timer(TIMER_T3411);
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
            mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

            break;

          case NAS_EMM_CSFB_NONE:
            MSG_FATAL_DS(MM_SUB,"=EMM=Redirection for =%d CSFB happening when redirection is not in progress",emm_ctrl_data_ptr->esr_type_in_progress,0,0);
            break;
          default: 
            MSG_HIGH_DS_0(MM_SUB, "=EMM=esr type not handled");
            break;
         }
       }
    }
  }
} /* end of emm_service_request_release_indication() */

/*===========================================================================
FUNCTION    EMM_START_NAS_SERVICE_REQUEST_PROCEDURE

DESCRIPTION
  This function starts service request procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_start_nas_service_request_procedure
(
  emm_ctrl_data_type               *emm_ctrl_data_ptr,
  lte_rrc_est_cause_e               service_req_cause
)
{
  dword                           sequence_number = 0 ;
  lte_nas_emm_service_req_type   *emm_service_request  = NULL;
  rex_timer_cnt_type              rem_value = 0;
  emm_failure_type                emm_failure_cause;
  byte index = 0;
  emm_security_context_info *context_ptr = NULL;
  
  emm_state_type  emm_state       =  EMM_INVALID_STATE;
  emm_substate_type emm_substate  =  EMM_UNKNOWN_STATE;
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 

  emm_state    = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();

  /*initialise it*/
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  /* Set service request cause */
  emm_ctrl_data_ptr->service_req_cause = service_req_cause;

  /* Init global out message ptr*/
  memset((void *)emm_ctrl_data_ptr->out_msg_ptr, 0x0, 
         sizeof(lte_nas_outgoing_msg_type)) ;

  emm_service_request = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.service_req;

  /* Fill out the fields of service request message */
  emm_service_request->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES;
  emm_service_request->security_hdr = SERV_REQ ;
  
  /* Note: In here the key set ID of the current context is used and not of the
     native EPS context. Modify this if KSI ASME needs to be used 
  */
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  if( context_ptr != NULL )
  {
    emm_service_request->ksi_seqnum.keyset_id = context_ptr->nasKSI.keyset_id;
    /* the 5 least significant bits of the NAS count is used */
    emm_service_request->ksi_seqnum.sequence_number = context_ptr->nas_ul_cnt[0] & 0x1F ;
    
    MSG_HIGH_DS_3(MM_SUB,"=EMM= SERV_REQ Current context - type %d , with KSI %d , SQN %d,  KASME - including index [0-31]",
                  context_ptr->context_type,
                  emm_service_request->ksi_seqnum.keyset_id,
                  emm_service_request->ksi_seqnum.sequence_number);
    /* Print the KASME */
    for(index=0; index+7< 32; index = index+8)
    {
       MSG_HIGH_DS_8(MM_SUB, "=EMM= 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", 
                   context_ptr->nas_kasme[index],context_ptr->nas_kasme[index+1],context_ptr->nas_kasme[index+2],context_ptr->nas_kasme[index+3],
                   context_ptr->nas_kasme[index+4],context_ptr->nas_kasme[index+5],context_ptr->nas_kasme[index+6],context_ptr->nas_kasme[index+7]);
    }
  }
  else 
  {
    emm_service_request->ksi_seqnum.keyset_id = (uint8)NO_KEY_AVAIALABLE;
    emm_service_request->ksi_seqnum.sequence_number = 0;
    MSG_HIGH_DS_0(MM_SUB, "=EMM=  No Current Context, Security NOT VALID, no KASME send SERV_REQ with KSI 7 and SQN 0 ");

  }
  
  /* Note: This value must be updated by MSG LIB */
  emm_service_request->short_mac_value = 0;

  if(emm_ctrl_data_ptr->esm_srv_req_trans_id == EMM_INVALID_ESM_SRV_REQ_TRANS_ID)
  {
    /* Obtain EMM transaction ID if service request is initiated because of 
       paging or esm data request */
    sequence_number = emm_get_nas_transaction_id();
  }
  else
  {
    /* Use received ESM transaction ID if service request is initiated because 
       of esm service request */
    sequence_number = emm_ctrl_data_ptr->esm_srv_req_trans_id;
  }

  /* Service request message sent. Enter SERVICE_REQUEST_INITIATED_STATE */
  EMM_MOVES_TO_SERVICE_REQUEST_INITIATED_STATE() ;

  /* set delay tolerant and store the value */
  if(emm_evaluate_low_priority_indicator_value(emm_ctrl_data_ptr,service_req_cause) == TRUE)
  {
    emm_ctrl_data_ptr->delay_tolerant_access = TRUE;
    /* Overide the connection est cause to Delay tolerant if the low priority indicator is set to TRUE,
       so conn_est_cause LTE_RRC_EST_CAUSE_EMERGENCY will never get overridden */
    service_req_cause = LTE_RRC_EST_CAUSE_DELAY_TOLERANT_ACCESS;
    emm_ctrl_data_ptr->mo_data_enabled_dt = TRUE;
  }

  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM, 
                                NULL, 
                                emm_service_request_failed, 
                                emm_ctrl_data_ptr))
  {
    if (FALSE == emm_connection_setup(service_req_cause, 
                                      emm_ctrl_data_ptr->out_msg_ptr, 
                                      sequence_number, 
                                      MSGR_NAS_EMM, 
                                      NULL, 
                                      emm_service_request_failed, 
                                      ROUTING_INFO_STMSI,
                                      &rem_value,
                                      emm_ctrl_data_ptr))
    {
      lte_emm_connection_cause_type cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED;

      /* 5.6.1.6 Abnormal cases in the UE: a */
      /* Service request shall not be started. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Srv req not started - access class barring");
      emm_failure_cause.cause_type              = LTE_NAS_IRAT_ACCESS_BARRING;
      emm_failure_cause.access_barring_rem_time = rem_value;
      /* Notify ESM if necessary */
      emm_process_service_req_failure(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                      LTE_NAS_NO_FAILURE,
                                      emm_failure_cause, 
                                      emm_ctrl_data_ptr);

#ifdef FEATURE_NAS_EAB
      if(emm_chk_eab_barring(service_req_cause, emm_ctrl_data_ptr) == TRUE)
      {
         cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB;
      }
#endif

      emm_process_unsent_messages(cause,
                                  emm_failure_cause,
                                  emm_ctrl_data_ptr);

      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_ACCESS_BARRING,
                                   LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                   rem_value,
                                   emm_ctrl_data_ptr);

      /* restore the state/substate since the conenction failed */
      emm_ctrl_data_ptr->emm_state = emm_state;
      emm_ctrl_data_ptr->emm_substate = emm_substate;
      return;
    }
  }
 
  /* Start service request timer (T3417) 5.6.1.2 */
  mm_stop_timer(TIMER_T3417) ;
  mm_start_timer(TIMER_T3417,EMM_T3417_TIMER_DURATION) ;
  MSG_HIGH_DS_0(MM_SUB, "=EMM= T3417 has been started");


  /* Register connection release client for service request procedure.
     The client needs to be removed upon service reject reception, procedure 
     abortion and completion */
  emm_add_new_release_client(emm_service_request_release_indication);

 /* send a SIM update to RRC to start using the latest NAS COUNT
     in KENb calculation, 
     Note: do not move this section of code, since the NAS count is 
     incremented when the message is sent.
     Moved the emm_send_sim_update() to emm_send_msg and emm_connection_setup
 */
  
} /* end of emm_start_service_request_procedure() */


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================
FUNCTION    EMM_PROCESS_SERVICE_REQ_FAILURE

DESCRIPTION
  This function sends failure indication to ESM upon service request procedure
  failure if necessary

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  emm_process_service_req_failure
(
  lte_nas_local_cause_enum_T  local_cause,
  lte_nas_emm_cause_type      emm_cause,
  emm_failure_type            emm_failure_cause,
  emm_ctrl_data_type         *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  if(emm_ctrl_data_ptr->esm_srv_req_trans_id != 
                                              EMM_INVALID_ESM_SRV_REQ_TRANS_ID)
  {    
    /* If there is user data pending, send failure ind to ESM. */
    emm_build_and_send_failure_ind(local_cause, 
                                   emm_cause,
                                   emm_ctrl_data_ptr->esm_srv_req_trans_id,
                                   emm_failure_cause);

    /* Reset esm trans ID */
    emm_ctrl_data_ptr->esm_srv_req_trans_id = EMM_INVALID_ESM_SRV_REQ_TRANS_ID;
  } 

  /* Send rej ind to CNM if SMS is active */
  if(emm_ctrl_data_ptr->emm_sms_active == TRUE)
  {
    if(emm_cause != LTE_NAS_NO_FAILURE)
    {
      emm_send_mmcnm_est_rej(OTA_REJECT_CAUSE, 
                             (byte)emm_cause,
                             emm_ctrl_data_ptr);
    }
    else
    {
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
      if(local_cause == LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE)
      {
        emm_send_mmcnm_est_rej(AS_REJECT_CAUSE, 
                               (byte)AS_REJ_ABORT_RADIO_UNAVAILABLE,
                               emm_ctrl_data_ptr);

      }
      else
#endif 
      {
        emm_send_mmcnm_est_rej(AS_REJECT_CAUSE, 
                               (byte)AS_REJ_LOW_LEVEL_FAIL,
                               emm_ctrl_data_ptr);
      }
    }
  }

  /* Reset service request cause */
  emm_ctrl_data_ptr->service_req_cause = LTE_RRC_EST_CAUSE_MAX;
#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */
}/* emm_process_servie_req_failure */


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================
FUNCTION    EMM_PROCESS_EXTENDED_SERVICE_REQ_FAILURE

DESCRIPTION
  This function sends failure indication to ESM upon extended service request procedure
  failure if necessary

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  emm_process_extended_service_req_failure
(
  emm_csfb_failure_cause_type  failure_cause_type,
  emm_ctrl_data_type         *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  /* Reset service request cause */
  emm_ctrl_data_ptr->service_req_cause = LTE_RRC_EST_CAUSE_MAX;
  if( emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE )
  {
    emm_process_multimode_csfb_failure(emm_ctrl_data_ptr,failure_cause_type);
    emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_CSFB_NONE;
    emm_ctrl_data_ptr->service_req_cause = LTE_RRC_EST_CAUSE_MAX;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= ESR flag is reset and ESR is aborted");
#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
  }

  /* Free the copy of ESR msg in data base */
  if(emm_ctrl_data_ptr->pended_esr_msg != NULL)
  {
    emm_modem_mem_free((void *)emm_ctrl_data_ptr->pended_esr_msg,
                        emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->pended_esr_msg = NULL;
  }
}/* emm_process_extended_servie_req_failure */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================
FUNCTION    EMM_ABORT_SERVICE_REQUEST

DESCRIPTION
  This function aborts service request procedure. It may also abort the RRC 
  connection and may send ESM failure indication upon the value of given 
  flags.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_abort_service_request
(
  emm_ctrl_data_type             *emm_ctrl_data_ptr,
  boolean                         abort_connection,
  boolean                         service_req_failed,
  lte_nas_local_cause_enum_T      local_cause,
  emm_failure_type                emm_failure_cause
)
{
  emm_csfb_failure_cause_type  failure_cause_type;
  failure_cause_type.failure_type = INVALID_REJECT_CAUSE;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)||
     (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)||
     (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED))
  {
    /* Stop timer 3417*/
    mm_stop_timer(TIMER_T3417);
#ifdef FEATURE_LTE_REL10
    emm_ctrl_data_ptr->responding_to_page = FALSE;
#endif
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting service request. T3417 is stopped");
#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
  }
  else
  {
    if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB)
    {
      /* Stop timer 3417*/
      mm_stop_timer(TIMER_T3417_EXT);
      emm_ctrl_data_ptr->csfb_mt_call_info_ptr->is_mt_call_recv_in_conn_state = FALSE;
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting EXTENDED service request for 3GPP CSFB. T3417_EXT is stopped");
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
    }
    else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB)
    {
      /* Stop timer 3417*/
      mm_stop_timer(TIMER_T3417);
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting service request for 1X CSFB. T3417 is stopped");
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
#ifdef FEATURE_1XSRLTE 
     /* Stop timer ESR Delay timer*/
      mm_stop_timer(TIMER_SRLTE_ESR);
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting service request for 1X CSFB. SRLTE ESR timer is stopped");
#endif
    }
  }

  if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->svc_status == 
                                                  LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    /* Enetr EMM_REGISTERED_NORMAL_SERVICE state */
    EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);
  }
  else
  {
    EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
  }

  /* Abort signaling connection if needed */
  if(abort_connection == TRUE)
  {
    emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL) ;
  }

  emm_remove_release_client(emm_service_request_release_indication);

  if(service_req_failed == TRUE)
  {
    if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE ||
       emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED ||
       emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)
    {
      emm_process_service_req_failure(local_cause,
                                      LTE_NAS_NO_FAILURE,
                                      emm_failure_cause, 
                                      emm_ctrl_data_ptr);
    }
    if(emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE)
    {
      failure_cause_type.failure_cause.lte_nas_failure_cause = local_cause; 
      emm_process_extended_service_req_failure(failure_cause_type,emm_ctrl_data_ptr);
    }
#ifdef FEATURE_1XSRLTE
    /*reset TRM priority to low to handle corner case- NW didn't release connection*/
    if(emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == TRUE)
    {
      emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
    }
#endif
#ifdef FEATURE_DUAL_DATA
    if( *emm_ctrl_data_ptr->is_volte_only_ps == TRUE &&
        emm_ctrl_data_ptr->ds_data_priority != SYS_DATA_PRIORITY_HIGH)
    {
       emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
    }
#endif

  }
} /* end of emm_abort_service_request() */

/*===========================================================================
FUNCTION    EMM_COMPLETE_SERVICE_REQUEST

DESCRIPTION
  This function wraps up successful service request procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_complete_service_request
(
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  /* Stop timer 3417*/
  mm_stop_timer(TIMER_T3417);
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Complete service request. T3417 has been stopped");
 
  /* Enetr EMM_REGISTERED_NORMAL_SERVICE state */
  EMM_MOVES_TO_REGISTERED_STATE(
                             (emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE);

  emm_remove_release_client(emm_service_request_release_indication);

  /* Reset service request cause and esm trans ID */
  emm_ctrl_data_ptr->service_req_cause = LTE_RRC_EST_CAUSE_MAX;
  emm_ctrl_data_ptr->esm_srv_req_trans_id = EMM_INVALID_ESM_SRV_REQ_TRANS_ID;

  /* Send est cnf to CNM if SMS is active */
  if(emm_ctrl_data_ptr->emm_sms_active == TRUE)
  {
    emm_send_mmcnm_est_cnf(emm_ctrl_data_ptr);
  }

#ifdef FEATURE_LTE_REL10
  emm_ctrl_data_ptr->responding_to_page = FALSE;
#endif

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Service request procedure has been completed successfully");

} /* end of emm_complete_service_request() */

/*===========================================================================
FUNCTION    EMM_TIMER_T3417_EXPIRE

DESCRIPTION
  This function aborts connection or service request procedure depends on EMM
  connection state upon timer 3417 expiry

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_timer_t3417_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)          
{
  lte_nas_local_cause_enum_T local_cause = LTE_NAS_SERVICE_REQ_FAILED;
  emm_failure_type           emm_failure_cause; 
  emm_connection_state_type  emm_conn_state;
  boolean abort_conn = TRUE;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  emm_conn_state = EMM_GET_CONNECTION_STATE();

  switch(emm_conn_state)
  {
    case EMM_CONNECTED_STATE:
      if( (emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE ) && (emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_1X_CSFB_BUFFERED) && (emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_3GPP_CSFB_BUFFERED)  ) 
      {
        if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB)   
        {
          local_cause = EMM_T3417_EXT_EXPIRED;
          if(emm_ctrl_data_ptr->csfb_mt_call_info_ptr->is_mt_call_recv_in_conn_state &&
             (mm_csfb_get_state() == MM_CSFB_STATE_MT_CALL_ORIG))
          {
            abort_conn = FALSE;
          }
        }
        else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB)
        {
          if(emm_ctrl_data_ptr->esr_est_conn == FALSE)
          {
            abort_conn = FALSE;
            emm_ctrl_data_ptr->esr_est_conn = FALSE;
          }
          local_cause = EMM_T3417_EXPIRED;
        }
        else
        {
          MSG_FATAL_DS(MM_SUB,"=EMM= esr_type_in_progress must be either NAS_EMM_3GPP_CSFB or NAS_EMM_1xCSFB",emm_ctrl_data_ptr->esr_type_in_progress,0,0);
        }
      }
      else
      {
        local_cause = LTE_NAS_SERVICE_REQ_FAILED;
      }
      emm_failure_cause.cause_type = LTE_NAS_IRAT_T3417_EXPIRY;
      /* Abort service request. Abort the connection and notify ESM if necessary */
      emm_abort_service_request(emm_ctrl_data_ptr,abort_conn, 
                                TRUE, local_cause,emm_failure_cause);
      if(emm_ctrl_data_ptr->ps_resumption_status == EMM_PS_RESUMPTION_ESR_STATE)
      {
        emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
      }
      emm_process_unsent_messages(EMM_SRV_REQ_FAILURE_THROTTLE,
                                  emm_failure_cause, 
                                  emm_ctrl_data_ptr);
      break;
    case EMM_WAITING_FOR_RRC_CONFIRMATION_STATE:
      /* Abort the connection. Service request procedure failure will be 
         handled upon receiving rrc est cnf with cause aborted */
      emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
      break;
    case EMM_RELEASING_RRC_CONNECTION_STATE:
       /* Service request procedure was started in releasing connection state.
          RRC_SERVICE_IND never comes. It is possible during L2W IRAT failure.
          Process unsent messages to abort service request procedure and notify
          DS and ESM the failure if necessary */
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB)
      {
        local_cause = EMM_T3417_EXT_EXPIRED;
        /* Abort service request */
        emm_abort_service_request(emm_ctrl_data_ptr,FALSE, 
                                  TRUE,local_cause,emm_failure_cause );
      }
      emm_process_unsent_messages(EMM_SRV_REQ_FAILURE_NO_THROTTLE,
                                  emm_failure_cause, 
                                  emm_ctrl_data_ptr);
      break;
    default:
      MSG_ERROR_DS_1(MM_SUB,"=EMM= T3417 expired in invliad EMM conncection state - %u", 
                     emm_conn_state);
      break;
  }
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
#endif
  /* Process all pending irat uplink messages */
  emm_process_pending_irat_msg(LTE_NAS_IRAT_T3417_EXPIRY,
                               LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                               LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                               emm_ctrl_data_ptr);
} /* end of emm_timer_t3417_expire() */


/*===========================================================================
FUNCTION    EMM_CONVERT_RRC_CAUSE_TO_LOCAL_CAUSE

DESCRIPTION
  This function aborts connection or service request procedure depends on EMM
  connection state upon timer 3417 expiry

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
lte_nas_local_cause_enum_T emm_convert_rrc_cause_to_local_cause(lte_emm_connection_cause_type cause, emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  switch (cause)
  {
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
      return LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_TXN;
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO:
      return LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_HO;
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
      return LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_CONN_REL;
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF:
      return LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_RLF;
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
      return LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN;
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE:
      return LTE_NAS_LRRC_CONN_EST_FAILURE;
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ABORTED:
      return LTE_NAS_LRRC_CONN_EST_FAILURE_ABORTED;         
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
      return LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED;  
#ifdef FEATURE_NAS_EAB
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
      return LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB;
#endif
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CELL_RESEL:
      return LTE_NAS_LRRC_CONN_EST_FAILURE_CELL_RESEL;      
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE:
      return LTE_NAS_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE;  
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE:
      return LTE_NAS_LRRC_CONN_EST_FAILURE_LINK_FAILURE;    
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED:
      return LTE_NAS_LRRC_CONN_EST_FAILURE_NOT_CAMPED;      
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_SI_FAILURE:
      return LTE_NAS_LRRC_CONN_EST_FAILURE_SI_FAILURE;      
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
      return LTE_NAS_LRRC_CONN_EST_FAILURE_CONN_REJECT;  
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
      return  LTE_NAS_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED;
    case CONN_REL_LTE_RRC_CONN_REL_NORMAL:
      return LTE_NAS_LRRC_CONN_REL_NORMAL;       
    case CONN_REL_LTE_RRC_CONN_REL_RLF:
      return LTE_NAS_LRRC_CONN_REL_RLF;                     
    case CONN_REL_LTE_RRC_CONN_REL_CRE_FAILURE:
      return LTE_NAS_LRRC_CONN_REL_CRE_FAILURE;            
    case CONN_REL_LTE_RRC_CONN_REL_OOS_DURING_CRE:
      return LTE_NAS_LRRC_CONN_REL_OOS_DURING_CRE;         
    case CONN_REL_LTE_RRC_CONN_REL_ABORTED:
      return LTE_NAS_LRRC_CONN_REL_ABORTED;                 
    case CONN_REL_LTE_RRC_CONN_REL_SIB_READ_ERROR:
      return   LTE_NAS_LRRC_CONN_REL_SIB_READ_ERROR;         
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
      return LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE;
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TAI_CHANGED:
      return LTE_NAS_LRRC_CONN_EST_FAILURE_TAI_CHANGED;
    default :
      return NAS_SIG_CONN_RELEASED;
      break ;
  }
}

/*===========================================================================
FUNCTION    emm_extended_service_request_failed

DESCRIPTION
  This function processes following failures in sending service request message
  - RRC_UL_DATA_CNF with failure causes
  - RRC_CONN_EST_FAILURE
  - RRC_CONN_REL

DEPENDENCIES
  None

RETURN VALUE
  TRUE: TAU is required
  FALSE: TAU is not required

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_extended_service_request_failed
(
  lte_emm_connection_cause_type    cause,
  dword                            trans_id,
  emm_failure_type                 emm_failure_cause,
  lte_nas_tai_lst1_type            tac,
  struct emm_pended_msg_list_tag   *message_ptr,
  emm_ctrl_data_type               *emm_ctrl_data_ptr
)
{
  emm_csfb_rsp     csfb_rsp;
  emm_esr_type     esr_type = NAS_EMM_CSFB_NONE;
  emm_esr_type     esr_type_in_progress = NAS_EMM_CSFB_NONE;
  lte_nas_local_cause_enum_T local_cause;

  ASSERT(message_ptr != (emm_pended_msg_list_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  if( EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED ) 
  {
    local_cause = emm_convert_rrc_cause_to_local_cause(cause,emm_ctrl_data_ptr);
    switch (cause)
    {
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
        /* 5.6.1.6 Abnormal cases in the UE: h, i */
        MSG_ERROR_DS_0(MM_SUB, "=EMM= EXTENDED Service request failed due to transmission failure");
        /* Note: TAI change indication is not supported so far */
        /* If TAI is changed and it is not in TAI list, abort service request and restart service request after TAU */
        /* Abort and restart service request procedure. Keep the connection and
           do not notify ESM since service request will be restarted */
        /* MNK: For now use NAS_SIG_CONN_RELEASED for all AS(access stratum) failures*/

        /* Save the ESR type in progress as it will be reset while aborting the current attempt*/
        esr_type = emm_ctrl_data_ptr->esr_type_in_progress;
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  FALSE, local_cause, emm_failure_cause);

        /* ESR type can be NONE, if the call is ABORTED before ESR is either completed or failed*/
        if(esr_type != NAS_EMM_CSFB_NONE) 
        {
          csfb_rsp.valid = FALSE;
          if(message_ptr->nas_out_message.emm_outgoing_msg.ext_service_req.service_type == LTE_MT_CSFB_1XCSFB )
          {
            csfb_rsp.valid = TRUE;
            csfb_rsp.csfb_rsp = message_ptr->nas_out_message.emm_outgoing_msg.ext_service_req.csfb_resp;
          }
          emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MO_DATA,message_ptr->nas_out_message.emm_outgoing_msg.ext_service_req.service_type,csfb_rsp, esr_type);
        }
        break ;

       case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
          MSG_ERROR_DS_0(MM_SUB, "=EMM= EXTENDED Service request failed due to transmission failure conn rel");
          /* Buffer ESR message */
          if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB) 
             &&(mm_csfb_get_state() == MM_CSFB_STATE_MT_CALL_ORIG))            
          {
              emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                        FALSE, local_cause, emm_failure_cause);
              emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_3GPP_CSFB_BUFFERED;
          }
          else  // handle 1x csfb as MO call in 3gpp csfb in the same way
          {
             /* Abort service request. Keep the connection and process ESR failure */    
             emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                        TRUE, local_cause, emm_failure_cause);
          }
          break;

      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ABORTED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
#ifdef FEATURE_NAS_EAB
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
#endif
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CELL_RESEL:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_SI_FAILURE:
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
#endif 
        /* 5.6.1.6 Abnormal cases in the UE: a, b */
        MSG_HIGH_DS_0(MM_SUB, "=EMM= EXTENDED Service Request failed due to conn est failure");
        /* Abort service request. Keep the connection and notify ESM if 
           necessary */
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, local_cause, emm_failure_cause);
        emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
        break ;
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
        /* 5.6.1.6 Abnormal cases in the UE: a, b */
        MSG_HIGH_DS_1(MM_SUB, "=EMM= EXTENDED Service Request failed - conn est rej or failure timer expired, ext_wait_time = %d",
                      emm_ctrl_data_ptr->extended_wait_time);

#ifdef FEATURE_LTE_REL10
        /* 5.6.1.6: abnormal case l
           Process extended wait time from lower layers only when connection establishment
           reject cause is CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT */
        if(cause == CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT)
        {
          (void)emm_handle_extended_wait_time(emm_ctrl_data_ptr);
        }
#endif
        /* Abort service request. Keep the connection and notify ESM if 
           necessary */
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, local_cause, emm_failure_cause);
        emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
        break ;
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TAI_CHANGED:
        esr_type = emm_ctrl_data_ptr->esr_type_in_progress;
        if(esr_type == NAS_EMM_3GPP_CSFB)
        {
           esr_type_in_progress = NAS_EMM_3GPP_CSFB_BUFFERED;
        }
#ifdef FEATURE_1XSRLTE 
        else if((esr_type == NAS_EMM_1xCSFB) 
        &&(emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == FALSE) 
        &&(emm_ctrl_data_ptr->partial_esr == FALSE)) 
#else 
        else if(esr_type == NAS_EMM_1xCSFB) 
#endif
        {
           esr_type_in_progress = NAS_EMM_1X_CSFB_BUFFERED;
        }
#ifdef FEATURE_1XSRLTE
        if((emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == TRUE)
        ||(emm_ctrl_data_ptr->partial_esr == TRUE))
        {
          //conclude ESR in SRLTE mode or HVOLTE
          emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, local_cause, emm_failure_cause);
        }
        else
#endif
        {
          emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  FALSE, local_cause, emm_failure_cause);
          emm_ctrl_data_ptr->esr_type_in_progress = esr_type_in_progress;
        }
        emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
        break;
      case CONN_REL_LTE_RRC_CONN_REL_NORMAL:
      case CONN_REL_LTE_RRC_CONN_REL_RLF:
      case CONN_REL_LTE_RRC_CONN_REL_CRE_FAILURE:
      case CONN_REL_LTE_RRC_CONN_REL_OOS_DURING_CRE:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED:
      case CONN_REL_LTE_RRC_CONN_REL_SIB_READ_ERROR:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS:
        /* Connection release is handled by procedure release indication client 
           for this case. Do nothing here. */
        break;
      default :
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Illegal ESR failure cause %d", cause);
        break ;
    }
  }

  message_ptr->message_sent_indicator = TRUE ;

  return FALSE ;
} /* end of emm_extended_service_request_failed() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================
FUNCTION    EMM_CHECK_IF_CSFB_ALLOWED

DESCRIPTION
  This function checks if ESR is allowed/disallowed. If allowed checks to see if ESR can be started or ESR should be buffered 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_check_if_csfb_allowed(lte_csfb_service_type   svc_type, emm_esr_type esr_type, emm_ctrl_data_type  *emm_ctrl_data_ptr,boolean *buffer_esr)
{
  boolean result = FALSE;
  emm_pended_msg_list_type* found_message = NULL;

  emm_state_type emm_state       = EMM_INVALID_STATE;
  emm_substate_type emm_substate = EMM_UNKNOWN_STATE;
  emm_connection_state_type emm_connection_state = EMM_IDLE_STATE;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);
  ASSERT(buffer_esr != NULL) ; 

  emm_state    = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();
  emm_connection_state = EMM_GET_CONNECTION_STATE();
  *buffer_esr = FALSE;
  /*If TIMER_3346 is running return False from here.
       a) Emergency call this call will be rejected later for CM to initiate pure limited CS call.
       b) For Normal call PLMN selection on GW will be initiated*/
#ifdef FEATURE_LTE_REL10
  if (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE 
#ifdef FEATURE_LTE_REL11
      &&
      ((emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL11) ||
       ((emm_check_high_priority(emm_ctrl_data_ptr) == FALSE) &&
        (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)))
#endif
     ) 
  {
    return FALSE;
  }
#endif   
  /* TO DO: First thing to check is if UE is REGISTERED for CS+PS in the case of 3GPP CSFB*/
  if((esr_type == NAS_EMM_3GPP_CSFB) && 
     (((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain != SYS_SRV_DOMAIN_CS_PS) &&
       (emm_state != EMM_TRACKING_AREA_UPDATING_INITIATED)
#ifdef FEATURE_LTE_REL10
      && (svc_type != LTE_MT_CSFB_1XCSFB)  /*If Rel 10, then do not return FALSE if it's MT CSFB*/
#endif
      )
      || (emm_ctrl_data_ptr->reg_req_pending_ptr->sms_only == TRUE)) 
     )
  {
    /* do not start esr if attach was for sms_only */
    MSG_ERROR_DS_2(MM_SUB,"=EMM= can't start ESR for 3GPP SRV domain = %d lte_cs_capability %d",
                   emm_ctrl_data_ptr->srv_domain_registered,
                   emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability);
    return FALSE; 
  }
#ifdef FEATURE_LTE_REL9
  /* With Rel10 changes the following code for T3442 timer will never be hit.
   * UE will reject the call in mm_csfb_start_mo_call_orig function.
   */ 
  /* If T3442 is running and it is not emergency call then reject the ESR procedure */
  if((esr_type == NAS_EMM_3GPP_CSFB) &&
     (mm_timer_status[TIMER_T3442] == TIMER_ACTIVE) &&
     (svc_type == LTE_MO_CSFB_1xCSFB))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= TIMER T3442 is running - ESR cannot be started");
    return FALSE; 
  }
#endif
  if((svc_type == LTE_MT_CSFB_1XCSFB) && 
     (esr_type == NAS_EMM_3GPP_CSFB)  &&
  	!(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_UMTS_RADIO_ACCESS) || 
     mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_GSM_RADIO_ACCESS)  || 
     mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_TDS_RADIO_ACCESS)))
  {

    MSG_HIGH_DS_0(MM_SUB, "=EMM= G,W,T not in rat priority list - ESR cannot be started ");
    return FALSE;
  }
  switch(emm_state)
  { 
    case EMM_REGISTERED:
      switch(emm_substate)
      {
        case EMM_REGISTERED_NORMAL_SERVICE:
          if(mm_timer_status[TIMER_T3440] != TIMER_STOPPED)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= T3440 active - Caching ESR");
            if((svc_type == LTE_MO_CSFB_ECALL_1XCSFB_ECALL) &&
               (emm_ctrl_data_ptr->reg_complete_ack_pending == EMM_PENDING_REG_ACK_NONE))
            {
              /* Abort connection to handle emergency call */
              emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE, 
                                       LTE_RRC_CONN_ABORT_CAUSE_NORMAL) ;
            }
            *buffer_esr = TRUE;
            result = TRUE;  
          }
          else if(((emm_connection_state == EMM_IDLE_STATE) || (emm_connection_state == EMM_CONNECTED_STATE)) &&
                  (emm_ctrl_data_ptr->plmn_service_state->service_status == SYS_SRV_STATUS_SRV))
          {
            result = TRUE;
          }
          else
          {
            MSG_ERROR_DS_2(MM_SUB,"=EMM=EMM is REGISTERED but in improper connection state = %d or service state %d to handle CSFB call and is REJECTED",
                           emm_connection_state,
                           emm_ctrl_data_ptr->plmn_service_state->service_status); 
            result = FALSE;
          }
          break;
        case EMM_REGISTERED_NO_CELL_AVAILABLE:
        case EMM_REGISTERED_ATTEMPTING_TO_UPDATE:
        case EMM_REGISTERED_LIMITED_SERVICE:
        case EMM_REGISTERED_IMSI_DETACH_INITIATED:
          /* Connection state doesn't matter. CSFB call should be rejected*/
          MSG_ERROR_DS_1(MM_SUB, "=EMM=EMM is in state %d and can't handle CSFB call and is REJECTED",
                         emm_substate);
          result = FALSE;
          break;
        case EMM_REGISTERED_PLMN_SEARCH:
          MSG_HIGH_DS_0(MM_SUB, "=EMM= EMM is in EMM_REGISTERED_PLMN_SEARCH state and the request will be buffered and sent out when EMM receives service from RRC & failed if no service from RRC");
          result = TRUE;
          *buffer_esr = TRUE;
          break;
        case EMM_REGISTERED_UPDATE_NEEDED:
          if(svc_type == LTE_MO_CSFB_1xCSFB)
          {               
            result = FALSE;
          }
          else if(svc_type == LTE_MT_CSFB_1XCSFB)
          {
            result = TRUE;
          }
          else
          {
            result = FALSE;
          }
          break;
        default:
          result = FALSE;
          break;
      }
      break;
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      /* for the MT case, only CS_NOTIFICATION is possible in this state. So, let it go and ESR will get buffered and sent out later */
      result = TRUE;
      *buffer_esr = TRUE;
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      break;
    case EMM_SERVICE_REQUEST_INITIATED:
      /* Page indication is the only case due to collision. EMM sends RRC connection est during svc_req procedure & RRC sends page indication that crosses over*/
      /* Ignore the page */
      if(svc_type == LTE_MO_CSFB_1xCSFB)
      {  
        result = TRUE;
        /* This is to cover for scenario where user as initiated an MO call and then
         disconnected it before ESR is complete. We stay in service request 
         initiated state so that we can use the network response. If user
         initiates a new MO call then process the call.
      */
        if((mm_timer_status[TIMER_T3417] == TIMER_STOPPED) &&
         (mm_timer_status[TIMER_T3417_EXT] == TIMER_STOPPED) &&
         (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE))
        {
          *buffer_esr = FALSE;
        }
        else          /* MO call should be bufered */
        {
          *buffer_esr = TRUE;
        }
      }
      else if(svc_type == LTE_MT_CSFB_1XCSFB)
      {
          *buffer_esr = TRUE;
          result = TRUE;
      }
      else if(svc_type == LTE_MO_CSFB_ECALL_1XCSFB_ECALL)
      {
        /* EMERGENCY call should be allowed*/
        result = TRUE;
        if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB)
        {
          /* Stop the timer for the MT CSFB call in progress*/
          mm_stop_timer(TIMER_T3417_EXT);
        }
        else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB)
        {
          /* Stop the timer for the MT CSFB call in progress*/
          mm_stop_timer(TIMER_T3417);
        }
        else if ( (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED) || (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED))
        {
          /* Assuming that the caller of the EMERGENCY CSFB will cancel on their own the previously pending MT CSFB call*/
          /* So, just clean up the buffered message in case it is buffered*/
          if(emm_ctrl_data_ptr->pended_esr_msg != NULL)
          {
            /* Free the memory block */
            emm_modem_mem_free((void *)emm_ctrl_data_ptr->pended_esr_msg, emm_ctrl_data_ptr);
            emm_ctrl_data_ptr->pended_esr_msg = NULL;
          }
        }
        /* Find if there is any ESR in the pended message list. There may be one if UE initiated ESR for a MT CSFB page. */
        found_message =  emm_find_pended_esr(emm_ctrl_data_ptr);
        if(found_message != NULL)
        {                    
          emm_delete_pended_message(found_message, emm_ctrl_data_ptr) ;
        }
      }
      else
      {
        result = FALSE;
      }
      break;
    default:
       MSG_HIGH_DS_1(MM_SUB, "=EMM= emm_state %d not handled", emm_state);
      break;
  }

  return result;
}



/*===========================================================================
FUNCTION    EMM_START_NAS_EXTENDED_SERVICE_REQUEST_PROCEDURE

DESCRIPTION
  This function starts extended service request procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_start_nas_extended_service_request_procedure
(
  lte_rrc_est_cause_e              service_req_cause, 
  lte_csfb_service_type            svc_type, 
  emm_csfb_rsp                     csfb_response,
  emm_esr_type                     esr_type
)
{
  dword                             sequence_number = 0 ;
  lte_nas_emm_extended_service_req  *emm_extended_service_request  = NULL;
  rex_timer_cnt_type                rem_value = 0; 
  boolean                           buffer_esr = FALSE;
  emm_csfb_failure_cause_type       failure_cause_type;
  emm_failure_type                  emm_failure_cause;
  emm_security_context_info *context_ptr = NULL;

#ifdef FEATURE_DUAL_SIM
   sp_mt_call_info mt_call_subs_update = {SP_TECH_MT_CALL_STATE_NONE, SYS_MODEM_AS_ID_NO_CHANGE, SYS_SYS_MODE_MAX};
#endif  

  emm_ctrl_data_type *emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  emm_state_type      emm_state         = EMM_INVALID_STATE;
  emm_substate_type   emm_substate      = EMM_UNKNOWN_STATE; 
   
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 

  emm_state      = EMM_GET_STATE();
  emm_substate   = EMM_GET_SUBSTATE(); 

  /* Set service request cause */
  emm_ctrl_data_ptr->service_req_cause = service_req_cause;

  /* Init out_msg  message */
  memset((void *)emm_ctrl_data_ptr->out_msg_ptr, 0x0, 
         sizeof(lte_nas_outgoing_msg_type)) ;

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  emm_extended_service_request = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.ext_service_req;
  /* Fill out the fields of service request message */
  emm_extended_service_request->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES;
  emm_extended_service_request->lte_nas_hdr.msg_id = EXT_SERVICE_REQUEST;
  
  /* Note: In here the key set ID of the current context is used and not of the
     native EPS context. Modify this if KSI ASME needs to be used 
  */
  /*
  MNK: 
          8  7  6  5                4         3  2  1  
  NAS key set identifier IEI       TSC      NAS key set identifier   (Mandatory for ESR, so only bits 4 3 2 1 apply)
   
  TSC:(Type of Security Context) 
   0      native security context (for KSIASME)
   1      mapped security context (for KSISGSN)
   TSC does not apply for NAS key set identifier value "111".
   
   Service Type: (Mandatory so only bits 4 3 2 1 apply
   Service type value
  Bits
  4  3  2  1  
  0  0  0  0  mobile originating CS fallback or 1xCS fallback
  0  0  0  1  mobile terminating CS fallback or 1xCS fallback
  0  0  1  0  mobile originating CS fallback emergency call or 1xCS fallback emergency call


  */
  
  emm_extended_service_request->service_type = svc_type;
  
  memscpy((void*)&emm_extended_service_request->m_tmsi[0],
         sizeof(emm_extended_service_request->m_tmsi),
         (void*)&emm_ctrl_data_ptr->emm_guti.m_tmsi[0],
         sizeof(emm_ctrl_data_ptr->emm_guti.m_tmsi));

  /* Populate NAS key set identifier */
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  if( context_ptr != NULL )
  {
    /* Find the type of context i.e mapped or native*/
    if(context_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS)
    {
      emm_extended_service_request->nas_key_set_identifier.security_context = 0;
    }
    else if (context_ptr->context_type == CONTEXT_TYPE_MAPPED_EPS)
    {
     emm_extended_service_request->nas_key_set_identifier.security_context = 1;
    } 
    else 
    { 
      MSG_FATAL_DS(MM_SUB,"=EMM= Security Context should be valid at the time of sending ESR. Current Security Contex=%d",context_ptr->context_type,0,0);    
    } 

    emm_extended_service_request->nas_key_set_identifier.keyset_id = context_ptr->nasKSI.keyset_id;
    MSG_HIGH_DS_2(MM_SUB,"=EMM= EXT_SERV_REQ with current context type %d, KSI %d " , 
                  context_ptr->context_type,
                  emm_extended_service_request->nas_key_set_identifier.keyset_id);
  }
  if(csfb_response.valid == TRUE)
  {
    emm_extended_service_request->csfb_resp = csfb_response.csfb_rsp;
  }

  if(emm_ctrl_data_ptr->esm_srv_req_trans_id == EMM_INVALID_ESM_SRV_REQ_TRANS_ID)
  {
    /* Obtain EMM transaction ID if service request is initiated because of 
       paging or esm data request */
    sequence_number = emm_get_nas_transaction_id();
  }
  else
  {
    /* Use received ESM transaction ID if service request is initiated because 
       of esm service request */
    sequence_number = emm_ctrl_data_ptr->esm_srv_req_trans_id;
  }

  if (svc_type != LTE_PACKET_SERVICES_VIA_S1)
  {
     if(emm_check_if_csfb_allowed(svc_type,esr_type, emm_ctrl_data_ptr,&buffer_esr) 
          == TRUE)
     {
     #ifdef FEATURE_1XSRLTE
       if((buffer_esr == TRUE) &&
         ((emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == TRUE)
           ||(emm_ctrl_data_ptr->partial_esr == TRUE))
           &&(esr_type == NAS_EMM_1xCSFB))
       {
         /* we do not buffer esr when UE is in SRLTE mode, simply return failure.*/
         failure_cause_type.failure_type = INVALID_REJECT_CAUSE;
         failure_cause_type.failure_cause.lte_nas_failure_cause = LTE_NAS_SERVICE_REQ_FAILED;
         emm_ctrl_data_ptr->esr_type_in_progress = esr_type;
         //Fail the ESR message & send a failure to the caller
         emm_process_multimode_csfb_failure(emm_ctrl_data_ptr,failure_cause_type);
         emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_CSFB_NONE;
     #ifdef TEST_FRAMEWORK
         #error code not present
#endif /* TEST_FRAMEWORK */
         return;
       }
     #endif
     
       if(emm_ctrl_data_ptr->pended_esr_msg == NULL)
       {
         emm_ctrl_data_ptr->pended_esr_msg = emm_modem_mem_alloc(
                            sizeof(lte_nas_outgoing_msg_type), emm_ctrl_data_ptr);
         mm_check_for_null_ptr((void*)emm_ctrl_data_ptr->pended_esr_msg);
       }
       else
       {
         MSG_HIGH_DS_0(MM_SUB, "=EMM= ESR msg buffer exists");
         /* Reset the memory to reuse it. */
         memset((void *)emm_ctrl_data_ptr->pended_esr_msg, 0x0, 
                sizeof(lte_nas_outgoing_msg_type));
       }
     
       /*Buffer ESR message to be sent later*/
       *(emm_ctrl_data_ptr->pended_esr_msg) = *(emm_ctrl_data_ptr->out_msg_ptr);
     
       if(buffer_esr == TRUE)
       {
         if(esr_type == NAS_EMM_3GPP_CSFB)
         {
           emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_3GPP_CSFB_BUFFERED;
         }
         else if(esr_type == NAS_EMM_1xCSFB)
         {
           emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_1X_CSFB_BUFFERED;
         }
     #if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
         emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
     #endif 
         return;
       }
     }
     else
     {
       emm_ctrl_data_ptr->esr_type_in_progress = esr_type;
       if(EMM_GET_CONNECTION_STATE() == EMM_RELEASING_RRC_CONNECTION_STATE &&
          emm_substate == EMM_REGISTERED_NORMAL_SERVICE)
       {
         /* 1x would like to consider this as soft failure */
         failure_cause_type.failure_type = AS_REJECT_CAUSE;
         if ((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB) 
             || (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED) )
   	  {
           failure_cause_type.failure_cause.lte_nas_failure_cause = LTE_NAS_LRRC_CONN_EST_FAILURE_NOT_CAMPED; 
   	  }
   	  else
   	  {
           failure_cause_type.failure_cause.lte_nas_failure_cause = LTE_NAS_LRRC_CONN_REL_NORMAL;
   	  }
       }
       else
       {
         /* This is to tell that the failure is not 1X related*/
         failure_cause_type.failure_type = INVALID_REJECT_CAUSE;
         failure_cause_type.failure_cause.lte_nas_failure_cause = LTE_NAS_SERVICE_REQ_FAILED;
       }
       //Fail the ESR message & send a failure to the caller
       emm_process_multimode_csfb_failure(emm_ctrl_data_ptr,failure_cause_type);
       emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_CSFB_NONE;
   #ifdef TEST_FRAMEWORK
       #error code not present
#endif /* TEST_FRAMEWORK */
       return;
     }
  }
#ifdef FEATURE_LTE_REL9
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
  {
    emm_extended_service_request->eps_bearer_context_status_exists = TRUE;
    emm_extended_service_request->eps_bearer_context_status = 
                                       emm_ctrl_data_ptr->eps_bearer_context_status;
  }
#endif

  /* Service request message sent. Enter SERVICE_REQUEST_INITIATED_STATE */
  EMM_MOVES_TO_SERVICE_REQUEST_INITIATED_STATE() ;

  if(emm_evaluate_low_priority_indicator_value(emm_ctrl_data_ptr,service_req_cause) == TRUE) 
  {
    emm_extended_service_request->low_priority_indicator = TRUE;
    if (svc_type == LTE_PACKET_SERVICES_VIA_S1) 
    {
      emm_ctrl_data_ptr->delay_tolerant_access = TRUE;
      service_req_cause = LTE_RRC_EST_CAUSE_DELAY_TOLERANT_ACCESS;
      emm_ctrl_data_ptr->mo_data_enabled_dt = TRUE;
    }
    else
    {
      emm_ctrl_data_ptr->delay_tolerant_access = FALSE;
    }
  }
  emm_ctrl_data_ptr->esr_type_in_progress = esr_type;

  if(svc_type == LTE_PACKET_SERVICES_VIA_S1)
  {
    emm_service_request_procedure_failed = (rrc_connection_failure_type)emm_service_request_failed;
  }
  else
  {
    emm_service_request_procedure_failed = (rrc_connection_failure_type)emm_extended_service_request_failed;
  }

  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM, 
                                NULL, 
                                emm_service_request_procedure_failed, 
                                emm_ctrl_data_ptr))
  {
    if (FALSE == emm_connection_setup(service_req_cause, 
                                      emm_ctrl_data_ptr->out_msg_ptr, 
                                      sequence_number, 
                                      MSGR_NAS_EMM, 
                                      NULL, 
                                      emm_service_request_procedure_failed, 
                                      ROUTING_INFO_STMSI,
                                      &rem_value,
                                      emm_ctrl_data_ptr))
    {
      lte_emm_connection_cause_type cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED;

      /* 5.6.1.6 Abnormal cases in the UE: a */
      /* Service request shall not be started. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Srv req not started - access class barring");
      failure_cause_type.failure_type = AS_REJECT_CAUSE;
      emm_failure_cause.cause_type              = LTE_NAS_IRAT_ACCESS_BARRING;
      emm_failure_cause.access_barring_rem_time = rem_value;
      failure_cause_type.failure_cause.lte_nas_failure_cause = 
                                   LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED;

#ifdef FEATURE_NAS_EAB
      if(emm_chk_eab_barring(service_req_cause, emm_ctrl_data_ptr) == TRUE)
      {
        cause = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB;
        failure_cause_type.failure_cause.lte_nas_failure_cause = 
                                     LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB;
      }
#endif
      if(svc_type == LTE_PACKET_SERVICES_VIA_S1)
      {
        emm_process_service_req_failure(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                      LTE_NAS_NO_FAILURE,
                                      emm_failure_cause, 
                                      emm_ctrl_data_ptr);
      }
      else
      {
        emm_process_extended_service_req_failure(failure_cause_type , 
                                               emm_ctrl_data_ptr);
      }
      emm_process_unsent_messages(cause,
                                  emm_failure_cause,
                                  emm_ctrl_data_ptr);

      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_ACCESS_BARRING,
                                   LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                   rem_value,
                                   emm_ctrl_data_ptr);

      /* restore the state/substate since the conenction failed */
      emm_ctrl_data_ptr->emm_state = emm_state;
      emm_ctrl_data_ptr->emm_substate = emm_substate;
      return;
    }

    if( esr_type == NAS_EMM_1xCSFB )
    {
      emm_ctrl_data_ptr->esr_est_conn = TRUE;
      MSG_HIGH_DS(MM_SUB,"=EMM= ESR is sent in new connection",0,0,0);
    }
  }
 
  if(esr_type == NAS_EMM_3GPP_CSFB)
  {
    /* Start service request timer (T3417) 5.6.1.2 */
    mm_stop_timer(TIMER_T3417_EXT) ;
    mm_start_timer(TIMER_T3417_EXT,EMM_T3417_EXT_TIMER_DURATION) ;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= T3417_EXT has been started for 3GPP CSFB");
  }
  else if(esr_type == NAS_EMM_1xCSFB)
  {
#ifdef FEATURE_1XSRLTE
    if(( svc_type == LTE_MO_CSFB_1xCSFB || svc_type == LTE_MT_CSFB_1XCSFB || svc_type == LTE_MO_CSFB_ECALL_1XCSFB_ECALL)&&
       ((emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == TRUE)
        ||(emm_ctrl_data_ptr->partial_esr == TRUE)))
    {
      /* Start 1XSRLTE ESR  timer */
      mm_start_timer(TIMER_SRLTE_ESR,emm_ctrl_data_ptr->srlte_esr_time) ;
      MSG_HIGH_DS_0(MM_SUB, "=EMM= SRLTE ESR timer has been started for 1X SRLTE");
      //delay TAU flag
      emm_ctrl_data_ptr->srlte_tau_needed = TRUE;
    }
    else
#endif
    {
    /* Start service request timer (T3417) 5.6.1.2 */
    mm_stop_timer(TIMER_T3417) ;
    mm_start_timer(TIMER_T3417,EMM_T3417_TIMER_DURATION) ;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= T3417 has been started for 1X CSFB");
    }
  }
  else if (svc_type == LTE_PACKET_SERVICES_VIA_S1) 
  {
    mm_stop_timer(TIMER_T3417) ;
    mm_start_timer(TIMER_T3417,EMM_T3417_TIMER_DURATION) ;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= T3417 has been started for ESR for packet services");
  }


  /* Register connection release client for service request procedure.
     The client needs to be removed upon service reject reception, procedure 
     abortion and completion */
  emm_add_new_release_client(emm_service_request_release_indication);

 /* send a SIM update to RRC to start using the latest NAS COUNT
     in KENb calculation, 
     Note: do not move this section of code, since the NAS count is 
     incremented when the message is sent.
  */
  context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr,
                                               CONTEXT_STATE_CURRENT);
  if( context_ptr != NULL )
  {
    emm_send_sim_update(emm_ctrl_data_ptr);
  }


#ifdef FEATURE_DUAL_SIM
  if((service_req_cause == LTE_RRC_EST_CAUSE_MT_ACCESS) && (esr_type == NAS_EMM_3GPP_CSFB))
  {  
    /* Inform MCS module that MM is responding to CS page
       This helps MCS module in deciding priority of subscriptions.
     */

    mt_call_subs_update.state = SP_TECH_MT_CALL_START;
    mt_call_subs_update.subscription_id = (sys_modem_as_id_e_type)mm_sub_id;
 
    mm_convert_rat_to_tech_id(emm_ctrl_data_ptr->plmn_service_state->active_rat, &mt_call_subs_update.tech_id);

    MSG_HIGH_DS_3( MM_SUB, "=MM= MCS being informed of SP_TECH_MT_CALL_START by NAS state = %d, subs_id = %d, tech_id = %d", mt_call_subs_update.state, 
                                                                       mt_call_subs_update.subscription_id, 
                                                                       mt_call_subs_update.tech_id );
    (void)sp_subscription_mt_call_info_update(mt_call_subs_update);
    
  }    
#endif


} /* end of emm_start_extended_service_request_procedure() */


/*===========================================================================
FUNCTION    EMM_TIMER_T3417_EXT_EXPIRE

DESCRIPTION
  This function aborts connection or service request procedure depends on EMM
  connection state upon timer 3417 extended expiry

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_timer_t3417_ext_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)          
{
  /* MM layer will be informed of the failure as part of handling extended service request failure function*/
  emm_timer_t3417_expire(emm_ctrl_data_ptr);
} /* end of emm_timer_t3417_expire() */

#ifdef FEATURE_1XSRLTE
/*===========================================================================
FUNCTION    EMM_SRLTE_ESR_TIMER_EXPIRE

DESCRIPTION
  This function aborts connection or service request procedure depends on EMM
  connection state upon timer 3417 extended expiry

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_srlte_esr_timer_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ;
  MSG_HIGH_DS_0(MM_SUB, "=EMM= SRLTE ESR timer expired");
  if(((EMM_GET_CONNECTION_STATE() == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE) || 
     (EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE)) &&
     (emm_ctrl_data_ptr->esr_est_conn == TRUE))
  {
    /* To let LTE RRC release the TRM and use RRC connection failure to conclude ESR*/
    MSG_HIGH_DS_0(MM_SUB,"=EMM= SRLTE ESR timer expired, sending abort connection to RRC");
    emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
    emm_ctrl_data_ptr->esr_est_conn = FALSE;
  }
  else
  {
    emm_failure_type emm_failure_cause;
    memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));
    emm_failure_cause.cause_type = LTE_NAS_IRAT_T3417_EXPIRY;
    emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                        TRUE, EMM_T3417_EXT_EXPIRED, emm_failure_cause);
    /*Reset TRM to low priority after esr type reset to NONE in abort service request*/
    emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
    emm_process_unsent_messages(EMM_SRV_REQ_FAILURE_NO_THROTTLE,
                                  emm_failure_cause, 
                                  emm_ctrl_data_ptr);
    /* Process all pending irat uplink messages */
    emm_process_pending_irat_msg(LTE_NAS_IRAT_T3417_EXPIRY,
                               LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                               LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                               emm_ctrl_data_ptr);
  }
} /* end of emm_srlte_esr_timer_expire() */
#endif


/*===========================================================================
FUNCTION    EMM_SEND_ESR_IF_BUFFERED

DESCRIPTION
  This function starts buffered ESR procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_esr_if_buffered(emm_ctrl_data_type* emm_ctrl_data_ptr)
{
  boolean result = FALSE;
  dword sequence_number = 0;
  emm_security_context_info *context_ptr = NULL;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ;
  ASSERT((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)||
         (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED));

  if(emm_ctrl_data_ptr->esm_srv_req_trans_id == EMM_INVALID_ESM_SRV_REQ_TRANS_ID)
  {
    /* Obtain EMM transaction ID if service request is initiated because of 
       paging or esm data request */
    sequence_number = emm_get_nas_transaction_id();
  }
  else
  {
    /* Use received ESM transaction ID if service request is initiated because 
       of esm service request */
    sequence_number = emm_ctrl_data_ptr->esm_srv_req_trans_id;
  }

  /* Populate NAS key set identifier */
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  if( context_ptr != NULL )
  {
    /* Find the type of context i.e mapped or native*/
    if(context_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS)
    {
     emm_ctrl_data_ptr->pended_esr_msg->emm_outgoing_msg.ext_service_req.nas_key_set_identifier.security_context = 0;
    }
    else if (context_ptr->context_type == CONTEXT_TYPE_MAPPED_EPS)
    {
     emm_ctrl_data_ptr->pended_esr_msg->emm_outgoing_msg.ext_service_req.nas_key_set_identifier.security_context = 1;
    } 
    else 
    { 
      MSG_FATAL_DS(MM_SUB,"=EMM= Security Context should be valid at the time of sending ESR. Current Security Contex=%d",context_ptr->context_type,0,0);    
    } 

    emm_ctrl_data_ptr->pended_esr_msg->emm_outgoing_msg.ext_service_req.nas_key_set_identifier.keyset_id = context_ptr->nasKSI.keyset_id;
  }

  memscpy((void*)&emm_ctrl_data_ptr->pended_esr_msg->emm_outgoing_msg.ext_service_req.m_tmsi[0],
         sizeof(emm_ctrl_data_ptr->pended_esr_msg->emm_outgoing_msg.ext_service_req.m_tmsi),
         (void*)&emm_ctrl_data_ptr->emm_guti.m_tmsi[0],
         sizeof(emm_ctrl_data_ptr->emm_guti.m_tmsi));

    /* Service request message sent. Enter SERVICE_REQUEST_INITIATED_STATE */
  EMM_MOVES_TO_SERVICE_REQUEST_INITIATED_STATE() ;

  result = emm_send_message(emm_ctrl_data_ptr->pended_esr_msg, 
                                sequence_number, 
                                MSGR_NAS_EMM, 
                                NULL, 
                                emm_extended_service_request_failed, 
                                emm_ctrl_data_ptr);

  if(result != TRUE)
  {
    emm_csfb_failure_cause_type failure_cause_type;
    lte_rrc_est_cause_e              service_req_cause;
    switch(emm_ctrl_data_ptr->pended_esr_msg->emm_outgoing_msg.ext_service_req.service_type)
    {
      case LTE_MT_CSFB_1XCSFB:
      service_req_cause = LTE_RRC_EST_CAUSE_MT_ACCESS;
      break;
      case LTE_MO_CSFB_ECALL_1XCSFB_ECALL:
      service_req_cause = LTE_RRC_EST_CAUSE_EMERGENCY;
      break;
      default:
      service_req_cause = LTE_RRC_EST_CAUSE_MO_DATA;
      break;
    }
    if (FALSE == emm_connection_setup(service_req_cause,
                                        emm_ctrl_data_ptr->pended_esr_msg, 
                                        sequence_number, 
                                        MSGR_NAS_EMM, 
                                        NULL, 
                                        emm_extended_service_request_failed,
                                        ROUTING_INFO_STMSI,
                                        NULL,
                                        emm_ctrl_data_ptr))
    {
         if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)
         {
           emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_3GPP_CSFB;
         }
         else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)
         {
           emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_1xCSFB;
         }
         failure_cause_type.failure_cause.lte_nas_failure_cause = LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE;
         emm_process_extended_service_req_failure(failure_cause_type , emm_ctrl_data_ptr);
    }

  }

  if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)
  {
    emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_3GPP_CSFB;
    mm_start_timer(TIMER_T3417_EXT, EMM_T3417_EXT_TIMER_DURATION);
  }
  else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)
  {
    emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_1xCSFB;
    mm_start_timer(TIMER_T3417,EMM_T3417_TIMER_DURATION);
  }
  
  emm_add_new_release_client(emm_service_request_release_indication);

} /* emm_send_esr_if_buffered */

/*===========================================================================
FUNCTION    EMM_ABORT_AND_CLEAN_CSFB_CALL

DESCRIPTION
  This function aborts the ESR and cleans up the ESR state information
  Autonomously clean up the pended ESR messages and reset the ESR type and STOP the TIMER
  It is a conscious decision to let the ESR run even after the call is borted by the upper lyaer/user. If UE is redirected it will go to GW.
    If ESR fails, it will not be retransmitted or failure is indicated to the upper layers 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  THIS WILL NOT PREVENT OTA extended service request procedure if already started
===========================================================================*/
void emm_abort_and_clean_csfb_call
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)          
{
  ASSERT(emm_ctrl_data_ptr != NULL);
  if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)
  {
   MSG_HIGH_DS_0(MM_SUB, "=EMM= Buffered ESR is cleared as the CSFB call is aborted");
   if(emm_ctrl_data_ptr->pended_esr_msg != NULL)
   {
     /* Free the memory block */
     emm_modem_mem_free((void *)emm_ctrl_data_ptr->pended_esr_msg, emm_ctrl_data_ptr);
     emm_ctrl_data_ptr->pended_esr_msg = NULL;
   }
  }

  if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB ||
  	emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)
  {
    emm_failure_type               emm_failure_cause;
    memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));
	emm_failure_cause.cause_type       = LTE_NAS_IRAT_NONE;
    emm_failure_cause.cause.rel_reason = LTE_RRC_CONN_REL_NORMAL;
	
    emm_abort_service_request(emm_ctrl_data_ptr, FALSE, TRUE,
                                    NAS_SIG_CONN_RELEASED, emm_failure_cause);
#ifdef FEATURE_1XSRLTE
    if(((emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == TRUE)
      &&(emm_ctrl_data_ptr->srlte_tau_needed == TRUE))
      ||(emm_ctrl_data_ptr->partial_esr == TRUE))
    {
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
    }
#endif
  }
  /* Stop the TIMER blindly if running or NOT*/
  mm_stop_timer(TIMER_T3417_EXT);  
  /* Clear the ESR context*/
  emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_CSFB_NONE;
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
#endif
  MSG_HIGH_DS_0(MM_SUB, "=EMM= CSFB call is aborted, cleared the TIMER and the ESR type");
} /* end of emm_abort_and_clean_csfb_call() */



#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================
FUNCTION    emm_process_buffered_csfb_hard_failure

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_buffered_csfb_hard_failure
(
  void
)
{
   emm_csfb_failure_cause_type failure_cause_type;
   failure_cause_type.failure_type = AS_REJECT_CAUSE;
   failure_cause_type.failure_cause.lte_nas_failure_cause = AS_REJ_LRRC_LOWER_LAYER_FAILURE;
   emm_process_extended_service_req_failure(failure_cause_type,emm_ctrl_data_ptr);
}
/*===========================================================================
FUNCTION    EMM_PROCESS_CS_SERVICE_NOTIFICATION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_cs_service_notification
(
  lte_nas_emm_cs_serv_notification* cs_service_notification,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)          
{
  emm_csfb_rsp                     csfb_response;
  boolean cs_srv_notification_allowed = FALSE;
  emm_substate_type emm_substate = EMM_GET_SUBSTATE();
  emm_state_type emm_state = EMM_GET_STATE();

  if(emm_reject_cs_service_notification(emm_ctrl_data_ptr) == TRUE)
  {
    emm_send_esr_csfb_reject_msg(emm_ctrl_data_ptr);
    MSG_HIGH_DS_0(MM_SUB, "=EMM= sending ESR with CSFB reject during VOLTE call");
    return;
  }
  else if(emm_ignore_cs_page_cs_service_notification(emm_ctrl_data_ptr) == TRUE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Ignoring CS Service Notification during VOLTE call");
    return;
  }


#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
  if((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS)&&
     (emm_ctrl_data_ptr->reg_req_pending_ptr->sms_only == FALSE)&&
     ((emm_state == EMM_REGISTERED && emm_substate != EMM_REGISTERED_NO_CELL_AVAILABLE) ||
      (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED) ||
      (emm_state == EMM_SERVICE_REQUEST_INITIATED)))
  {
    cs_srv_notification_allowed = mm_csfb_process_mt_page(cs_service_notification->paging_id);
  }
  if(cs_srv_notification_allowed)
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= CS_SERVICE_NOTIFICATION with paging identity %x is accepted",
                  cs_service_notification->paging_id);
    emm_ctrl_data_ptr->csfb_mt_call_info_ptr->is_mt_call_recv_in_conn_state = TRUE;
    csfb_response.valid = TRUE;
    csfb_response.csfb_rsp = CSFB_ACCEPT;
#ifdef FEATURE_NAS_REL10
    mm_stop_timer(TIMER_T3246);
#endif
    emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MT_ACCESS,LTE_MT_CSFB_1XCSFB,csfb_response,NAS_EMM_3GPP_CSFB);
  }
  else
  {
    MSG_ERROR_DS_4(MM_SUB,"=EMM=CS_SERVICE_NOTIFICATION is DROPPED page id %d, emm_state %d emm_substate %d, reg_service_domain %d",
                   cs_service_notification->paging_id,
                   emm_state,
				   emm_substate,
                   emm_ctrl_data_ptr->plmn_service_state->reg_service_domain);
#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
    /* Should we send a REJECT for this or not. The SPEC says to respond back ONLY if USER REJECTS the call*/
  }
} /* end of emm_process_cs_service_notification() */

/*===========================================================================
FUNCTION   EMM_PROCESS_RRC_1XCSFB_HO_STARTED_IND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rrc_1xcsfb_ho_started_ind(lte_rrc_1xcsfb_ho_started_ind_s* msg_ptr,emm_ctrl_data_type   *emm_ctrl_data_ptr)
{
  ASSERT(msg_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);

  if( (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB) || (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB) )
  {
    mm_send_cm_ps_data_suspend_ind();
    emm_process_esr_complete(emm_ctrl_data_ptr);

    /* setting rrc_active to false and service_status to no service since HO started*/
    emm_ctrl_data_ptr->rrc_active = FALSE;
    emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_NO_SRV;
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM=Received unsolicited 1XCSFB_HO_STARTED_IND. Ignroring it");
  }
  //EMM_MOVES_TO_IDLE_STATE();
}

/*===========================================================================
FUNCTION   EMM_PROCESS_RRC_1XCSFB_HO_FAILED_IND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rrc_1xcsfb_ho_failed_ind(lte_rrc_1xcsfb_ho_failed_ind_s* msg_ptr,emm_ctrl_data_type   *emm_ctrl_data_ptr)
{
  emm_csfb_failure_cause_type failure_cause_type;
  emm_failure_type            emm_failure_cause;
  boolean reset_esr_type = FALSE;
  
  ASSERT(msg_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  emm_failure_cause.cause_type = LTE_NAS_IRAT_LOWER_LAYER_FAILURE;

  /* This is to tell that the failure is not 1X related*/
  failure_cause_type.failure_type = ONEX_CSFB_REJECT_CAUSE;
  failure_cause_type.failure_cause.onex_failure_cause = msg_ptr->failure_cause;

  if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB)
  {
    //Send 3rd paramater as FALSE as this is not a service request failed case. Pass local_casue i.e 4th paramater as anything. It doesn't matter as long as the 3rd parameter is FALSE
    emm_abort_service_request(emm_ctrl_data_ptr, FALSE, FALSE, 
                              NAS_SIG_CONN_RELEASED, emm_failure_cause);
  }

  if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)
  {
    emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_1xCSFB;
    reset_esr_type = TRUE;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Forcing esr_type_in_progress to NAS_EMM_1xCSFB from NAS_EMM_CSFB_NONE. This happens in cases ESR ended but EMM had to route 1XCP failures through NAS");
  }

  emm_process_extended_service_req_failure(failure_cause_type,emm_ctrl_data_ptr);

  if(reset_esr_type == TRUE)
  {
    emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_CSFB_NONE;
    reset_esr_type = FALSE;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Forcing esr_type_in_progress to NAS_EMM_CSFB_NONE from NAS_EMM_1xCSFB after routing 1XCP failures through NAS when no ESR is pending");
  }
}

/*===========================================================================
FUNCTION   EMM_PROCESS_RRC_1XSRVCC_HO_STARTED_IND

DESCRIPTION Process 1XSRVCC HO Started Indication from RRC 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rrc_1xsrvcc_ho_started_ind(lte_rrc_1xsrvcc_ho_started_ind_s* msg_ptr,emm_ctrl_data_type   *emm_ctrl_data_ptr)
{
  emm_state_type            emm_state = (emm_state_type)EMM_UNKNOWN_STATE;
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);

  emm_state = EMM_GET_STATE();

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Rcved 1xsrvcc HO Start IND in EMM state %d", emm_state);

  /*UE will start TAU procedure to resume PS on LTE*/
  if((emm_state != EMM_DEREGISTERED)&&
     (emm_state != EMM_DEREGISTERED_INITIATED)&&
     ((emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_INVALID)||
      (emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_PERIODIC)))
  {
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
  }
}

/*===========================================================================
FUNCTION   EMM_PROCESS_RRC_1XSRVCC_HO_FAILED_IND

DESCRIPTION  Process 1XSRVCC HO Failure from RRC 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rrc_1xsrvcc_ho_failed_ind(lte_rrc_1xsrvcc_ho_failed_ind_s* msg_ptr,
                                           emm_ctrl_data_type   *emm_ctrl_data_ptr)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Rcved 1xsrvcc HO Failure IND");
}

#endif /*FEATURE_LTE*/




