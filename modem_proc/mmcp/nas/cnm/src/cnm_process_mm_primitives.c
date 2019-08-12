/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_process_mm_primitives.c_v   1.9   12 Jul 2002 17:17:30   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_process_mm_primitives.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

09/17/01    CD     In CNM_process_mm_reestablishmnet(), CNM_process_mm_conn_reject(), 
                   and CNM_process_mm_establishment_lost() removed sms and ss 
                   state machine actions and called the appropriate handling 
                   functions instead

                   Deleted cm_sms.h since it is no longer used

                   In CNM_process_mm_establishment_lost(), corrected the check for 
                   SMS states WAIT_FOR_CP_ACK and CONN_REESTABLISH

19-Nov-2001  VT    In CNM_process_mm_establishment_lost(), for each transaction_id 
                     (for CC), while sending mncc_rel_ind 
                     if attempt_reestablish = FALSE, also stop all timers by calling
                     CNM_stop_timer().

01/25/02     CD    Changed CNM_process_mm_conn_rej() to send to CM/UI cause value of NORMAL_UNSPECIFIED
                   as defined in cause.h rather than internal LLC causes

03/27/02     RG    Fix for CR19242(NAS timers are not cleaned up for RF failure scenarios): All CC
                   timers are stopped when processing
                   the MM-connection-reject from MM (due to an RRC connection release from RL failu
                   re).
                   Change: CNM_process_mm_conn_reject() line 215: TIMER_T303 changed to ALL_CC_TIME
                   RS.

04/19/02    CD     Changed function definition of CNM_process_mm_conn_reject() and 
                   CNM_process_mm_establishment_lost()

                   Changed CNM_process_mm_reestablishment() to process a reestablishemnt
                   based on the message id (MMCC_REEST_CNF/REJ) instead of the 
                   reestablishment success flag
                
                   Updated Copyright

06/27/02     CD    In CNM_process_mm_conn_reject(), CNM_process_mm_establishment_lost()
                   and CNM_process_mm_reestablishment() corrected passing the local and 
                   nw release causes to MN

                   Fixed some 'unusual pointer cast' lint errors

07/29/02     CD    Modified CNM_process_mm_establishment_lost(), 
                   CNM_process_mm_reestablishment() for call re-establishment scenarios

09/04/02     AB    Modified CNM_process_mm_establishment_lost() to remove compiler
                   warning of nested comments.

09/16/02     AB    In CNM_process_mm_conn_reject(), passed on the reject cause
                   from MM instead of NORMAL_UNSPECIFIED reason/value.

02/12/02     AB    Added reference to mmcnm_rel_ind in 
                   CNM_process_mm_establishment_lost() per Lint.

                   In CNM_process_mm_conn_accepted(), set the perfer domain (CS or PS) 
                   for SMS data request to MM.

04/16/03     AB    In CNM_process_mm_conn_reject(), added supports for new
                   reject type and reject value received from MM layer.

                   Also, sent MNCC_REJ_IND instead of MNCC_REL_IND to MM on
                   connection reject.

09/10/03     AB    Removed last end call deferred function.

12/15/04     AB    Added supports for AT+CMMS=1, ATCOP multiple MO SMS.

9/12/11      PM    Fix for CR 272613.

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "com_iei_v.h"
#include "ms.h"
#include "mn.h"
#include "cc_cc.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"

#include "cmlib.h"
#include "cnm.h"

#include "nasutils.h"
#include "mnglobal.h"
#include "cm_sms.h"

