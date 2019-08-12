/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmsecurity.c_v   1.24   01 Jul 2002 11:53:40   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmsecurity.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/22/07   ab      removed lint errors
05/14/01   jca     Added service_type parameter to mm_send_mmr_service_ind().
                   Fixed bug in mm_start_retransmission_timers().
05/25/01   jca     Updated to reflect changes to REG<->MM interface.
06/03/01   sbk     Global variables are renamed to use mm_ prefix
06/08/01   jca     Fixed bug in mm_stop_retransmission_timers().
06/20/01   sbk     Implemented security key/hfn management for SIM/USIM
                   Added support for security context management
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
08/15/01   ab      Rename CM to CNM.
09/18/01   sbk     Fixed a bug where RAND and AUTN lengths are not set
                   when running USIM auth algorithm
10/11/01   jc      Updated perform sim and usim functions to use new gsdi header type.
11/08/01   jca     Fixed AUTHENTICATION REJECT message state transition bug.
04/24/02   jca     Modifications to reflect changes to MM-CNM interface.
05/02/02   vdr     Added domain id to the MM/GMM Authentication request
08/02/02   ks      Setting proper cause in the mmr_service_ind primitive
08/09/02   jca     Stop the T3212 timer when AUTHENTICATION REJECT
                   message received (as per 24.008 spec).
10/01/02   vdr     Changed Authentication Server interface
11/06/02   jca     Fixed code to invalidate both CS and PS SIM info and status
                   when an Authentication Reject message is received (as per spec).
11/26/02   jca     Modified code to set MM PLMN info status through call to
                   new function mm_update_mm_serving_plmn_info().
02/10/03   cd      Fixed Lint errors
02/21/03   vdr     Fixed Authentication Request message parsing
04/16/03   vdr     Now forcing GSM AKA when MS is running 2G SIM
03/03/04   jca     If gmm_state is set to NULL, do not set gmm_state
                   to DEREGISTERED when authentication is rejected.
10/25/04   vdr     Now sending MMR_REG_CNF upon reception of the Authentication or 
                   Authentication and Ciphering Reject message when REG is waiting
                   for confirmation and MMR_SERVICE_IND otherwise
10/28/04   ks      Fixed a missing paranthesis.
12/17/04   vdr     Now performing cache authentication if TIMER_T3218 is running
                   in UMTS
03/21/05   sm      Handling the Network Authentication failure
10/06/05   ajt     Invalidate Authentication Cache while returning Authentication 
                   Failure to network.
12/09/05   vdr     Now performing same RAND authentication if UE is able to 
                   perform UMTS authentication
06/14/06   up      Called new mm_update_mm_serving_plmn_info for GSTK event.
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "ms.h"
#include "com_iei_v.h"
#include "mm_iei.h"
#include "mm_rr.h"
#include "mm_mm_v.h"
#include "mm_v.h"
#include "err.h"
#include "mm_umts_v.h"
#include "mm_sim.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mmsecurity.h"
#include "mmauth.h"
#include "gmm_int_v.h"
#include "mm_coord.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Current Authentication transaction ID */
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
/* Current Authentication transaction ID */
static client_id  auth_transaction_id_sim[MAX_NAS_STACKS] = { (client_id)NULL, (client_id)NULL ,(client_id)NULL } ;
#define auth_transaction_id auth_transaction_id_sim[mm_as_id]

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
/* Current Authentication transaction ID */
static client_id  auth_transaction_id_sim[MAX_NAS_STACKS] = { (client_id)NULL, (client_id)NULL } ;
#define auth_transaction_id auth_transaction_id_sim[mm_as_id]

#else
static client_id  auth_transaction_id = (client_id)NULL;
#endif /* FEATURE_TRIPLE_SIM*/

static boolean mm_compare_auth_data(auth_rand_type rand_value, auth_autn_type autn, 
  byte  ciphering_key_sequence_number);
