/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     GMM Security Management (GMMSECURITY.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001, 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmmsecurity.c_v   1.11   18 Jul 2002 11:40:42   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmmsecurity.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/01   jca     Created module
05/29/01   jca     Added support for P-TMSI Reallocation procedure.
06/08/01   jca     Added support for integrity protection.
09/12/01   jca     Added RRC_PAGING_CTRL_REQ processing.
03/21/02   vdr     Fixed the domain ID, when the Authentication responses are
                   sent to the net
04/30/02   ks      Added support for GMM Identity Procedure.
05/02/02   vdr     Added CN domain handling for the GMM Authentication
07/08/02   vdr     Added GMM-LLC related changes
07/18/02   vdr     Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
08/10/02   mks     Obsoleted RRC_PAGING_CTRL_REQ as the information conveyed by
                   this primitive is now sent to RRC through RRC_SIM_INSERTED_REQ
                   & RRC_SIM_UPDATE_REQ.
10/01/02   vdr     Changed Authentication Server interface
12/13/02   vdr     Fixed Authentication response when RAND is not present
12/16/02   jca     Removed redundant PERFORCE header.  Fixed Lint error.
02/27/03   vdr     Added processing of the UMTS to GSM RAT changes
04/16/03   vdr     Now forcing GSM AKA if MS is running GSM SIM
05/23/03   jca     Make sure to update mm_serving_plmn_info when GMM deregisters.
07/09/03   vdr     Remove processing of the gmm_ptmsi_allocated_rai variable
12/17/04   vdr     Now performing cache authentication if TIMER_T3316 is running
                   in UMTS
03/21/05   sm      Handling the Network Authentication failure.
03/31/05   hj      Added support for sending CM_CIPHER_IND after configuring
04/12/05   hj      Changed call to mm_send_cm_cipher_ind to reflect new 
                   enum type for cipher_domain.
09/21/05   hj      Now not sending cipher indication if the ciphering 
                   algorithm is set to 0 by the network.
10/06/05   ajt     Invalidate authentication cache while sending Authentication
                   & Ciphering Failure to the network.                   
11/11/05   ss      Updated for Rel5 changes.
12/09/05   vdr     Now performing same RAND authentication if UE is able to 
                   perform UMTS authentication
06/14/06  up       Called new gmm_update_mm_serving_plmn_info for GSTK.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "comdef.h"
#include "err.h"
#include "msg.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_sim.h"
#include "gmm_v.h"
#include "gmm_int_v.h"
#include "gmm_msg.h"
#include "rrcmmif.h"
#include "rrccmd.h"
#include "mm_umts_v.h"
#include "mmsecurity.h"
#include "gmmsecurity.h"
#include "timers_v.h"
#include "rrcmmif.h"
#include "libprot.h"
#include "mm_coord.h"
#include "sys_eplmn_list.h"
#include <stringl/stringl.h>
#ifdef FEATURE_GSM_GPRS
#include "gmmllcm.h"
#endif
#include "smtask.h"
#include "smtask_v.h"
#include "sys_eplmn_list_v.h"



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.
===========================================================================*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

/* The IMEISV included indicator, The indicator is equal to 1 (TRUE) when
** IMEISV shall be included into AUTHENTICATION & CIPHERING RESPONSE message
** and 0 (FALSE) otherwise
*/
byte       gmm_imeisv_required_sim[MAX_NAS_STACKS] ;

/* This variable keeps AUTHENTICATION & CIPHERING RESPONSE message reference
** number.
*/
byte       gmm_anc_reference_number_sim[MAX_NAS_STACKS] ;

#else

/* The IMEISV included indicator, The indicator is equal to 1 (TRUE) when
** IMEISV shall be included into AUTHENTICATION & CIPHERING RESPONSE message
** and 0 (FALSE) otherwise
*/
byte       gmm_imeisv_required ;

/* This variable keeps AUTHENTICATION & CIPHERING RESPONSE message reference
** number.
*/
byte       gmm_anc_reference_number ;

#endif /* FEATURE_DUAL_SIM  ||  FEATURE_SGLTE*/

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
void ghdi_ds_read_imeisv(byte *imeisv,sys_modem_as_id_e_type as_id);   
#endif

/* FEATURE_TRIPLE_SIM*/
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
/* Current Authentication transaction ID */
static client_id  auth_transaction_id_sim[MAX_NAS_STACKS] = { (client_id)NULL, (client_id)NULL ,(client_id)NULL } ;
#define auth_transaction_id auth_transaction_id_sim[mm_as_id]

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
/* Current Authentication transaction ID */
static client_id  auth_transaction_id_sim[MAX_NAS_STACKS] = { (client_id)NULL, (client_id)NULL } ;
#define auth_transaction_id auth_transaction_id_sim[mm_as_id]

#else
/* Current Authentication transaction ID */
static client_id  auth_transaction_id = (client_id)NULL;

#endif /* FEATURE_TRIPLE_SIM*/


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,(sys_modem_as_id_e_type)mm_sub_id,(sys_modem_as_id_e_type)mm_as_id)
#endif