/**************** Begin of   CNM_process_mm_conn_accepted *************************
 *
 *  Function name: CNM_process_mm_conn_accepted()
 *  --------------------------------------------
 *
 *  Description:
 *  ------------
 *  Process the response to a request for an MM connection.
 * Send out the next request in the queue where the cnm_request_id
 * matches that of an element in the relevant cnm transaction store
 * and where the state is 'pending MM connection', if one is there.
 * Update the mm_connection_pending store.
 *
 *
 *  Parameters:
 *  -----------
 *  CNM_input_primitive : IN  - (IMH_T *)
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mm_conn_accepted( void )
{
   connection_id_T   connection_id;

   connection_id = mm_connection_pending.connection_id;

   if ( ( mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT ) ||
        ( mm_connection_pending.CNM_service_type == EMERGENCY_CALL_ESTABLISHMENT ) )
   {
       if(cc_transaction_information_p[connection_id] != NULL)
       {
         cc_transaction_information_p[connection_id]->cc_state = CC_STATE_U1;
       }
       else
       {
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot set cc_state to CC_STATE_U1 as cc_transaction_information_p[%d] is NULL",
                            connection_id);
       }
   }

   /* 
    *  Adjust the prefer radio link DOMAIN (CS or PS) for SMS
    */ 
   if((connection_id < MAXNO_SMS_TRANSACTIONS) &&
      (mm_connection_pending.CNM_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT))
   {
       mm_connection_pending.mmcnm_data_req.header.message_set = 
                        sms_transaction_information[connection_id].pref_rl_domain;

       /* AT +CMMS=1: needed a MM connection to transport CP_ACK */
#ifndef FEATURE_NAS_LONG_SMS_BRANCH_A
       /* Now MS doesn't have to wait until the CM SERVICE ACCEPT is received for sending
        * the defferred CP-ACK. As the defered final CP-ACK would have already been sent immediately after the 
        * CM SERVICE REQ Message is transfered OTA in BRANCH-A Behaviour, the below block is not needed */

       if(sms_link_control.cp_ack_deferred)
       {
         if(sms_transaction_information[sms_link_control.connection_id].pref_rl_domain != MS_CC_MM_PS)
         {
           CNM_send_cp_ack(sms_link_control.connection_id);
         }    
         sms_link_control.cp_ack_deferred = FALSE;  
       }
#endif /* FEATURE_NAS_LONG_SMS_BRANCH_A */

   }

   CNM_send_message( (IMH_T *)&mm_connection_pending.mmcnm_data_req, GS_QUEUE_MM );

   CNM_send_est_request_from_service_queue();
   
}
/**************** End of   CNM_process_mm_conn_accepted ***************************/


/**************** Begin of   CNM_process_mm_conn_reject ***************************
 *
 *  Function name: CNM_process_mm_conn_reject()
 *  ------------------------------------------
 *
 *  Description:
 *  ------------
 *  Process a MM connection release indication where a call reestablishment
 *  is not possible or a MM reestablishment request has been rejected. 
 *  
 *
 *  Parameters:
 *  -----------
 *  CNM_input_primitive : IN  - mmcnm_rel_ind_T or mmcc_reest_rej
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mm_conn_reject( IMH_T *CNM_input_primitive)
{
   mmcnm_est_rej_T      *mmcnm_rej_ind;
   transaction_id_T     transaction_id;
   cnm_request_id_T     cnm_request_id;
   boolean              msg_present;
   boolean              rlf_present;

   mm_link_reestablishing = FALSE;

   mmcnm_rej_ind = (mmcnm_est_rej_T *)CNM_input_primitive;


   FOREVER
   {
      /* Get any pending CNM SERVICE REQ messages.
       * If the request is for CC or SS, send a reject indication
       * to MN
       */
      msg_present = CNM_pop_cnm_service_request_queue(&cnm_request_id);

      if ( msg_present != FALSE )
      {
          CNM_send_reject_message( &cnm_request_id, 
                                   mmcnm_rej_ind->cause_type, 
                                   mmcnm_rej_ind->cause_value);
      }
      else
      {
          break;
      }
   } 
 
   /*
    * Now check for pending connection, generating a reject message if
    * there is one
    */
   if(mm_connection_pending.CNM_service_type != NO_CNM_SERVICE)
   {
       CNM_send_reject_message(&mm_connection_pending,
                               mmcnm_rej_ind->cause_type, 
                               mmcnm_rej_ind->cause_value);
   }

   /* Release the rest of the remaining active transactions */
   for ( transaction_id = 0; transaction_id < MAXNO_CC_TRANSACTIONS; transaction_id++ )
   {
      if ( cc_transaction_information_p[transaction_id] != NULL && cc_transaction_information_p[transaction_id]->cc_state != CC_STATE_U0 )
      {
            cnm_request_id.connection_id = transaction_id;
            cnm_request_id.CNM_service_type = MO_CALL_ESTABLISHMENT;  /* assign default value */

            if((mmcnm_rej_ind->cause_value == OTHER_CAUSE || mmcnm_rej_ind->cause_value == AS_REJ_DEEP_FADE)&&
               ((cc_transaction_information_p[transaction_id]->cc_state == CC_STATE_U11 ) ||      /*  call is Disconnected from UE*/
                (cc_transaction_information_p[transaction_id]->cc_state == CC_STATE_U12 ) ||     /* Disconnect with progress ind from Network*/
                 (cc_transaction_information_p[transaction_id]->cc_state == CC_STATE_U19 )))     /* Disconnect from Network*/
            {
               rlf_present= TRUE;  
            }
            else
            {
               rlf_present= FALSE;
            }

            /* Fix for CR 19242(NAS timers not cleared properly) */
            /* All active CC timers are stopped below by saying ALL_CC_TIMERS as opposed to TIMER_T303
             * only that was stopped previously.
             */
            CNM_stop_timer(ALL_CC_TIMERS, (unsigned char)FORM_TI_PD( transaction_id, PD_CC ));

            MN_stop_cc_timer(transaction_id);

            CNM_clear_dtmf_queue(transaction_id);

            CNM_set_state_to_U0( transaction_id );

            if((mn_call_information_p[transaction_id] != NULL) &&
               (mn_call_information_p[transaction_id]->cause_before_gsm_reestablishment != 0))
            {
               CNM_fill_mmcnm_rej_ind(mmcnm_rej_ind, mn_call_information_p[transaction_id]->cause_before_gsm_reestablishment);
               mn_call_information_p[transaction_id]->cause_before_gsm_reestablishment  = 0;
            }
            else if(tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == FALSE)	/* no RAB */
            {
               mmcnm_rej_ind->cause_type = CNM_MN_REJECT_CAUSE;
               mmcnm_rej_ind->cause_value = CNM_REJ_NO_RESOURCES;
            }

            /* change the cause when RLF happens or CS signalling connection release with cause deep fade during Disconnect*/
            if (rlf_present == TRUE)
            {
              if(mmcnm_rej_ind->cause_value == OTHER_CAUSE)
              {
                 CNM_send_reject_message(&cnm_request_id,
                                    mmcnm_rej_ind->cause_type, 
                                    RLF_DURING_CC_DISCONNECT);
              }
              else
              {
                 CNM_send_reject_message(&cnm_request_id,
                                    mmcnm_rej_ind->cause_type, 
                                    RLF_DEEP_FADE_CC_DISCONNECT);
              }
            }
            else

            {
			  CNM_send_reject_message(&cnm_request_id,
                                    mmcnm_rej_ind->cause_type, 
                                    mmcnm_rej_ind->cause_value);
            }
      }
   }
   transaction_id = 0x07;

   MN_clear_call_data(transaction_id);
 
}