static void mm_store_auth_data(auth_rand_type rand_value, auth_autn_type autn, 
  byte  ciphering_key_sequence_number);

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE  
static auth_rand_type rand_value_cache_sim[MAX_NAS_STACKS];
static auth_autn_type autn_cache_sim[MAX_NAS_STACKS];
static byte  ciphering_key_sequence_number_cache_sim[MAX_NAS_STACKS];
#define rand_value_cache rand_value_cache_sim[mm_as_id]
#define autn_cache autn_cache_sim[mm_as_id]
#define ciphering_key_sequence_number_cache ciphering_key_sequence_number_cache_sim[mm_as_id]
#else
static auth_rand_type rand_value_cache;
static auth_autn_type autn_cache;
static byte  ciphering_key_sequence_number_cache;
#endif

/*===========================================================================

FUNCTION MM_GET_TIMER_LIST

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_get_timer_list
(
  byte *timer_list_length,
  auth_timer_list_type *timer_list
)
{
  *timer_list_length = 3 ;

  timer_list[0].timer = TIMER_T3210 ;

  timer_list[1].timer = TIMER_T3220 ;

  timer_list[2].timer = TIMER_T3230 ;
} /* end mm_get_timer_list() */


/*===========================================================================

FUNCTION MM_HANDLE_AUTHENTICATION_REJECT

DESCRIPTION
  This function responds to the authentication reject indication from
  the network. The function is activated upon reception of the Authentication
  Reject message. The rection to this is similar to that when a reject
  is received with cause 6, illegal ME.
  If an MM connection exists, then the CNM is requested to abort all
  connections,after which it will send MMCNM_REL_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_handle_authentication_reject
(
  void
)
{
  byte mm_timer_list_length ;
  auth_timer_list_type mm_retx_timer_list[MAX_TIMER_ARRAY_LENGTH] ;

  /*
  ** Retrieve MM retx timers
  */
  mm_get_timer_list(&mm_timer_list_length,mm_retx_timer_list) ;

  /*
  ** Call the Authentication server in order to initiate the processing
  ** of the Authentication Reject message
  */
  auth_handle_network_reject(
          auth_transaction_id,mm_timer_list_length,mm_retx_timer_list) ;

  /*
  ** Authentication transaction has been finished, clear the transaction id
  */
  auth_transaction_id = (client_id)NULL ;

  mm_perform_reject_actions(RRC_CS_DOMAIN_CN_ID);

  /* ----------------------------------------------------------------
  ** The T3212 timer is stopped when an AUTHENTICATION REJECT message
  ** is received (3GPP TS 24.008 section 4.2.2)
  ** ---------------------------------------------------------------- */
  mm_stop_timer( TIMER_T3212 );

  mm_start_timer ( TIMER_T3240, DEFAULT_TIMEOUT );

  mm_state_control( MM_STD_AUTHENTICATION_REJECT );

  /* this will call CM_REG_REJECT_IND with reject cause 255 to indicate 
  this reject is due to authentication failure*/
  
  mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_ONLY, 
                                      255,
                                      mm_serving_plmn.info.plmn,
                                      mm_serving_plmn.info.lac
                      #ifdef FEATURE_FEMTO_CSG
                                    , mm_serving_plmn.info.csg_info.csg_id
                      #endif 
                                    , mm_serving_plmn.info.active_rat
                            );

 

} /* end mm_handle_authentication_reject() */