static boolean gmm_compare_auth_data(gmm_anc_request_message_T message);
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE  
static gmm_anc_request_message_T gmm_auth_data_sim[MAX_NAS_STACKS];
#define gmm_auth_data gmm_auth_data_sim[mm_as_id]
#else
static gmm_anc_request_message_T gmm_auth_data;
#endif


//#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
// #include <tf_stub.h>

// TF_PROTO_STUB(void ,mmrrcconn_send_L3_message_to_rrc, void*,rrc_establish_cause_e_type,rrc_cn_domain_identity_e_type);
// #define mmrrcconn_send_L3_message_to_rrc TF_STUB(mmrrcconn_send_L3_message_to_rrc)
//#endif

/*===========================================================================

FUNCTION GMM_PROCESS_PTMSI_REALLOCATION_COMMAND_MSG

DESCRIPTION
  Function to

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_process_ptmsi_reallocation_command_msg( mm_cmd_type *message )
{
  gmm_ptmsi_realloc_command_msg_T ptmsi_reallocation_command_msg;
  byte                            i;
  sys_plmn_id_s_type ptmsi_realloc_plmn;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (gmm_valid_ptmsi_reallocation_command_message(
        message, &ptmsi_reallocation_command_msg ))
  {

    if ((gmm_state == GMM_REGISTERED) || (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
/*** spec 24.008 4.7.4.1.4: 
  **GPRS detach containing other causes than "power off" and containing detach type "IMSI detach":
	-If the MS receives a message used in a GMM common procedure before the GPRS detach procedure has been completed, both the GMM common procedure and the GPRS detach procedure shall continue.
**-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------**/
#ifdef FEATURE_NAS_REL11
        || ((gmm_state == GMM_DEREGISTERED_INITIATED) && (gmm_detach_type == GMM_MO_IMSI_DETACH))
#endif
       )
    {
	   ptmsi_realloc_plmn.identity[0] = ptmsi_reallocation_command_msg.routing_area_identification[0];
	   ptmsi_realloc_plmn.identity[1] = ptmsi_reallocation_command_msg.routing_area_identification[1];
	   ptmsi_realloc_plmn.identity[2] = ptmsi_reallocation_command_msg.routing_area_identification[2];
	   if((mm_reg_waiting_for_reg_cnf) && 
		((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
		 (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
		  mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)) &&
	   !((sys_plmn_match(mm_serving_plmn.info.plmn , ptmsi_realloc_plmn )) ||
	     (sys_eplmn_list_equivalent_plmn(ptmsi_realloc_plmn))))
	   {
		 MSG_HIGH_DS_3(MM_SUB, "=MM= Invalid PLMN ID received during ptmsi reallocation MCC/MNC %02x %02x %02x ",
									   ptmsi_realloc_plmn.identity[0],ptmsi_realloc_plmn.identity[1],ptmsi_realloc_plmn.identity[2]);
	   }
	   else
	   {
        /* ----------------------------------------------------------------------
        ** Upon receipt of the P-TMSI REALLOCATION COMMAND message, the MS stores
        ** the Routing Area Identifier (RAI) and the P-TMSI and sends a P-TMSI
        ** REALLOCATION COMPLETE message to the network.
        ** ---------------------------------------------------------------------- */
        gmm_stored_rai.plmn_id.identity[0] =
          ptmsi_reallocation_command_msg.routing_area_identification[0];
        gmm_stored_rai.plmn_id.identity[1] =
          ptmsi_reallocation_command_msg.routing_area_identification[1];
        gmm_stored_rai.plmn_id.identity[2] =
          ptmsi_reallocation_command_msg.routing_area_identification[2];
        gmm_stored_rai.location_area_code.lac[0] =
          ptmsi_reallocation_command_msg.routing_area_identification[3];
        gmm_stored_rai.location_area_code.lac[1] =
          ptmsi_reallocation_command_msg.routing_area_identification[4];
        gmm_stored_rai.routing_area_code =
          ptmsi_reallocation_command_msg.routing_area_identification[5];
      }

      for (i=0; i < PTMSI_SIZE; i++)
      {
        gmm_stored_ptmsi.digit[i] = ptmsi_reallocation_command_msg.allocated_ptmsi[i];
      }

#ifdef FEATURE_GSM_GPRS
      if (is_gsm_mode())
      {
        gmmllc_gmm_trigger_llc_event
          (GMM_TRIGGER_PTMSI_ASSIGNED | GMM_TRIGGER_FORCE_TO_STANDBY,
                           ptmsi_reallocation_command_msg.force_to_standby,0) ;
      }
#endif
      /* ---------------------------------------------------------------------
      ** If a P-TMSI signature is present in the P-TMSI REALLOCATION COMMAND
      ** message, the MS shall store the new P-TMSI signature and shall if
      ** available delete the old P-TMSI signature.  If no P-TMSI signature
      ** is present in the P-TMSI REALLOCATION COMMAND message, the old P-TMSI
      ** signature, if available, shall be kept (24.008 4.7.6.2)
      ** --------------------------------------------------------------------- */
      if (ptmsi_reallocation_command_msg.ptmsi_signature_present)
      {
        /* -----------------------------------------------------
        ** Delete the old P-TMSI signature and store the new one
        ** ----------------------------------------------------- */
        for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
        {
          gmm_stored_ptmsi_signature.value[i] =
            ptmsi_reallocation_command_msg.ptmsi_signature[i];
        }
      }

      mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                          gmm_stored_ptmsi_signature,
                                          gmm_stored_rai,
                                          gmm_update_status );

      gmm_send_ptmsi_reallocation_complete_message();

#ifdef FEATURE_DUAL_SIM
      if(MM_MODEM_STATISTICS_SUB1)
#endif
      {
        mm_set_signal_for_cm(SYS_NAS_REGISTRATION_INFO_MASK);
      }


    }
    else if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
    {
      /* ---------------------------------------------------------------------
      ** Routing area updating and P-TMSI reallocation procedure collision -
      ** If the MS receives a P-TMSI REALLOCATION COMMAND message before the
      ** routing area updating procedure has been completed, the P-TMSI
      ** reallocation procedure shall be aborted and the routing area updating
      ** procedure shall be progressed.
      ** --------------------------------------------------------------------- */
    }
    else if (gmm_state == GMM_DEREGISTERED_INITIATED)
    {
      /* -----------------------------------------------------------------------
      ** Detach and GMM common procedure collision - If the MS receives a P-TMSI
      ** REALLOCATION COMMAND message before the GPRS detach procedure has been
      ** completed, this message shall be ignored and the GPRS detach procedure
      ** shall continue.
      ** ----------------------------------------------------------------------- */
    }
  }
  else
  {
    MSG_HIGH_DS_1( MM_SUB, "=MM= Message received in unexpected state %d", gmm_state );
  }
} /* end gmm_process_ptmsi_reallocation_command_msg() */