/**************** End of   CNM_process_mm_conn_reject *****************************/


/**************** Begin of   CNM_process_mm_establishment_lost ********************
 *
 *  Function name: CNM_process_mm_establishment_lost()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This module processes the notification of loss of the MM connections and
 *  sends a MMCNM_EST_REQ primitive to MM to attempt to re-establish the MM
 *  connection
 *
 *  Parameters:
 *  -----------
 *  CNM_input_primitive : IN  - (IMH_T *)  (the rejection message)
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mm_establishment_lost( mmcnm_rel_ind_T *mmcnm_rel_ind, CNMServiceType_T CNM_service_type )
{
   mmcc_reest_req_T     mmcc_reest_req;
   MNCC_ERR_IND_T       mncc_err_ind;
   transaction_id_T     transaction_id;


   /* reest_possible already been checked by caller */
   if(mmcnm_rel_ind)
   {
      MSG_LOW_DS_1(MN_SUB,"=MNCNM= Attempting to re-establish link=%d", mmcnm_rel_ind->reest_possible); /* per Lint */
   
      mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;
   
      /* Remove all entries from the cnm_service request queue */
      CNM_init_cnm_service_request_queue();
   
      /*
       * attempt reestablishment
       */
      mmcc_reest_req.header.message_set = MS_CC_MM;
      mmcc_reest_req.header.message_id  = MMCC_REEST_REQ;
	  mmcc_reest_req.CNM_service_type  = CNM_service_type;
#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK) /* DSDS */
      {
        mmcc_reest_req.as_id =  mmcnm_rel_ind->as_id;
        PUT_IMH_LEN(sizeof(mmcc_reest_req) - sizeof(IMH_T), 
                          &mmcc_reest_req.header);
      }
#ifdef FEATURE_DUAL_ACTIVE      
      else 
      {
        mmcc_reest_req.as_id =  (sys_modem_as_id_e_type)mn_dsda_as_id;
        PUT_IMH_LEN(sizeof(mmcc_reest_req) - sizeof(IMH_T), 
                          &mmcc_reest_req.header);
      }
#endif      
#else
      {
        PUT_IMH_LEN(sizeof(mmcc_reest_req) - sizeof(IMH_T) - sizeof(sys_modem_as_id_e_type), 
                          &mmcc_reest_req.header);
      }
#endif   
      CNM_send_message((IMH_T *)&mmcc_reest_req,GS_QUEUE_MM);
   
      mm_link_reestablishing = TRUE;
   
      /*
      * tell MN that conection is lost so that it can disconnect the vocoder
      * and inform CM that the connection has been lost
      */
   
      mncc_err_ind.message_header.message_set = MS_MN_CC;
      mncc_err_ind.message_header.message_id  = MNCC_ERR_IND;
   
      /* If Multi-SIM fill as_id*/
#ifdef FEATURE_DUAL_SIM   
      if(MNCNM_IS_SINGLE_STACK) /* DSDS */
      {
        mncc_err_ind.as_id =  mmcnm_rel_ind->as_id;
        PUT_IMH_LEN(sizeof(MNCC_ERR_IND_T) - sizeof(IMH_T), 
                    &mncc_err_ind.message_header);
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        mncc_err_ind.as_id =  (sys_modem_as_id_e_type)mn_dsda_as_id;
        PUT_IMH_LEN(sizeof(MNCC_ERR_IND_T) - sizeof(IMH_T), 
                    &mncc_err_ind.message_header);
   
      }
#endif
#else
      {
         PUT_IMH_LEN(0, &mncc_err_ind.message_header);
      }
#endif
      CNM_send_message((IMH_T *)&mncc_err_ind,GS_QUEUE_MN_CM );
	  for (transaction_id= 0; transaction_id < MAXNO_CC_TRANSACTIONS; transaction_id++)
      {
        if ((cc_transaction_information_p[transaction_id] != NULL) &&
            (cc_transaction_information_p[transaction_id]->cc_state == CC_STATE_U10 ) &&
			(mn_call_information_p[transaction_id] != NULL))
           {
			  mn_call_information_p[transaction_id]->cause_before_gsm_reestablishment = mmcnm_rel_ind->rel_cause;
           
		   }
      }
   
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Invalid input :: Can't process CNM_process_mm_establishment_lost");
   }

}
/**************** End of   CNM_process_mm_establishment_lost ***************************/