/*===========================================================================

FUNCTION MM_HANDLE_SECURITY

DESCRIPTION
  This function routes the messages relating to security to the
  appropriate function for detailed processing

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_handle_security(mm_cmd_type *msg_p)
{

   if( is_OTA_message(msg_p) )
   {
      switch (get_OTA_msg_type( msg_p ))
      {
         case IDENTITY_REQUEST:
            mm_return_identity( msg_p );
            break;

         case AUTHENTICATION_REQUEST:
            mm_perform_authentication( );
            break;

         case AUTHENTICATION_REJECT:
            mm_handle_authentication_reject( );
            break;

         case TMSI_REALLOCATION_COMMAND:
            mm_perform_tmsi_reallocation( msg_p );
            break;

         default:
            MSG_FATAL_DS
                  (MM_SUB, "=MM= Unexpected message type %d",
                                          msg_p->cmd.rr_data_ind.layer3_message[1],0,0);
            break;
      }

   }
   else
   {
     if (msg_p->cmd.hdr.message_set == MS_TIMER)
     {
       switch (msg_p->cmd.timer_expiry.timer_id)
       {
         case TIMER_T3214:
         case TIMER_T3216:
           auth_handle_timer_expiry(msg_p) ;
           break ;
         default:
           MSG_FATAL_DS
                (MM_SUB, "=MM= Unexpected message expiry %d",
                                         msg_p->cmd.timer_expiry.timer_id,0,0);
           break ;
       }
     }
     else
     {
      MSG_FATAL_DS(MM_SUB, "=MM= Unexpected message set %d",msg_p->cmd.hdr.message_set,0,0);
     }
   }
} /* end mm_handle_security() */


/*===========================================================================

FUNCTION MM_PARSE_AUTH_REQUEST

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_parse_auth_request
(
  byte *ciphering_key_sequence_number,
  auth_rand_type *p_rand,
  auth_autn_type *autn
)
{
  byte ie_count ;

  /* Reset the length of the optional AUTN field */
  autn->value_length = 0 ;

  /* Read parser results */
  for ( ie_count=0; ie_count < mm_no_of_ies; ie_count++ )
  {
    switch (mm_IE_info[ie_count].IEI)
    {
      case IEI_CIPHER_KEY_SEQ_NUMBER:
        *ciphering_key_sequence_number =
                                     *((byte *)mm_IE_info[ie_count].start) & 0x0F;
        break ;
      case IEI_RAND :
        /* RAND is always 16 octets long */
        p_rand->value_length = 16 ;
        /* Read the RAND field */
        memscpy((void *)p_rand->value_data,MAX_AUTH_DATA_LENGTH,(void*)(mm_IE_info[ie_count].start+1),16) ;
        break ;
      case IEI_AUTN :
        /* Read length of the AUTN context */
        /**************************************************************************** 
        *From section 10.5.3.1.1 of 24.008,MAX length of AUTN is 18 bytes.So its
        *value part is max of 16 bytes.From section 11.4.2 of 24.007 network can send
        *more than 16 bytes of AUTN value.If Network sends the AUTN value more than
        *16 bytes,take only 16 bytes form the network given value since AUTN value part
        *can have MAX of 16 bytes only as per 24.008            
        ****************************************************************************/                                                                               ;
        autn->value_length = MIN(mm_IE_info[ie_count].start[1],MAX_AUTH_DATA_LENGTH) ;
        /* Read the AUTN context */
        memscpy((void *)autn->value_data,MAX_AUTH_DATA_LENGTH,
                      (void *)&mm_IE_info[ie_count].start[2],autn->value_length) ;
        if (mm_IE_info[ie_count].start[1] > MAX_AUTH_DATA_LENGTH)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= AUTN Length %d is greater than %d",
                mm_IE_info[ie_count].start[1], MAX_AUTH_DATA_LENGTH, 0);
        }
        break ;
      default :
        break ;
    } /* switch */
  } /* for */

  /* Authentication Request message has been successfully parsed */
  return TRUE ;
} /* end mm_parse_auth_request() */