/*===========================================================================

FUNCTION    GMM_HANDLE_COMMON_AUTH_RESPONSE

DESCRIPTION
  This function is called by the Authentication server in order to build
  appropriate authentication response message (AUTHENTICATION & CIPHERING
  RESPONSE or AUTHENTICATION & CIPHERING FAILURE) and send it to the network.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_handle_common_auth_response
(
  /* Functional Authentication Status code */
  auth_status_type status_code,

  /* Authentication Response Error code */
  auth_error_type auth_error,

  /* Pointer to Authentication Response Value (RES) */
  auth_res_type *auth_res,

  /* Pointer to AUTS value */
  auth_auts_type *auth_auts
)
{
  byte message_buffer_length ;
  gmm_authentication_message_buffer_type message_buffer ;
  memset((void *)&message_buffer, 0, sizeof(gmm_authentication_message_buffer_type));

  /* If the authentication procedure has been completed successfully, i.e.,
  ** the Mobile sends AUTHENTICATION & CIPHERING RESPONSE message,
  ** the current auth_transaction_id shall be re-initialized to ZERO
  */
  if (status_code == AUTHENTICATION_DONE)
  {
     auth_transaction_id = (client_id)NULL;
  }else if (status_code == AUTHENTICATION_NETWORK_FAILURE)
  {
    auth_transaction_id = (client_id)NULL;
    MSG_HIGH_DS_0( MM_SUB, "=MM= Authentication failed for Network in MS");
    return;
  }

  switch (auth_error)
  {
    case AUTH_NO_ERROR:
      /* The Authentication challenge completed successfully, build
      ** AUTHENTICATION & CIPHERING RESPONSE message.
      */
      gmm_build_anc_response(auth_res,&message_buffer_length,&message_buffer) ;

      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending AUTHENTICATION AND CIPHERING RESPONSE message");

      break ;
    case AUTH_MAK_CODE_FAILURE:
    case AUTH_SQN_FAILURE:
    case AUTH_AUTHENTICATION_UNACCEPTABLE:
      /* Error condition has been met when the mobile tried to authenticate the
      ** network, AUTHENTICATION & CIPHERING FAILURE message shall be built.
      */
      gmm_build_anc_failure
                (auth_error,auth_auts,&message_buffer_length,&message_buffer) ;
                
      auth_reset_cache_memory(FALSE, NO_TIMER_EXPIRED, RRC_PS_DOMAIN_CN_ID);                     
      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY, 
                                 (byte)auth_error,
                                 mm_serving_plmn.info.plmn,
                                 mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                 , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                 , mm_serving_plmn.info.active_rat
                              );
      MSG_HIGH_DS_0( MM_SUB, "=MM= Sending AUTHENTICATION AND CIPHERING FAILURE message");

      break ;
    default:
      /* This case should never be reached */
      MSG_FATAL_DS(MM_SUB, "=MM= Illigal authentication error code returned",0,0,0) ;
      break ;
  }

  /* Setup a right message length for an outgoing message */
  PUT_IMH_LEN(message_buffer_length,&message_buffer.gmm_msg_head.IMH);
 
  /* Send authentication response (AUTHENTICATION & CIPHERING RESPONSE or
  ** AUTHENTICATION & CIPHERING FAILURE) message to the network.
  */
  mmrrcconn_send_L3_message_to_rrc(
    &message_buffer, RRC_EST_LOW_PRI_SIGNALLING, RRC_PS_DOMAIN_CN_ID );

} /* end mm_handle_common_auth_response() */