/**************** Begin of   CNM_process_mm_reestablishment ***********************
 *
 *  Function name: CNM_process_mm_reestablishment()
 *  ----------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function responds to confirmation from MM of the reestablishment
 *  of an MM connection
 *
 *  Parameters:
 *  -----------
 *  
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mm_reestablishment(void)
{
   MNCC_REEST_IND_T      mncc_reest_ind;
   mmcnm_data_req_T      mmcnm_data_req;
   int                   transaction_id;
   boolean               reestablish_conf = FALSE;

   mm_link_reestablishing = FALSE;



   /* While in the process of reestablishing the MM connection it is 
    * possible that CNM received another primitive that caused a state 
    * change.  First,  send any requests in the CNM data queue and then
    * Check to see if we are still in the right states before 
    * continuing with the reestablishment process
    */
   while (CNM_pop_cnm_data_queue(&mmcnm_data_req) != FALSE)
   {
      CNM_send_message( (IMH_T *)&mmcnm_data_req, GS_QUEUE_MM );
   }

   for (transaction_id= 0; 
        transaction_id < MAXNO_CC_TRANSACTIONS; 
        transaction_id++)
   {
      if ( (cc_transaction_information_p[transaction_id] != NULL ) &&
          ((cc_transaction_information_p[transaction_id]->cc_state == CC_STATE_U10 ) ||
          (cc_transaction_information_p[transaction_id]->cc_state == CC_STATE_U26)))
      {
         reestablish_conf = TRUE;
      }
   }

   if (reestablish_conf != TRUE)
   {
      MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MMCC_REEST_CNF received in wrong state");
   }
   else
   {
      mncc_reest_ind.message_header.message_set = MS_MN_CC;
      mncc_reest_ind.message_header.message_id  = MNCC_REEST_IND;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
      {
        mncc_reest_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
      PUT_IMH_LEN( sizeof(MNCC_REEST_IND_T) - sizeof(IMH_T), &mncc_reest_ind.message_header );

      CNM_send_message((IMH_T *)&mncc_reest_ind, GS_QUEUE_MN_CM );

   }

   /*
    * now check if there are any service requests to consider. If
    * so, then send first one th MM for processing, and record the
    * pending connection
    */

   /*
    * first check that we are not in the middle of a connection
    * establishment anyway. If we are, then don't do anything here,
    * the requests will be popped off when the EST_CNF is received
    */

   if ( mm_connection_pending.CNM_service_type == NO_CNM_SERVICE )
   {
      CNM_send_est_request_from_service_queue();
   }

}
/**************** End of   CNM_process_mm_reestablishment ***************************/