/*===========================================================================

FUNCTION BUILD_AUTHENTICATION_RESPONSE_MESSAGE

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void build_authentication_response_message
(
  auth_res_type *auth_res,
  byte *message_buffer_length,
  mm_authentication_message_buffer_type *response_message_buffer
)
{
  byte rest_copied_length ;
  byte *message_buffer = response_message_buffer->message_body ;

  if (auth_res->value_length > MAX_AUTH_DATA_LENGTH)
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Authentication data length %d is greater than %d", 
                  auth_res->value_length, MAX_AUTH_DATA_LENGTH,0) ;   
  }

  /* Protocol Discriminator and Skip Indicator */
  response_message_buffer->gmm_msg_head.skip_pd = PD_MM;

  /* Message type */
  response_message_buffer->gmm_msg_head.type = AUTHENTICATION_RESPONSE ;

  /* Set the RES value */
  memscpy((void *)&message_buffer[0],sizeof(response_message_buffer->message_body),(void *)auth_res->value_data,4) ;

  *message_buffer_length = 6 ;

  /* Set extended part of the RES value */
  if ((rest_copied_length=auth_res->value_length-4)>0)
  {   
    void *buffer_ptr = (void *)(((byte *)auth_res->value_data) + 4);

    message_buffer[4] = IEI_AUTH_RSP_EXT ;

    message_buffer[5] = rest_copied_length ;

    memscpy((void *)&message_buffer[6],sizeof(response_message_buffer->message_body)-6,buffer_ptr, 
                       MIN(rest_copied_length, (MAX_AUTH_DATA_LENGTH - 4))) ;

    *message_buffer_length += (2 + rest_copied_length) ;
  }
} /* end build_authentication_response_message() */


/*===========================================================================

FUNCTION BUILD_AUTHENTICATION_FAILURE_MESSAGE

DESCRIPTION
  None
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void build_authentication_failure_message
(
  auth_error_type auth_error,
  auth_auts_type *auth_auts,
  byte *message_buffer_length,
  mm_authentication_message_buffer_type *failure_message_buffer
)
{
  byte *message_buffer = failure_message_buffer->message_body ;

  if (auth_auts->value_length > MAX_AUTH_DATA_LENGTH)
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Authentication data length %d is greater than %d", 
                  auth_auts->value_length, MAX_AUTH_DATA_LENGTH,0) ;   
  }

  /* Protocol Discriminator and Skip Indicator */
  failure_message_buffer->gmm_msg_head.skip_pd = PD_MM;

  /* Message type */
  failure_message_buffer->gmm_msg_head.type = AUTHENTICATION_FAILURE ;

  message_buffer[0] = (byte) auth_error;

  *message_buffer_length = 3 ;

  if (auth_auts->value_length > 0)
  {
    message_buffer[1] = 0x22 ;

    message_buffer[2] = auth_auts->value_length ;
    memscpy((void *)&message_buffer[3],sizeof(failure_message_buffer->message_body)-3, (void *)auth_auts->value_data,
                      MIN(auth_auts->value_length, MAX_AUTH_DATA_LENGTH)) ;

    *message_buffer_length += (2 + auth_auts->value_length);
  }

} /* end build_authentication_failure_message() */