/*===========================================================================

FUNCTION

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_get_timer_list
(
  byte *timer_list_length,
  auth_timer_list_type *timer_list
)
{
  *timer_list_length = 4 ;

  timer_list[0].timer = TIMER_T3310 ;

  timer_list[1].timer = TIMER_T3321 ;

  timer_list[2].timer = TIMER_T3330 ;

  timer_list[3].timer = TIMER_T3317 ;
} /* end gmm_get_timer_list() */
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
void gmm_handle_sim_busy_ind(boolean cancel_auth)
{
   if((auth_transaction_id != ((client_id)NULL) && auth_is_procedure_to_be_aborted(auth_transaction_id))
	   || cancel_auth == TRUE)
   {
     gmm_cancel_authentication();
     mmgsdi_session_cancel_requests(mm_sim_mmgsdi_info.session_id);
   
    
        if(is_umts_mode())
        {
          if(cs_session_status == MMRRC_IDLE)
          {
           rrc_cmd_type *rrc_cmd_p = NULL; 
      
           rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
      
           if (rrc_cmd_p == NULL)
           {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
           }
           rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
           rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = FALSE;
           rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = TRUE;
           rrc_cmd_p->cmd.rrc_abort_req.hard_abort = FALSE;     
         
           rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
           MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RRC_ABORT_REQ");
           send_cmd_to_rrc(rrc_cmd_p, mm_serving_plmn.info.active_rat );
         }
         else
         {
           rrc_cmd_type *rrc_cmd_p = NULL;
      
           rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
      
           if (rrc_cmd_p == NULL)
           {
              MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
           }
		   rrc_cmd_p->cmd_hdr.cmd_id = RRC_CLOSE_SESSION_REQ;
           rrc_cmd_p->cmd.close_sess_req.close_session_cause =RRC_OTHERS;
	       rrc_cmd_p->cmd.close_sess_req.cn_domain_id = RRC_PS_DOMAIN_CN_ID;

           MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RRC_CLOSE_SESSION_REQ" );
           send_cmd_to_rrc(rrc_cmd_p, mm_serving_plmn.info.active_rat );
         }
     }
     else
     {
        if(gmm_state == GMM_REGISTERED_INITIATED)
        {
           gmm_increment_gprs_attach_attempt_counter();
        }
	 else if(gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
	 {
          gmm_increment_routing_area_update_attempt_counter
          (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
            FALSE
#endif
          );
	 }
	 else if(gmm_state == GMM_DEREGISTERED_INITIATED)
	 {
	   gmm_handle_gprs_detach_low_level_failure();

	 }
   }
 }
}
#endif
/*===========================================================================

FUNCTION GMM_PERFORM_AUTHENTICATION

DESCRIPTION
  This performs an GMM Authentication procdeure according to TS 24.008 4.7.7.
  The function is called upon reception of the AUTHENTICATION & CIPHERING
  REQUEST message and receives a pointer to the incoming message, decodes the
  message and calls the USIM interface routine in order to perform an
  authentication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_perform_authentication
(
  mm_cmd_type *message
)
{
  gmm_anc_request_message_T incoming_request ;
  auth_request_properties_type  request_properties ;

  /* Clear incoming message buffer */
  memset((void *)&incoming_request,0x00,sizeof(gmm_anc_request_message_T)) ;

  /* convert incoming message into internal format */
  if (gmm_get_valid_anc_request_message(message,&incoming_request) == TRUE)
  {
    /* If the MAK/SQN Timers are already running, stop them */
    MSG_HIGH_DS_0( MM_SUB, "=MM= Perform Authentication, stop MAK/SQN timers if running " );
    if (auth_transaction_id != ((client_id)NULL))
    {
      auth_stop_auth_mak_sqn_timers(auth_transaction_id) ;  
    }

#ifdef FEATURE_GSM_GPRS
    if (is_gsm_mode())
    {
      gmm_trigger_llc_type gmm_command = GMM_TRIGGER_FORCE_TO_STANDBY ;

      if ( incoming_request.rand.value_length == 0)
      {
        gmm_command |= GMM_TRIGGER_NEW_KC_VALUE ;
      }

      auth_ps_ciphering_algorithm = incoming_request.ciphering_algorithm ;

      gmmllc_gmm_trigger_llc_event(gmm_command,
                                        incoming_request.force_to_standby,0) ;

    }
#endif

    /* Save Authentication And Ciphering message reference number */
    gmm_anc_reference_number = incoming_request.anc_reference_number ;

    /* Save imeisv_required */
    gmm_imeisv_required = incoming_request.imeisv_required ;

    if (incoming_request.rand.value_length == 0)
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to perform authentication due to lack of RAND") ;

      /* Send empty Authentication and Ciphering Response message */
      gmm_handle_common_auth_response
                             (AUTHENTICATION_DONE, AUTH_NO_ERROR, NULL, NULL) ;
    }
    else
    {
      if ((mm_sim_card_mode == MMGSDI_APP_USIM) && 
          (mm_timer_status [ TIMER_T3316 ] == TIMER_ACTIVE))
      {
        if (auth_gmm_perform_cache_authentication(
             incoming_request.rand.value_length,
                (byte *)incoming_request.rand.value_data, 
                                             gmm_handle_common_auth_response))
        {
          return ; /* Authentication completed */
        }
      }
      /* Check if SIM is already processing a request*/
      if (mm_ps_auth_sent_to_card) 
      {
          /* Check if the new request is the same as the old one */
          if (gmm_compare_auth_data(incoming_request)) 
          {
              MSG_HIGH_DS_0(MM_SUB, "=MM= Back to back same Auth Req, ignore") ;
              return;
          }          
      }
      
      memscpy(&gmm_auth_data, sizeof (incoming_request),
                  &incoming_request, sizeof (incoming_request));
      
            
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
      if(mm_is_sim_busy)
      {
         gmm_handle_sim_busy_ind(TRUE);
         return;
      }
#endif
      /* When the AUTHENTICATION & CIPHERING REQUEST message has been parsed
      ** successfuly, prepare the internal authentication request and call
      ** Authentication server
      **/
      request_properties.ciphering_key_sequence_number =
                                          incoming_request.gprs_ck_seqnumber ;

      request_properties.ciphering_algorithm =
                                        incoming_request.ciphering_algorithm ;

      request_properties.cn_domain_identity = RRC_PS_DOMAIN_CN_ID ;

      request_properties.client_response_callback =
                                            gmm_handle_common_auth_response ;

      request_properties.client_timer_set.mak_failure_timer = TIMER_T3318 ;

      request_properties.client_timer_set.sqn_failure_timer = TIMER_T3320 ;

      request_properties.client_timer_set.num_failures = 0;

      request_properties.cache_guard_timer_id = TIMER_T3316 ;

      /* Initialize GMM re-tx timer list */
      gmm_get_timer_list(
                &request_properties.client_timer_set.timer_list_length,
                              request_properties.client_timer_set.timer_list) ;

      /* According to TS 24.008 4.7.7.2 A MS which does not support the UMTS
      ** authentication algorithm shall ignore the Authentication Parameter AUTN
      ** if included in the AUTHENTICATION AND CIPHERING REQUEST message and shall
      ** proceed as in case of a GSM authentication challenge.
      **/
      if ((mm_sim_card_mode == MMGSDI_APP_SIM)
          && (incoming_request.autn.value_length > 0))
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Due to lack of UMTS auth capability force GSM auth") ;

        incoming_request.autn.value_length = 0 ;
      }

      if (incoming_request.autn.value_length == 0)
      {
        request_properties.current_auth_request_context = MMGSDI_AUTN_USIM_GSM_CONTEXT;
      }
      else
      {
        request_properties.current_auth_request_context = MMGSDI_AUTN_USIM_3G_CONTEXT;
      }
      
      auth_transaction_id = auth_authentication_request(
        incoming_request.gprs_ck_seqnumber,
            &incoming_request.rand,&incoming_request.autn,&request_properties) ;

    }
  }
  else
  {
    /* When the mobile is unable to read incoming AUTHENTICATION & CIPHERING
    ** REQUEST message, the message shall be descarded and appropriate error
    ** message shall be generated
    */

    MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to perform authentication" );
  }
} /* end gmm_perform_authentication() */