/*===========================================================================

FUNCTION MM_HANDLE_COMMON_AUTH_RESPONSE

DESCRIPTION
  This function deletes the loaded security context from the SIM and the
  mm_security_context varible

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_handle_common_auth_response
(
  /* Functional Authentication Status code */
  auth_status_type status_code,

  /* Authentication Response Error code */
  auth_error_type auth_error,

  /* Pointer to Authentication Response Value (RES) */
  auth_res_type *auth_res,

  auth_auts_type *auth_auts
)
{
  byte message_buffer_lenght ;
  mm_authentication_message_buffer_type message_buffer ;

  memset(&message_buffer, 0x00, sizeof(mm_authentication_message_buffer_type));

  if (status_code == AUTHENTICATION_DONE)
  {
     auth_transaction_id = (client_id)NULL ;
  }else if (status_code == AUTHENTICATION_NETWORK_FAILURE) 
  {
    auth_transaction_id = (client_id)NULL ;
    MSG_HIGH_DS_0(MM_SUB, "=MM= Network failed in MS Authentication");
    return;
  }

  switch (auth_error)
  {
    case AUTH_NO_ERROR:
      build_authentication_response_message( auth_res,&message_buffer_lenght,&message_buffer );
      MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent AUTHENTICATION RESPONSE");
      break ;
    case AUTH_MAK_CODE_FAILURE:
    case AUTH_SQN_FAILURE:
    case AUTH_AUTHENTICATION_UNACCEPTABLE:
      build_authentication_failure_message(
      auth_error,auth_auts,&message_buffer_lenght,&message_buffer );
      auth_reset_cache_memory(FALSE, NO_TIMER_EXPIRED, RRC_CS_DOMAIN_CN_ID);           
      mm_send_mmr_reg_reject_ind(SYS_SRV_DOMAIN_CS_ONLY, 
                                 (byte)auth_error,
                                 mm_serving_plmn.info.plmn,
                                 mm_serving_plmn.info.lac
                                 #ifdef FEATURE_FEMTO_CSG
                                 , mm_serving_plmn.info.csg_info.csg_id
                                 #endif 
                                 , mm_serving_plmn.info.active_rat
                            );
      MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent AUTHENTICATION FAILURE");
      break ;
    default:
      MSG_FATAL_DS(MM_SUB, "=MM= Illigal authentication error code returned",0,0,0) ;
      break ;
  }

  /* Send appropriate MS response to the network */

  mm_send_ota_message((byte *)&message_buffer,message_buffer_lenght) ;

} /* end mm_handle_common_auth_response() */
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
void mm_handle_sim_busy_ind(boolean cancel_auth)
{
   if((auth_transaction_id != ((client_id)NULL) && auth_is_procedure_to_be_aborted(auth_transaction_id))
	   || cancel_auth == TRUE )
   {
     mm_cancel_authentication();
     mmgsdi_session_cancel_requests(mm_sim_mmgsdi_info.session_id);
       
      if(mm_state == MM_LOCATION_UPDATE_INITIATED)
      {
         mm_lu_reject_info.reject_cause = CAUSE_SIM_BUSY;
      }
      else if((mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
	  	   (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION))
      {
 #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
          mm_send_mmcnm_est_rej(
                     MM_REJECT_CAUSE, MM_REJ_SIM_BUSY,MMCC_SMS_CS);
#else
          mm_send_mmcnm_est_rej(
                     MM_REJECT_CAUSE, MM_REJ_SIM_BUSY );
#endif 

      }

      if((mm_state == MM_LOCATION_UPDATE_INITIATED) ||
	  (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION))
      {

        if(is_umts_mode())
        {
          if(ps_session_status == MMRRC_IDLE)
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
           MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RRC_ABORT_REQ");
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
	       rrc_cmd_p->cmd.close_sess_req.cn_domain_id = RRC_CS_DOMAIN_CN_ID;
		   	
           MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RRC_CLOSE_SESSION_REQ");
           send_cmd_to_rrc(rrc_cmd_p, mm_serving_plmn.info.active_rat );
         }
       }
       else
       {

          if(mm_state == MM_LOCATION_UPDATE_INITIATED)
          {
            mm_state_control( MM_STD_LU_REJECT );
          }
          else 
          {
            mm_state_control( MM_STD_RR_CONNECTION_ABORTED );
          }
                  
          MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RR_ABORT_REQ");
          mm_send_rr_abort_req(FALSE);
       }
      }
    }
}
#endif
/*===========================================================================

FUNCTION MM_PERFORM_AUTHENTICATION

DESCRIPTION
  This function responds to an authentication request received from
  the network. The mobile station uses the challenge information in
  the input message to calculate the new ciphering key and the
  signed response, which is returned to the network for verification.

  Extended to support UMTS as well as GSM challenges


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_perform_authentication(void)
{
  auth_rand_type rand_value ;
  auth_autn_type autn ;
  byte  ciphering_key_sequence_number ;
  auth_request_properties_type  request_properties ;

  if (mm_parse_auth_request(&ciphering_key_sequence_number,&rand_value,&autn)==TRUE)
  {
    /* If the MAK/SQN Timers are already running, stop them */
    MSG_HIGH_DS_0(MM_SUB, "=MM= Perform Authentication, stop MAK/SQN timers if running ");
    if (auth_transaction_id != ((client_id)NULL))
    {
      auth_stop_auth_mak_sqn_timers(auth_transaction_id) ;
    }

    if ((mm_sim_card_mode == MMGSDI_APP_USIM) && 
        (mm_timer_status [ TIMER_T3218 ] == TIMER_ACTIVE))
    {
      if (auth_mm_perform_cache_authentication(rand_value.value_length,
                          (byte *)rand_value.value_data, 
                                              mm_handle_common_auth_response))
      {
        return ; /* Authentication completed */
      }
    }
    /* Check if SIM is already processing a request*/
    if (mm_cs_auth_sent_to_card) 
    {
        /* Check if the new request is identical */
        if (mm_compare_auth_data(rand_value,autn, ciphering_key_sequence_number)) 
        {            
            MSG_HIGH_DS_0(MM_SUB, "=MM= Back to back same Auth Req, ignore") ;
            return;
        }          
    }
     
    mm_store_auth_data(rand_value,autn, ciphering_key_sequence_number);

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
    if(mm_is_sim_busy)
    {
      mm_handle_sim_busy_ind(TRUE);
      return;
    }