/*===========================================================================

FUNCTION GMM_AUTH_FAILED

DESCRIPTION
  This function conteins an actions the MS shall perform upon reception of the
  AUTHENTICATION AND CIPHERING REJECT MESSAGE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_handle_authentication_reject
(
  void
)
{
  byte gmm_timer_list_length ;
  auth_timer_list_type gmm_retx_timer_list[MAX_TIMER_ARRAY_LENGTH] ;
  sm_cmd_type                *sm_cmd;
  /*
  ** Retrieve GMM retx timers
  */
  gmm_get_timer_list(&gmm_timer_list_length,gmm_retx_timer_list) ;

  /*
  ** Call the Authentication server in order to initiate the processing
  ** of the Authentication Reject message
  */
  auth_handle_network_reject
          (auth_transaction_id,gmm_timer_list_length,gmm_retx_timer_list) ;

  /*
  ** Authentication transaction has been finished, clear the transaction id
  */
  auth_transaction_id = (client_id)NULL ;

  /* ------------------------------------------------------------------
  ** The MS shall set the GPRS update status to GU3 ROAMING NOT ALLOWED
  ** and shall delete any P-TMSI, P-TMSI signature, RAI and GPRS
  ** ciphering key sequence number.  The new GMM state is
  ** GMM-DEREGISTERED.  The SIM shall be considered as invalid for
  ** GPRS services until switching off or the SIM is removed.
  ** ------------------------------------------------------------------ */
  gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;
  gmm_delete_ps_location_information();
  gmm_delete_gprs_cksn();
  mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                      gmm_stored_ptmsi_signature,
                                      gmm_stored_rai,
                                      gmm_update_status );

  gmm_set_state( GMM_DEREGISTERED );
  gmm_set_substate( GMM_LIMITED_SERVICE );
  MSG_HIGH_DS_0( MM_SUB, "=MM= pmm_mode = PMM_DETACHED");
  pmm_mode = PMM_DETACHED;