#endif
    request_properties.ciphering_key_sequence_number =
                                                ciphering_key_sequence_number ;

    request_properties.ciphering_algorithm = FIELD_NOT_IN_USE ;

    request_properties.cn_domain_identity = RRC_CS_DOMAIN_CN_ID ;

    request_properties.client_response_callback = mm_handle_common_auth_response ;

    request_properties.client_timer_set.mak_failure_timer = TIMER_T3214 ;

    request_properties.client_timer_set.sqn_failure_timer = TIMER_T3216 ;

    request_properties.client_timer_set.num_failures = 0;

    request_properties.cache_guard_timer_id = TIMER_T3218 ;

    /* Initialize GMM re-tx timer list */
    mm_get_timer_list(
                &request_properties.client_timer_set.timer_list_length,
                              request_properties.client_timer_set.timer_list) ;

    /* According to TS 24.008 4.3.2.2 An MS which does not support the UMTS
    ** authentication algorithm shall ignore the Authentication Parameter AUTN
    ** if included in the AUTHENTICATION REQUEST message and shall proceed as
    ** in case of a GSM authentication challenge.
    **/
    if ((mm_sim_card_mode == MMGSDI_APP_SIM) && 
        (autn.value_length > 0))
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= Due to lack of UMTS auth capability force GSM auth") ;

      autn.value_length = 0 ;
    }

    if (autn.value_length == 0)
    {
      request_properties.current_auth_request_context = MMGSDI_AUTN_USIM_GSM_CONTEXT;
    }
    else
    {
      request_properties.current_auth_request_context = MMGSDI_AUTN_USIM_3G_CONTEXT;
    }

    auth_transaction_id = auth_authentication_request(
               ciphering_key_sequence_number,&rand_value,&autn,&request_properties) ;
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to perform auth. due to illigal request format") ;
  }
} /* end mm_perform_authentication() */


/*===========================================================================

FUNCTION MM_CANCEL_AUTHENTICATION

DESCRIPTION
  This function is call whenever the dedicated connection has been teared down
  or cunceled from any reason in order to stop possibly running Authentication
  server.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_cancel_authentication
(
  void
)
{
  if (auth_transaction_id != ((client_id)NULL))
  {
    auth_reset_transaction(auth_transaction_id) ;
    auth_transaction_id = ((client_id)NULL) ;
  }  
  mm_cs_auth_sent_to_card = FALSE;  
} /* mm_cancel_authentication() */