#ifdef FEATURE_GPRS_PS_HANDOVER
  if (gmm_psho_status == GMM_PSHO_IN_PROGRESS)
  {
    gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_LLC_SUSPENSION,FALSE,0) ;
    gmm_psho_status = GMM_PSHO_NONE;
  }
#endif 

  sm_cmd = sm_get_cmd_buf();

  if (sm_cmd == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
  }

  sm_cmd->header.message_set = MS_GMM_SM;
  sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
  sm_cmd->cmd.gmm_gprs_rel_ind.cause = NO_GPRS_CONTEXT;
#ifdef FEATURE_DUAL_SIM
  sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

  MSG_HIGH_DS_0( MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND" );

  sm_put_cmd( sm_cmd );

  /* TMSI, LAI and ciphering key sequence number shall be deleted and the
  ** update status shall be set to U3 ROAMING NOT ALLOWED.
  ** The SIM shall be considered as invalid until switching off or the
  ** SIM is removed.
  */
  mm_perform_reject_actions(RRC_PS_DOMAIN_CN_ID) ;

  /* Clean up authentication cache memory */
  auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED,RRC_PS_DOMAIN_CN_ID) ;


  gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_PS_INVALID,
                                   SYS_SRV_DOMAIN_NO_SRV,
                                   TRUE,
                                   SYS_SRV_STATUS_LIMITED,
                                   TRUE );

  if((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
     (mm_gs_association_established == TRUE))
  {
      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_PS, 
                                 255,
                                 mm_serving_plmn.info.plmn,
                                 mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                ,mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                ,mm_serving_plmn.info.active_rat
                              );
  }
  else
  {
      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_PS_ONLY, 
                                 255,
                                 mm_serving_plmn.info.plmn,
                                 mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                ,mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                ,mm_serving_plmn.info.active_rat
                              );
  }

} /* end gmm_handle_authentication_reject() */

/*===========================================================================

FUNCTION        GMM_HANDLE_SECURITY

DESCRIPTION
  This function routes the messages relating to GMM security to the
  appropriate function for detailed processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_handle_security
(
  /* Incoming security related message */
  mm_cmd_type *message
)
{
  switch (message->cmd.hdr.message_set)
  {
    case MS_MM_TDSRRC:
    case MS_MM_RRC:
      switch (message->cmd.hdr.message_id)
      {
        case MM_DATA_IND:
          switch (message->cmd.mm_data_ind.L3_data[1])
          {
            case GMM_PTMSI_REALLOCATION_COMMAND:
              MSG_HIGH_DS_0( MM_SUB, "=MM= Received PTMSI REALLOCATION COMMAND message" );

#ifdef FEATURE_GSM_GPRS
              if ((is_umts_mode() && (gmm_integrity_protection_activated
#ifdef FEATURE_TDSCDMA
                                      || (is_tdscdma_mode() && !gmm_anite_gcf_flag)
#endif /* FEATURE_TDSCDMA */
                                      )) || is_gsm_mode())
#else
              if (gmm_integrity_protection_activated
#ifdef FEATURE_TDSCDMA
                  || (is_tdscdma_mode() && !gmm_anite_gcf_flag)
#endif /* FEATURE_TDSCDMA */
                  )
#endif
              {
                gmm_process_ptmsi_reallocation_command_msg( message );
              }
              else
              {
                MSG_ERROR_DS_0( MM_SUB, "=MM= Integrity protection not activated");
              }
              break;

            case GMM_AUTHENTICATION_AND_CIPHERING_REQUEST:
              MSG_HIGH_DS_0( MM_SUB, "=MM= RECEIVED AUTH. AND CIPHERING REQUEST MESSAGE");
              gmm_perform_authentication(message) ;
              break ;
            case GMM_AUTHENTICATION_AND_CIPHERING_REJECT:
              MSG_HIGH_DS_0( MM_SUB, "=MM= RECEIVED AUTH. AND CIPHERING REJECT MESSAGE" ) ;
              gmm_handle_authentication_reject() ;
              break ;
            case GMM_IDENTITY_REQUEST:
              MSG_HIGH_DS_0( MM_SUB, "=MM= RECEIVED IDENTITY REQUEST MESSAGE");
              gmm_handle_identity_request(message);

              break;
            default:
              MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message type %d",
                            message->cmd.mm_data_ind.L3_data[1], 0,0 );
          } /* end switch (gmm_message_type) */
          break;

        default:
          MSG_FATAL_DS(MM_SUB,
            "=MM= Unexpected RRC cmd %d", message->cmd.hdr.message_id, 0,0 );
      } /* end switch (message->cmd.hdr.message_id) */
      break;
      case MS_TIMER:
         switch (message->cmd.timer_expiry.timer_id)
         {
           case TIMER_T3318:
           case TIMER_T3320:
             auth_handle_timer_expiry(message) ;
             break ;
           default:
             MSG_FATAL_DS
                  (MM_SUB, "=MM= Unexpected timer expiry %d",
                                       message->cmd.timer_expiry.timer_id,0,0);
             break ;
         }
        break ;
    default:
      MSG_FATAL_DS
        ( MM_SUB, "=MM= Unexpected message set %d", message->cmd.hdr.message_set, 0,0 );
  }
} /* end gmm_handle_security() */


/*===========================================================================

FUNCTION    GMM_CANCEL_AUTHENTICATION

DESCRIPTION
  This function is call whenever the dedicated connection has been teared down
  or cunceled from any reason in order to stop possibly running Authentication
  server.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Stops internal mechanisms of the Authentication server.

===========================================================================*/
void gmm_cancel_authentication
(
  void
)
{
  if (auth_transaction_id != ((client_id)NULL))
  {
    auth_reset_transaction(auth_transaction_id) ;
    auth_transaction_id = ((client_id)NULL) ;
  }  
  mm_ps_auth_sent_to_card = FALSE;
} /* mm_cancel_authentication() */