/*===========================================================================

FUNCTION MM_PERFORM_REJECT_ACTIONS

DESCRIPTION
  This function responds to the authentication reject indication from
  the network. The function is activated upon reception of the Authentication
  Reject message by the GMM module or by the Auth server.
  The rection to this is similar to that when a reject
  is received with cause 6, illegal ME.
  If an MM connection exists, then the CNM is requested to abort all
  connections,after which it will send MMCNM_REL_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_perform_reject_actions( rrc_cn_domain_identity_e_type domain_id)
{
  mmr_service_ind_s_type  mmr_service_ind;
  sm_cmd_type             *sm_cmd = NULL;

  /*---------------------------------------------------------
    Delete the ciphering keys, LAI and set the update status
    Mark SIM as illegal delete
  *--------------------------------------------------------*/
  mm_delete_lu_status( ROAMING_NOT_ALLOWED );

  mm_substate_control( MM_STD_NO_IMSI );

  if (mm_cnm_is_active)
  {
    mm_send_mmcnm_rel_ind( OTHER_CAUSE, FALSE );
  }

  /* ------------------------------------------------------------------------------------
  ** If the authentication procedure is performed by MM and the authentication is
  ** rejected by the network (i.e upon receive of AUTHENTICATION REJECT), the MS shall
  ** in addition set the GPRS update status to GU3 ROAMING NOT ALLOWED and shall, if
  ** available, delete the P-TMSI, P-TMSI signature, RAI and GPRS ciphering key sequence
  ** number stored. The SIM shall be considered as invalid for GPRS and non-GPRS services
  ** until switching off or the SIM is removed. The MS shall abort any GMM procedure and
  ** shall enter state GMM-DEREGISTERED.
  ** ------------------------------------------------------------------------------------ */
  gmm_delete_ps_location_information();

  if (gmm_state != GMM_NULL)
  {
    gmm_set_state( GMM_DEREGISTERED );

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

    MSG_HIGH_DS_0(MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");

    sm_put_cmd( sm_cmd );
  }

  gmm_set_substate( GMM_NO_IMSI );

  mm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_PS_INVALID,
                                  SYS_SRV_DOMAIN_NO_SRV,
                                  TRUE,
                                  SYS_SRV_STATUS_LIMITED,
                                  TRUE );
  mmsim_mark_sim_as_illegal();

  /*
  * Tell REG that service is now available
  */
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
    mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
    mmr_service_ind.service_state = mm_serving_plmn.info;
    mm_send_mmr_service_ind( &mmr_service_ind );
  }

  if (domain_id == RRC_CS_DOMAIN_CN_ID)
  {
    /* Reset stored RAND and RES */
    auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED,RRC_CS_DOMAIN_CN_ID) ;
  }
} /* end mm_perform_reject_actions() */
/*===========================================================================

FUNCTION    MM_COMPARE_AUTH_DATA

DESCRIPTION
   This function compares the authentication data sent by the NW to the
   cached data.

DEPENDENCIES
  None

RETURN VALUE
  FALSE if the two data packages are different, TRUE otherwise

SIDE EFFECTS

===========================================================================*/
boolean mm_compare_auth_data(auth_rand_type rand_value, auth_autn_type autn, 
  byte  ciphering_key_sequence_number) 
{

    if ((rand_value_cache.value_length == rand_value.value_length) && 
        (memcmp( &rand_value_cache.value_data , &rand_value.value_data, rand_value_cache.value_length) == 0) &&
        (autn_cache.value_length == autn.value_length) &&
        (memcmp( &autn.value_data , &autn.value_data, autn_cache.value_length) == 0) &&
        (ciphering_key_sequence_number_cache == ciphering_key_sequence_number))
    {
        return TRUE;
    }
    MSG_HIGH_DS_0(MM_SUB, "=MM= Different Auth Req") ;
    return FALSE;
}
/*===========================================================================

FUNCTION    MM_STORE_AUTH_DATA

DESCRIPTION
   This function stores the auth data passed in from the NW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void mm_store_auth_data(auth_rand_type rand_value, auth_autn_type autn, 
  byte  ciphering_key_sequence_number)
{
    ciphering_key_sequence_number_cache = ciphering_key_sequence_number;

    rand_value_cache.value_length= rand_value.value_length;
    memset((void *)&rand_value_cache.value_data[0], 0, MAX_AUTH_DATA_LENGTH);
    
    memscpy((void *)&rand_value_cache.value_data[0], rand_value.value_length, 
        (void *)&rand_value.value_data[0], rand_value.value_length);

    autn_cache.value_length = autn.value_length;
    memset((void *)&autn_cache.value_data[0], 0, MAX_AUTH_DATA_LENGTH);
    memscpy((void *)&autn_cache.value_data[0], autn.value_length, 
        (void *)&autn.value_data[0], autn.value_length);
}