/*===========================================================================

FUNCTION    GMM_HANDLE_IDENTITY_REQUEST

DESCRIPTION
  This function is called whenever the UE receives the Identity Request
  from the Network. IN the GMM Identification procedure the Mobile
  station shall select the mobile Identity type which was specified by
  the Network. Refer to section 10.5.1.4

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void gmm_handle_identity_request
(
  mm_cmd_type *message
)
{
  gmm_identity_request_msg_T incoming_request;
  gmm_ie_LV_resp_mobile_id_T mobile_identity = {0};
  byte identity_type, i;
  byte imei[MAX_IMEI_LEN+1],imeisv[MAX_IMEI_LEN+2];
  
  /* Clear imei buffer */
  memset((void *)imei,0,MAX_IMEI_LEN+1);
  memset((void *)imeisv,0,MAX_IMEI_LEN+2);

  /* Clear Identity request message message buffer */
  memset((void *)&incoming_request,0x00,sizeof(gmm_identity_request_msg_T)) ;

  /* convert incoming message into internal format */
  if (gmm_valid_identity_request_message(message,&incoming_request) == TRUE)
  {

#ifdef FEATURE_GSM_GPRS
    if (is_gsm_mode())
    {
      gmmllc_gmm_trigger_llc_event
        (GMM_TRIGGER_FORCE_TO_STANDBY,incoming_request.force_to_standby,0) ;
    }
#endif

    identity_type = incoming_request.identity_type;

    if (identity_type == GMM_IMSI)
    {
      /* -----------------------------------------------------------------------------
      * Use IMSI as mobile_identity - first set length of mobility identity contents
      ** ----------------------------------------------------------------------------- */
      if(mm_serving_plmn.info.sim_state == SYS_SIM_STATE_NOT_AVAILABLE)
      {
        /*
        * Sim not availabe, So NO IMSI. Sending NO_IDENTITY as type of Identity.
        */
        mobile_identity.length = GMM_IDENTIFICATION_PROC_NO_IDENTITY_LEN;
        /* ------------------------------------------------
        ** Set Identity type field (TS 24.008 Table 10.5.4)
        ** Type of identity
        **  Bits   3    2   1   
        **          0   0   0   No Identity (note 1)
        **  Bit 4 
        **       1 (Odd Identity Digits is used which is set to 0)
        **  Bits   8  7   6   5
        **          0  0   0   0  - Digits 1
        **  Byte 2, Byte 3 - Digits 2-5 all are set to '0's.
        ** ------------------------------------------------ */
        mobile_identity.value[0] = 0x08;
        mobile_identity.value[1] = 0x0;
        mobile_identity.value[2] = 0x0;
      }
      else
      {    
        mobile_identity.length = mm_stored_imsi.length;
        for (i=0; i < mm_stored_imsi.length; i++)
        {
          mobile_identity.value[i] = mm_stored_imsi.data[i];
        }
      }
    }

    else if (identity_type == GMM_TMSI)
    {
      /* -----------------------------------------------------------------------------
      * Use P-TMSI as mobile_identity - first set length of mobility identity contents
      ** ----------------------------------------------------------------------------- */
      if( !mm_has_valid_ptmsi() )
      {
        /*
        * NO P-TMSI.So,  Sending NO_IDENTITY as type of Identity.
        */
        mobile_identity.length = GMM_IDENTIFICATION_PROC_NO_IDENTITY_LEN;
        /* ------------------------------------------------
        ** Set Identity type field (TS 24.008 Table 10.5.4)
        ** ------------------------------------------------ */
        mobile_identity.value[0] = 0x08;
        mobile_identity.value[1] = 0x0;
        mobile_identity.value[2] = 0x0;        
      }
      else
      {      
        /* -----------------------------------------------------------------------------
        * Use PTMSI as mobile_identity - first set length of mobility identity contents
        ** ----------------------------------------------------------------------------- */
        mobile_identity.length = TMSI_SIZE + 1;

        /* ------------------------------------------------
        ** Set Identity type field (TS 24.008 Table 10.5.4)
        ** ------------------------------------------------ */
        mobile_identity.value[0] = 0xF4;

        for (i=0; i < PTMSI_SIZE; i++)
        {
          mobile_identity.value[i+1] = gmm_stored_ptmsi.digit[i];
        }
      }
    }

    else if (identity_type == GMM_IMEI )
    {
      mm_read_imei(imei);
      mobile_identity.length = imei[0];

      memscpy(mobile_identity.value, sizeof(mobile_identity.value), &imei[1], MIN(mobile_identity.length,MAX_IMEI_LEN));
    }

    else if (identity_type == GMM_IMEISV )
    {
    
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
      ghdi_ds_read_imeisv(imeisv,(sys_modem_as_id_e_type)mm_sub_id);    
#else
      read_imeisv(imeisv);
#endif

      mobile_identity.length = imeisv[0];
      memscpy(mobile_identity.value, sizeof(mobile_identity.value), &imeisv[1], mobile_identity.length);
    }
    else
    {
      MSG_FATAL_DS( MM_SUB, "=MM= Unknown identity type %d", identity_type, 0,0) ;
    }

    //Send the Identity response message to RRC
    gmm_send_identity_response_message(mobile_identity);
  }
  else
  {
    MSG_ERROR_DS_0( MM_SUB, "=MM= Invalid Identity Request message");
  }
} /* gmm_handle_identity_request() */
/*===========================================================================

FUNCTION    GMM_STOP_AUTH_FAIL_TIMERS

DESCRIPTION
   This function is called whenever Ps registration is failed in GRAT in order to stop
   T3320/T3318

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void gmm_stop_auth_fail_timers
(
  void
)
{
  if (auth_transaction_id != ((client_id)NULL))
  {
    auth_stop_auth_fail_timers(auth_transaction_id) ;
    auth_transaction_id = ((client_id)NULL) ;
  }
} /* mm_cancel_authentication() */
/*===========================================================================

FUNCTION    GMM_COMPARE_AUTH_DATA

DESCRIPTION
   This function compares the authentication data sent by the NW to the
   cached data.

DEPENDENCIES
  None

RETURN VALUE
  FALSE if the two data packages are different, TRUE otherwise

SIDE EFFECTS

===========================================================================*/
boolean gmm_compare_auth_data(gmm_anc_request_message_T message) 
{
    return (memcmp( &message , &gmm_auth_data, sizeof(gmm_anc_request_message_T)) == 0);
}
