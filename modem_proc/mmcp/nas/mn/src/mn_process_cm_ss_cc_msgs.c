/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_process_cm_ss_cc_msgs.c_v   1.12   13 May 2002 14:06:24   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_process_cm_ss_cc_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/01   AB      Moved the CM SS call related here.

10/12/01   AB      Fixed Lint errors.   

12/01/01   AB      Updated to new MN_start_ss_timer() interface changes.

01/12/02   AB      Added capability for 2_SEND command, MN_process_2_send()
                     to toggle and single held call.  Update the same capability
                     in 2_x_send().

01/23/02   AB      Added mn_exp.h includes becuase SS components ERROR tag had been
                      moved to mn_exp.h so that these tag can be used by CM/UI.

01/25/02   CD      Updated copyright
                   Replaced calls to ERR() by MSG_ERR()
                   Replaced IEI by boolean flag
                   Added customer.h and removed unnecessary mn_exp.h

02/11/02   AB      Allow 1_x_send to disconnect held call as well.

02/12/02   AB      Added checking for single retrieve call in MN_process_2_send().
                      in both MultiCall Index table instead just default entry.

02/28/02   AB      If 3_send is not possible, return error to CM to prevent 
                      lock up.

03/12/02   AB      Added MN_process_cm_4_send() and MN_process_cm_4_x_send() for
                      handling explicit call transfer and call deflection 
                      feature, respectively.

04/02/02   AB      Changed the Cause value for Disconnect request due to 
                      call deflection to NORMAL_CLEARING from USER_BUSY.

05/06/02   AB      Added more user error handling for CM_4_SEND and CM_4_X_SEND
                      command. 

08/12/02   AB      In MN_handle_cm_4_x_send, set the CAUSE IE present flag to 
                      TRUE per design.

08/14/02   AB      In MN_handle_cm_4_x_send, change the reference of 
                      callDelfection, to callDeflection.

08/20/02   AB      In MN_handle_cm_2_x_send, updated the splitMPTY operation
                      that is destined to be HELD with CCE_HOLD_REQUEST instead
                      SPLIT_REQUEST.

09/04/02   AB      In MN_handel_cm_2_x_send, update the splitMPTY operation
                      to meet GSM 11.10 conformance.

10/12/02   AB      In MN_handle_cm_3_send(), select connection ID that not
                      already in MPTY to perform an buildMPTY request. 

10/28/02   AB      In MN_handle_cm_3_send(), change AUX state of all non-MPTY
                      calls instead of just one.

11/02/04   AB      Added SS Call related User-to-User Signaling feature.

05/02/05   HS      Added support for FEATURE_CCBS

06/14/05   HS      Variable initialization

04/11/06   NR      Activating the codec in MN_process_cm_2_x_send(), when there are
                   no active call, but has a waiting call

12/22/06   RD      Fixed lint errors

02/13/06   NR      Answering the incoming call for abnormal cases in case of 2_SEND request

03/21/07   NR      For tracking SS pending requests

05/22/07   NR      Sending HOLD request before RETRIEVING a HELD call

08/28/07   NR      Not respond waiting call in case of HOLD request sent

05/25/11   AM      Putting the error range check for Multicall_Status_index

07/06/11    PM     Guarding against NULL pointer access
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include <string.h>
#include "cause.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "ds_cap_v.h"
#include <stringl/stringl.h>
#include "nasutils.h"



/*
 *   Function name : MN_process_cm_0_send
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function provides the routing of messages
 * to the MN layer from the CNM layer and routes them to
 * the appropriate CM message handling functions in MN-C
 *
 * user releases all held calls or
 * sets UDUB for a waiting call
 * sends DISCONNECT *
 *
 *
 *  Parameters: none
 *  -----------
 *  
 *
 *  Returns: void
 *
 *
 */
VOID_FUNC   MN_process_cm_0_send(cm_n_send_req_T *message)
{
   ie_cm_cc_cause_T   cm_cc_cause={0};
   ie_cm_ss_error_T   cm_ss_error;
   int              index,Multicall_Status_index;
   connection_id_T   connection_id;
   boolean success = FALSE;
   memset(&(cm_ss_error),'\0',sizeof(ie_cm_ss_error_T));


   MN_MULTICALL_LOG(MN_read_multicall_status()); /* which refreshes Multicall_Status */

   if (Multicall_Status[2].call_activity == CALL_WAITING)
   /* reject the call waiting with UDUB first */
   {
      cm_cc_cause.location = STD_LOCATION;
      cm_cc_cause.coding_standard = STD_CODESTANDARD;
      cm_cc_cause.recommendation_present = FALSE;
      cm_cc_cause.cause_value = USER_BUSY;
      cm_cc_cause.diagnostic_length = 0;

      connection_id = Multicall_Status[2].connection_id[0];

      MN_put_MNCC_DISC_REQ(connection_id,
                           &(cm_cc_cause),
                           NULL);
	  success = TRUE;

      mn_call_information_p[connection_id]->mo_call_clear_pending = TRUE;
   }
   else
   { /* If no call waiting, reject any held calls */

      for (Multicall_Status_index = 0; Multicall_Status_index < 2; Multicall_Status_index++)
      {
         if (Multicall_Status[Multicall_Status_index ].call_activity == CALL_HELD)
         {
            cm_cc_cause.location = STD_LOCATION;
            cm_cc_cause.coding_standard = STD_CODESTANDARD;
            cm_cc_cause.recommendation_present = FALSE;
            cm_cc_cause.cause_value = NORMAL_CALL_CLEARING;
            cm_cc_cause.diagnostic_length = 0;

            for (index = 0; index <  Multicall_Status[Multicall_Status_index].id_index; index++)
            {
               connection_id = Multicall_Status[Multicall_Status_index].
                   connection_id[index];

               if((connection_id < MAXNO_CC_TRANSACTIONS) && mn_call_information_p[connection_id] != NULL)
               {
                  MN_stop_cc_timer(connection_id);
   
                  MN_put_MNCC_DISC_REQ(connection_id, &(cm_cc_cause), NULL);
                  success = TRUE;
                  mn_call_information_p[connection_id]->mo_call_clear_pending = TRUE;
               }
            } /* index */
         } /* CALL_HELD */
      } /* Multicall_Status_index */
   } /* CALL WAITING */
   if (success == FALSE)
    {
        cm_ss_error.present = TRUE;
        cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
        cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

        MN_put_cm_manage_calls_conf(FALSE,
#if defined(FEATURE_DUAL_SIM ) 
                                    message->as_id,
#endif
			                       cm_ss_error);
    }
}


/*
 *   Function name : MN_process_cm_1_send
 *   ---------------
 *
 *   Description:
 *   ------------
 *   This function processes the cm_1_send message that
 *   releases all active calls (if any existed).
 *
 *   The MN_handle_MNCC_REL_CNF would accepts the held or 
 *   waiting call 
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


VOID_FUNC MN_process_cm_1_send(cm_n_send_req_T *message)
{
   ie_cm_cc_cause_T    cm_cc_cause={0};
   int                 Multicall_Status_index;
   int                 index, active_calls = FALSE;
   connection_id_T     connection_id;


   MN_MULTICALL_LOG(MN_read_multicall_status()); /* which refreshes  Multicall_Status  */

   /* set up the cause data */
   cm_cc_cause.location = STD_LOCATION;
   cm_cc_cause.coding_standard = STD_CODESTANDARD;
   cm_cc_cause.recommendation_present = FALSE;
   cm_cc_cause.cause_value = NORMAL_CALL_CLEARING ;
   cm_cc_cause.diagnostic_length = 0;

   for (Multicall_Status_index = 0; Multicall_Status_index < 2; Multicall_Status_index++)
   {
      if (Multicall_Status[Multicall_Status_index].call_activity == CALL_ACTIVE)

      /* close down these calls */
      {
         for (index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
         {
            connection_id = Multicall_Status[Multicall_Status_index].connection_id[index];

            if (connection_id < MAXNO_CC_TRANSACTIONS)
            {
               active_calls = TRUE;

               MN_stop_cc_timer(connection_id);

               MN_put_MNCC_DISC_REQ(connection_id,&(cm_cc_cause), NULL);

               mn_call_information_p[connection_id]->mo_call_clear_pending = TRUE;
               mn_call_information_p[connection_id]->mo_release_indicator = CM_1_SEND_REL;
               mn_call_information_p[connection_id]->ss_request_pending= CM_1_SEND_PENDING; 
            }
         } /* index */
      }/* CALL_ACTIVE */
   } /* Multicall_Status_index*/

   if(active_calls != TRUE)
   {
       MN_process_cm_2_send(message);
   }
}


/*
 *   Function name : MN_process_cm_2_send
 *   ---------------
 *
 *   Description:
 *   ------------
 *   This function processes  the cm_2_send message
 *   that puts active calls on hold so that held or
 *   waiting can be retrieves.
 *
 *   After the user releases all active calls, the  
 *   MN_handle_MNCC_HOLD_CNF or MNCC_FACILITY_IND
 *   accepts the held or waiting call 
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC   MN_process_cm_2_send(cm_n_send_req_T * message)
{

   enum {NO_RESOURCE, TOGGLE_HOLD, TOGGLE_RETRIEVE, ANSWER_WAITING} hold_success;
   byte       Multicall_Status_index;
   ie_cm_ss_error_T   cm_ss_error;
   cm_connected_subaddress_T   null_cm_connected_subaddress;
   mn_call_information_T *data_ptr;   
   connection_id_T           connection_id = 0;
   byte      state;
   int       index;
   boolean   success = TRUE;

   memset(&(cm_ss_error),'\0',sizeof(ie_cm_ss_error_T));

   for(index = 0; index < MULTICALL_STATUS_SIZE; index++)
   {
      connection_id =Multicall_Status[index].connection_id[0];
      if (cc_transaction_information_p[connection_id] != NULL && mn_call_information_p[connection_id] != NULL)  /* reserved connection ID = emergency call */ 
      {
         state = cc_transaction_information_p[connection_id]->cc_state;
		 
         /* Reject the 2_send request, if the lowest connection id call is in the process of disconnection and NAS receives HOLD/RETRIEVE request from CM at the same time*/
         /* Generally UE sends FACILITY message with lowest connection id for HOLD/MPTY request. If the lowest connection id is getting released, UE should not send the FACILITY to network in this case.*/
		 if ((Multicall_Status[index].call_activity == CALL_HELD || Multicall_Status[index].call_activity == CALL_ACTIVE) && (state != CC_STATE_U10))
		 {
			 success = FALSE;
			 break;
		 }
	  }
   }
   if (success == TRUE)
   {
      MN_MULTICALL_LOG(MN_read_multicall_status());

      /* for this to work, you need 1 active call and 1 held or waiting call */

      if ((Multicall_Status[0].call_activity == CALL_ACTIVE)&&
         (((Multicall_Status[1].call_activity == CALL_HELD)&& 
         (Multicall_Status[2].call_activity != CALL_WAITING))||
         ((Multicall_Status[1].call_activity != CALL_HELD)&& 
         (Multicall_Status[2].call_activity == CALL_WAITING))||
         ((Multicall_Status[1].call_activity == CALL_INACTIVE)&& 
         (Multicall_Status[2].call_activity == CALL_INACTIVE))))
      { /* Put call [0] on hold */

          Multicall_Status_index = 0;

          hold_success = TOGGLE_HOLD;
      }
      else if((Multicall_Status[1].call_activity == CALL_ACTIVE)&&
            (((Multicall_Status[0].call_activity == CALL_HELD)&& 
            (Multicall_Status[2].call_activity != CALL_WAITING))||
            ((Multicall_Status[0].call_activity != CALL_HELD)&& 
            (Multicall_Status[2].call_activity == CALL_WAITING))||
            ((Multicall_Status[0].call_activity == CALL_INACTIVE)&& 
            (Multicall_Status[2].call_activity == CALL_INACTIVE))))
      {   /* Put call [1] on hold */

          Multicall_Status_index = 1;
          hold_success = TOGGLE_HOLD;
      }
      else if ((Multicall_Status[0].call_activity == CALL_HELD) &&
               (Multicall_Status[1].call_activity == CALL_INACTIVE) &&
               (Multicall_Status[2].call_activity == CALL_INACTIVE))
      {  /* toggle single retrieve call */
          Multicall_Status_index = 0;
          hold_success = TOGGLE_RETRIEVE;
      }
      else if ((Multicall_Status[1].call_activity == CALL_HELD) &&
               (Multicall_Status[0].call_activity == CALL_INACTIVE) &&
               (Multicall_Status[2].call_activity == CALL_INACTIVE))
      { 
          Multicall_Status_index = 1;
          hold_success = TOGGLE_RETRIEVE;
      }
      else if ((((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
                 (Multicall_Status[1].call_activity == CALL_HELD) &&
                 (cc_transaction_information_p[Multicall_Status[1].connection_id[0]] != NULL) &&
                 (cc_transaction_information_p[Multicall_Status[1].connection_id[0]]->hold_auxiliary_state != HOLD_REQUEST)) ||
                 ((Multicall_Status[0].call_activity == CALL_HELD) &&
                 (Multicall_Status[1].call_activity == CALL_INACTIVE) &&
                 (cc_transaction_information_p[Multicall_Status[0].connection_id[0]] != NULL) &&
                 (cc_transaction_information_p[Multicall_Status[0].connection_id[0]]->hold_auxiliary_state != HOLD_REQUEST))) &&
                 (Multicall_Status[2].call_activity == CALL_WAITING))
      {
         Multicall_Status_index = 2;
         hold_success = ANSWER_WAITING;
      }
      else if (((Multicall_Status[0].call_activity == CALL_INACTIVE) &&
                (Multicall_Status[1].call_activity == CALL_INACTIVE))&&
                (Multicall_Status[2].call_activity == CALL_WAITING))
      {
         Multicall_Status_index = 2;
         hold_success = ANSWER_WAITING;
      }
      else 
      {
          Multicall_Status_index = 0;
          hold_success = NO_RESOURCE;
      }

      if (hold_success == TOGGLE_HOLD)
      {
    
          /* only for abnormal case handling where DISCONNECT may be
             received in response to HOLD_REQ */  
          if(( (Multicall_Status[0].call_activity == CALL_ACTIVE)&&
               ((Multicall_Status[1].call_activity != CALL_HELD)&& 
               (Multicall_Status[2].call_activity == CALL_WAITING))) ||
               ((Multicall_Status[1].call_activity == CALL_ACTIVE)&&
               ((Multicall_Status[0].call_activity != CALL_HELD)&& 
               (Multicall_Status[2].call_activity == CALL_WAITING))))
          {
             mn_call_information_p[Multicall_Status[Multicall_Status_index].connection_id[0]]->hold_request = CM_2_SEND_HOLD;
             MSG_HIGH_DS_0(MN_SUB,"=MNCNM= hold_request set due to 2_SEND request with active and waiting call present");          
          }

          /* for marking the pending requests in order to respond to ATCOP commands later */
          mn_call_information_p[Multicall_Status[Multicall_Status_index].connection_id[0]]->
              ss_request_pending= CM_2_SEND_PENDING;
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= ss_request CM_2_SEND_PENDING pending");   

          (void)MN_put_call_on_hold(Multicall_Status[Multicall_Status_index].connection_id[0]);

          Multicall_Status_index = (byte )((Multicall_Status_index + 1) % 2); /* the other calls */

          if(Multicall_Status[Multicall_Status_index].call_activity == CALL_HELD) 
          {
             /* for marking the pending requests in order to respond to ATCOP commands later */
             mn_call_information_p[Multicall_Status[Multicall_Status_index].connection_id[0]]->
             ss_request_pending= CM_2_SEND_PENDING;  
             MSG_HIGH_DS_0(MN_SUB,"=MNCNM= ss request CM_2_SEND_PENDING pending Alternate Procedure");

             /* clearing the pending request for hold request so that 2 cm_manage_call_conf() 
             requests are not generate in case of RR faliure */
             mn_call_information_p[Multicall_Status[((Multicall_Status_index + 1) % 2)].connection_id[0]]->
              ss_request_pending= CM_NONE_PENDING; 

             MN_retrieve_call(Multicall_Status_index); /* the held call */
             mn_alternate_proc_status.is_alternate_procedure = TRUE;
             mn_alternate_proc_status.status = MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED;
          }
      }
      else if (hold_success == TOGGLE_RETRIEVE)
      {
          /* for marking the pending requests in order to respond to ATCOP commands later */
          mn_call_information_p[Multicall_Status[Multicall_Status_index].connection_id[0]]->
              ss_request_pending= CM_2_SEND_PENDING;
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= ss request CM_2_SEND_PENDING pending");   
 
          MN_retrieve_call(Multicall_Status_index);
      }
      else if (hold_success == ANSWER_WAITING) /*else if clause added to fix cr 114967*/
      {
         /* initialize null_cm_connected_subaddress */
         memset(&null_cm_connected_subaddress, 0, sizeof(cm_connected_subaddress_T));

         mn_call_information_p[Multicall_Status[Multicall_Status_index].connection_id[0]]->
                                       mt_call_accept_cause = CM_2_SEND_REQ;

         MN_put_MNCC_SETUP_RES(Multicall_Status[2].connection_id[0],
                        &(null_cm_connected_subaddress));

         data_ptr = mn_call_information_p[Multicall_Status[MN_CALL_WAITING_SESSION].connection_id[0]];

         /*  IF 1st BC IS FOR SPEECH, CONNECT SPEECH NOW  */
         if(
#ifdef FEATURE_ALS
            (data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
            (data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
           )
         {
           MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability);
         }

      }
      else
      {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Can't HOLD nor RETRIEVE for given calls");

          cm_ss_error.present = TRUE;
          cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
          cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

          MN_put_cm_manage_calls_conf(FALSE,
#if defined(FEATURE_DUAL_SIM ) 
                                      message->as_id,
#endif
                                     cm_ss_error);
	  }
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Can't HOLD nor RETRIEVE for given calls");
      cm_ss_error.present = TRUE;
      cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
      cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

      MN_put_cm_manage_calls_conf(FALSE,
#if defined(FEATURE_DUAL_SIM )
                                    message->as_id,
#endif
			                       cm_ss_error);
	}
   

}



/*
 *   Function name : MN_process_cm_2_x_send
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function processes the cm_2_x_send message from the 
 *  cm to MN. 2 - x - send signifies a request to retrieve call x
 *  and put the other call on hold or, if call x is part of a multi-
 *  party call, to split the call and put the other mpty call parties
 *  on hold
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


VOID_FUNC   MN_process_cm_2_x_send(cm_n_x_send_req_T    *pointer)
{

   boolean             success=FALSE;
   byte                Multicall_Status_index,other_call,invoke_id,
                       components[MAX_COMPONENTS_LENGTH],
                       ss_version[MAX_SS_VERSION_IND_LENGTH];
   ie_cm_ss_error_T               cm_ss_error;
   cm_connected_subaddress_T   null_cm_connected_subaddress;
   mn_call_information_T *data_ptr;

#if defined(FEATURE_DUAL_SIM ) 
   if(( MNCNM_IS_SINGLE_STACK == FALSE) || (pointer->as_id == mn_as_id))
#endif     
   {
     /* initialize null_cm_connected_subaddress */
     memset(&null_cm_connected_subaddress, 0, sizeof(cm_connected_subaddress_T));
     memset(&(cm_ss_error),'\0',sizeof(ie_cm_ss_error_T));
   
     /* Locate call session, this also refresh the multicall status (call session table) */
     Multicall_Status_index = MN_whereami(pointer->connection_id);
     if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
     {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
     }
     else if (Multicall_Status[Multicall_Status_index].call_is_mpty == FALSE)
     {
       if (Multicall_Status[Multicall_Status_index].call_activity == CALL_HELD)
       {
          if((cc_transaction_information_p[Multicall_Status[Multicall_Status_index].connection_id[0]] != NULL) && 
             (mn_call_information_p[Multicall_Status[Multicall_Status_index].connection_id[0]] != NULL)) 
          {
             if(cc_transaction_information_p[Multicall_Status[Multicall_Status_index].connection_id[0]]->cc_state == CC_STATE_U10)
			 { 
                /* Put the other call on hold  */
                Multicall_Status_index = (byte)((Multicall_Status_index + 1) %2);

                if (Multicall_Status[Multicall_Status_index].call_activity == CALL_ACTIVE)
                {
                   success = MN_put_call_on_hold(
                       Multicall_Status[Multicall_Status_index].connection_id[0]);
			       /* Mark that this is an alternate procedure so that while processing various network responses
                                 * NAS take appropriate actions. */
                   mn_call_information_p[Multicall_Status[Multicall_Status_index].connection_id[0]]->
                                ss_request_pending= CM_2_X_SEND_PENDING;
                   mn_alternate_proc_status.is_alternate_procedure = TRUE;
                   mn_alternate_proc_status.status = MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED;

                }

                /* Retrieve a held call */
                Multicall_Status_index = (byte)((Multicall_Status_index + 1) %2);
                MN_retrieve_call(Multicall_Status_index);
             }
             else
             { 
                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Can't HOLD nor RETRIEVE for given calls when any of the call is disconnected");
                cm_ss_error.present = TRUE;
                cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

                MN_put_cm_manage_calls_conf(FALSE,
#if defined(FEATURE_DUAL_SIM )
                                    pointer->as_id,
#endif
                                    cm_ss_error);

              }
         }

       }
       else if (Multicall_Status[Multicall_Status_index].call_activity == CALL_WAITING)
       {
        
         mn_call_information_p[Multicall_Status[Multicall_Status_index].connection_id[0]]->hold_request = CM_2_X_SEND_HOLD;
         /* Retrieve a waiting call */
         if ((Multicall_Status[0].call_activity == CALL_INACTIVE)||
             (Multicall_Status[1].call_activity == CALL_INACTIVE))
         /* 
          * Retrieving a waiting call is possible so put an active call on 
          * hold 
          */
         {
            if (Multicall_Status[0].call_activity == CALL_ACTIVE)
            {
               /* put this call on hold */
               success = 
                   MN_put_call_on_hold(Multicall_Status[0].connection_id[0]);
            }
            else
            {
               if (Multicall_Status[1].call_activity == CALL_ACTIVE)
               {
                  /* put this call on hold */
                  success = 
                      MN_put_call_on_hold(Multicall_Status[1].connection_id[0]);
               }
               else    
               {
                  /* there are no active calls, so continue the MT all setup */
                  mn_call_information_p[Multicall_Status[2].connection_id[0]]->
                                     mt_call_accept_cause = CM_2_X_SEND_REQ;

                  MN_put_MNCC_SETUP_RES(Multicall_Status[2].connection_id[0],
                      &(null_cm_connected_subaddress));

                  data_ptr =  mn_call_information_p[Multicall_Status[2].connection_id[0]];

                  /*  IF 1st BC IS FOR SPEECH, CONNECT SPEECH NOW  */
                  if(
#ifdef FEATURE_ALS
                     (data_ptr->bearer_capability_1.information_transfer_capability == ITC_AUX_SPEECH) ||
#endif
                     (data_ptr->bearer_capability_1.information_transfer_capability == ITC_SPEECH)
                    )
                  {
                      MN_connect_TCH(data_ptr->bearer_capability_1.information_transfer_capability);
                  }
               }
            }
         }
         else
         {

            cm_ss_error.present = TRUE;
            cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
            cm_ss_error.error_code = MN_SPLIT_MPTY_REJ;

            MN_put_cm_manage_calls_conf(
                                        FALSE,
#if defined(FEATURE_DUAL_SIM)
                                        pointer->as_id,
#endif 
                                        cm_ss_error);
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: MPTY call in wrong state to split"); 
         }
       }
       else /*  If Call is active, send manage call confirm*/
       {
         cm_ss_error.present = TRUE;
         cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
         cm_ss_error.error_code = MN_CALL_RETRIEVE_REJ;

         MN_put_cm_manage_calls_conf(
                                      FALSE,
#if defined(FEATURE_DUAL_SIM )
                                      pointer->as_id,
#endif 
                                      cm_ss_error);

         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Recd CM_2_X_SEND, Can't retrieve an active call"); 

       }
   }
   else
   /* Set up the split for a mpty call.
    *
    * For a split, the mty call must be active and there must not be 
    * another active call 
    */
   {
      other_call = (byte)((Multicall_Status_index+1) % 2);

      if ((Multicall_Status[other_call].call_activity == CALL_INACTIVE) &&
          (Multicall_Status[Multicall_Status_index].call_activity == CALL_ACTIVE))
      {
         /* Update the split call state */
         success = MN_manage_CC_auxiliary_state(pointer->connection_id, CCE_SPLIT_MPTY_REQ);
      }

      /* 
       * if the above changing of the state machine has been successful, 
       * we can continue ... 
       */

      if (success == TRUE)
      {
         /* request a new invoke_id */
         invoke_id = MN_request_invoke_id(splitMPTY);

         /* for marking the pending requests in order to respond to ATCOP commands later */
         mn_call_information_p[pointer->connection_id]->ss_request_pending = CM_2_X_SEND_PENDING;
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= ss request CM_2_X_SEND_PENDING pending");  

         /* set up the facility component fields */
         components[0] = 0xa1;   /* component type tag = Invoke */
         components[1] = 0x06;   /*  = component length */
         components[2] = 0x02;   /*  = Invoke ID tag  */
         components[3] = 0x01;   /*  = Invoke ID length */
         components[4] = invoke_id;
         components[5] = 0x02;   /*  = Operational_code tag */
         components[6] = 0x01;   /*  = Operational code length */
         components[7] = splitMPTY;     /*  = operational code */

         /* set up a ss_version */
         ss_version[0] = 0;

         /* send the split multiparty ss facility message */
         MN_put_MNCC_FACILITY_REQ(pointer->connection_id,
             components,
             8,               /*  component_length  */
             ss_version,
             1);               /* ss_version_length */

         MN_start_ss_timer(pointer->connection_id,invoke_id, 10000);  /* 10 sec */
      }
      else
      {
         cm_ss_error.present = TRUE;
         cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
         cm_ss_error.error_code = MN_SPLIT_MPTY_REJ;

         MN_put_cm_manage_calls_conf(
                            FALSE,
#if defined(FEATURE_DUAL_SIM )
                            pointer->as_id,
#endif 
                            cm_ss_error);

         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: MPTY call in wrong state to split"); 
       }
     }
   }
#if defined(FEATURE_DUAL_SIM )
   else if(MNCNM_IS_SINGLE_STACK)
   {
     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", pointer->as_id,mn_as_id); 
     cm_ss_error.present = TRUE;
     cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
     cm_ss_error.error_code = AS_REJ_ABORT_RADIO_UNAVAILABLE;
     MN_put_cm_manage_calls_conf(FALSE, pointer->as_id ,cm_ss_error);
   }
#endif
}


/*
 *   Function name : MN_process_cm_3_send
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function processes the cm_3_send message from the 
 *  cm to MN.
 *  3 - send signifies a request to build a multi-party call
 *  by combining the current active call with the current 
 *  held call
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


VOID_FUNC   MN_process_cm_3_send(cm_n_send_req_T *message)
{

   /* Locals */

   boolean      success = TRUE;
   int          Multicall_index;
   byte         invoke_id;
   byte         components[8];
   byte         ss_version[MAX_SS_VERSION_IND_LENGTH];
   ie_cm_ss_error_T   cm_ss_error;

   memset(&(cm_ss_error),'\0',sizeof(ie_cm_ss_error_T));
   /* Refresh Multicall_Status */

   MN_MULTICALL_LOG(MN_read_multicall_status());

   if (((Multicall_Status[0].call_activity == CALL_ACTIVE) &&
       (Multicall_Status[1].call_activity == CALL_HELD)) ||
       ((Multicall_Status[1].call_activity == CALL_ACTIVE) &&
       (Multicall_Status[0].call_activity == CALL_HELD)))
   {

      if((Multicall_Status[0].call_is_mpty) || (Multicall_Status[1].call_is_mpty))
      {
          Multicall_index =  ((Multicall_Status[0].call_is_mpty) ? 1 : 0);  /* get non-MPTY session */

          success = MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_index].connection_id[0], 
                                  CCE_BUILD_MPTY_REQ);
      }
      else
      {
          /* both call session are non-MPTY */
          Multicall_index = 0;
          success = MN_manage_CC_auxiliary_state(Multicall_Status[0].connection_id[0], CCE_BUILD_MPTY_REQ);
          success = MN_manage_CC_auxiliary_state(Multicall_Status[1].connection_id[0], CCE_BUILD_MPTY_REQ);
      }

      if (success == FALSE) 
      {
          /* AUX state recovery */
          success = MN_manage_CC_auxiliary_state(Multicall_Status[0].connection_id[0], CCE_BUILD_MPTY_REJ);
          success = MN_manage_CC_auxiliary_state(Multicall_Status[1].connection_id[0], CCE_BUILD_MPTY_REJ);

          MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid call state for buildMPTY operation = %d ", 
                          Multicall_Status[0].connection_id[0],
                          Multicall_Status[1].connection_id[0]);
          cm_ss_error.present = TRUE;
          cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
          cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

          MN_put_cm_manage_calls_conf(FALSE,
#if defined(FEATURE_DUAL_SIM )
                                      message->as_id,
#endif
                                      cm_ss_error); 
      }
      else
      {

         /* for marking the pending requests in order to respond to ATCOP commands later */
         mn_call_information_p[Multicall_Status[Multicall_index].connection_id[0]]->
               ss_request_pending = CM_3_SEND_PENDING;
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= ss request CM_3_SEND_PENDING pending");  
 
         /* 
          * successful change of call status - send a build multiparty 
          * request to the network 
          */
         invoke_id = MN_request_invoke_id(buildMPTY);

         /* set up the facility message component = invoke */

         components[0] = 0xa1; /* = component type tag = Invoke */
         components[1] = 0x06; /* component length */
         components[2] = 0x02; /* Invoke ID tag  */
         components[3] = 0x01; /*  Invoke ID length */
         components[4] = invoke_id;
         components[5] = 0x02; /*  Operational_code tag */
         components[6] = 0x01; /*  Operational code length */
         components[7] = buildMPTY;

         /* set up the ss version data */

         ss_version[0] = 0;

         MN_put_MNCC_FACILITY_REQ(
               Multicall_Status[Multicall_index].connection_id[0],  /*Anyone will do */
               components,
               8,            /* component_length */
               ss_version,
               1);

         /* Start the timer */

         MN_start_ss_timer(Multicall_Status[Multicall_index].connection_id[0], invoke_id, 10000); /* 10 sec */
      }
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Not enough calls to build a mpty call");

      cm_ss_error.present = TRUE;
      cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
      cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

      MN_put_cm_manage_calls_conf(FALSE,
#if defined(FEATURE_DUAL_SIM )
                                  message->as_id,
#endif
                                  cm_ss_error);
   }
}



/*
 *   Function name : MN_process_cm_1_x_send
 *   ---------------
 *
 *   Description:
 *   ------------
 *  Handle mobile originated call cleardown *
 *
 * CM requests call cleardown for a specific call.
 *
 *The X refers to the CM concept of 'call number'.
 *
 * CM must map this to the required connection_id.
 *
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


VOID_FUNC   MN_process_cm_1_x_send(cm_n_x_send_req_T    *pointer)
{
   int                    Multicall_Status_index;
   ie_cm_cc_cause_T       cm_cc_cause = {0};
   ie_cm_ss_error_T       cm_ss_error;
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (pointer->as_id == mn_as_id))
#endif     
   {
     memset(&(cm_ss_error),'\0',sizeof(ie_cm_ss_error_T));
     /* Refresh Multicall_Status */

     MN_MULTICALL_LOG(MN_read_multicall_status());

     /* Locate the stored data */

     Multicall_Status_index =  MN_whereami(pointer->connection_id);

     if(Multicall_Status_index >= MULTICALL_STATUS_SIZE) 
     {
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Invalid connection id = %d",pointer->connection_id);

       cm_ss_error.present = TRUE;
       cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
       cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;
              MN_put_cm_manage_calls_conf(
                FALSE, 
#if defined(FEATURE_DUAL_SIM )
                pointer->as_id, 
 #endif  
                cm_ss_error);
     }
     else  /*ie there is an active call with this id */
     {
       if(((Multicall_Status[Multicall_Status_index].call_activity == CALL_ACTIVE) ||
           (Multicall_Status[Multicall_Status_index].call_activity == CALL_HELD))
           || (pointer->cause_value == 68))
        /* close down the call */
       {
         /* set up the cause data */
         cm_cc_cause.location = STD_LOCATION;
         cm_cc_cause.coding_standard = STD_CODESTANDARD;
         cm_cc_cause.recommendation_present = FALSE;
         cm_cc_cause.cause_value = pointer->cause_value;
         cm_cc_cause.diagnostic_length = 0;

         /* Stop MN Timer if Call is held or being Retrieved*/

         MN_stop_cc_timer(pointer->connection_id);

         /* send a disconnect message */
         MN_put_MNCC_DISC_REQ(pointer->connection_id, &(cm_cc_cause), NULL);

         mn_call_information_p[pointer->connection_id]->mo_call_clear_pending = TRUE;
         mn_call_information_p[pointer->connection_id]->mo_release_indicator = CM_1_X_SEND_REL;
         mn_call_information_p[pointer->connection_id]->ss_request_pending= CM_1_X_SEND_PENDING; 
      }
      else
      {

          MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Connection id = %d is not active",pointer->connection_id);

          cm_ss_error.present = TRUE;
          cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
          cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

          MN_put_cm_manage_calls_conf(
               FALSE,
#if defined(FEATURE_DUAL_SIM )
               pointer->as_id, 
#endif  
               cm_ss_error);
      }
    } 
}
#if defined(FEATURE_DUAL_SIM )
   else if(MNCNM_IS_SINGLE_STACK)
   {
     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", pointer->as_id,mn_as_id); 
     cm_ss_error.present = TRUE;
     cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
     cm_ss_error.error_code = AS_REJ_ABORT_RADIO_UNAVAILABLE;
     MN_put_cm_manage_calls_conf(FALSE, pointer->as_id ,cm_ss_error);

   }
#endif
}

/*
 *  Function name : MN_process_cm_4_x_send
 *  ---------------
 *
 *  Description:
 *  ------------
 *   Handle mobile originated call deflection which
 *   Deflect the incoming call waiting to a specified
 *   directory number.
 *
 *  Parameters:
 *  -----------
 *   IN message
 *
 *  Returns:
 *
 */

VOID_FUNC   MN_process_cm_4_x_send(cm_4_x_send_req_T    *pointer)
{
   int  Multicall_Status_index; /* call session status */ 
   ie_cm_cc_invoke_T  cm_cc_invoke;
   ie_cm_ss_error_T   cm_ss_error;
   ie_cm_cc_cause_T       cm_cc_cause = {0};
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK) || (pointer->as_id == mn_as_id))
#endif     
   {
     memset(&(cm_ss_error),'\0',sizeof(ie_cm_ss_error_T));
  
     /* Refresh Multicall_Status */
     MN_MULTICALL_LOG(MN_read_multicall_status());

     /* Which call session of given connection id */
     Multicall_Status_index =  MN_whereami(pointer->connection_id);

     if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
     {
        /* CD response error condition */              
        cm_ss_error.present = TRUE;
        cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
        cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

        MN_put_cm_manage_calls_conf(
                FALSE,
#if defined(FEATURE_DUAL_SIM )
                pointer->as_id,
 #endif
                cm_ss_error);

        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid Multicall_Status_Index, conn = %d", 
                                     pointer->connection_id);

     }
     else
     {
       
       /* copy invoke data */
       cm_cc_invoke.deflectedToNumber =pointer->deflectedToNumber;

       cm_cc_invoke.defelectedToSubAddress = pointer->deflectedToSubAddress;

       cm_cc_invoke.invoke_id = MN_request_invoke_id(callDeflection); /* new invoke id */

       /* required CD parameters */
       if ((Multicall_Status[Multicall_Status_index].call_activity == CALL_WAITING) &&
           (cm_cc_invoke.invoke_id != INVOKE_FAIL) &&
           (cm_cc_invoke.deflectedToNumber.present))    
       {

           /* set up the cause data */
           cm_cc_cause.present = TRUE;
           cm_cc_cause.location = STD_LOCATION;
           cm_cc_cause.coding_standard = STD_CODESTANDARD;
           cm_cc_cause.recommendation_present = FALSE;
           cm_cc_cause.cause_value = NORMAL_EVENT | NORMAL_CALL_CLEARING;
           cm_cc_cause.diagnostic_length = 0;

           /* send a disconnect message w/ CD invoke data */
           MN_put_MNCC_DISC_REQ(pointer->connection_id, &cm_cc_cause, &cm_cc_invoke);

           /* update status */
           mn_call_information_p[pointer->connection_id]->mo_call_clear_pending = TRUE;

           MN_start_cc_timer(pointer->connection_id, CCE_CALL_DEFLECTION_REQ); /* T2 duration */
  
     }
     else 
     {
           /* CD response error condition */              
           cm_ss_error.present = TRUE;
           cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
           cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

           MN_put_cm_manage_calls_conf(
               FALSE,
#if defined(FEATURE_DUAL_SIM )
               pointer->as_id,
#endif
               cm_ss_error);

           /* Can't perform call deflect */
           MSG_ERROR_DS_2(MN_SUB,"=MNCNM= CD insuff required params, conn = %d, invoke = %d", 
                                                pointer->connection_id, 
                                                cm_cc_invoke.invoke_id);

      } /* CALL_WAITING */

    } /* Multicall_Status_index  */

  }
#if defined(FEATURE_DUAL_SIM )
  else if(MNCNM_IS_SINGLE_STACK)
  {
    MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", pointer->as_id,mn_as_id); 
    cm_ss_error.present = TRUE;
    cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
    cm_ss_error.error_code = AS_REJ_ABORT_RADIO_UNAVAILABLE;
    MN_put_cm_manage_calls_conf(FALSE, pointer->as_id ,cm_ss_error);

  }
#endif
}

/*
 *  Function name : MN_process_cm_4_send
 *  ---------------
 *
 *  Description:
 *  ------------
 *   Handle mobile originated explicit call transfer which
 *   can be invoke with one call in answered and held state, 
 *   and the other is answered and active or alerting state.
 *
 *  Parameters:
 *  -----------
 *   IN message
 *
 *  Returns:
 *
 */

VOID_FUNC   MN_process_cm_4_send(cm_n_send_req_T    *pointer)
{
    byte  invoke_id;
    byte  components[MAX_COMPONENTS_LENGTH/MAX_COMPONENTS]; /* needs only 1 components */
    byte  ss_version[MAX_SS_VERSION_IND_LENGTH];
    ie_cm_ss_error_T   cm_ss_error;
    int   index = 0,  other_index, calls, ect_connection_id, connection_id[2] = {0, 8};  /* start of MO, MT ID */ 
#if defined(FEATURE_DUAL_SIM )
    if((MNCNM_IS_SINGLE_STACK == FALSE) || (pointer->as_id == mn_as_id))
#endif     
    {
    memset(&(cm_ss_error),'\0',sizeof(ie_cm_ss_error_T));
    
    /* Refresh Multicall_Status */
    MN_MULTICALL_LOG(MN_read_multicall_status());

    /* search how many active parties */
    for(calls = 0, index = 0; index < MAXNO_CC_TRANSACTIONS ; index++)  /* for 2nd connection id */
    {
         if((cc_transaction_information_p[index] != NULL) &&
            (cc_transaction_information_p[index]->cc_state != CC_STATE_U0)) 
         {
             connection_id[calls % MN_CALL_WAITING_SESSION] = index;  /* save only two calls */
             calls++;
         } 
    }
 
    /*  normally 2 calls with one in answered and held state, the other in ALERTING or connected state */
    if (((calls == 2) || (calls == 3)) &&  /* 3rd is a CW */
        (((Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_HELD) &&
          (Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_is_mpty == FALSE)) ||
          ((Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_activity == CALL_HELD) &&
          (Multicall_Status[MN_HOLD_ACTIVE_SESSION].call_is_mpty == FALSE))))    
    {    

         index = ((Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_activity == CALL_HELD) ? 
                                        MN_ACTIVE_HOLD_SESSION : MN_HOLD_ACTIVE_SESSION);
         other_index = (index + 1) %2;  /* other index is for checking  active call */
 

         ect_connection_id = Multicall_Status[index].connection_id[0];

         index = ((connection_id[0] == Multicall_Status[index].connection_id[0]) ? 
                   connection_id[1] :  
                   connection_id[0]);  /* connection id of other call not in held  state */

         /* 2nd call state is ACTIVE or ALERTING */
         /* Alerting can be MO alerting /MT alerting / MT call is just answered */ 
         /* active call should not be mpty */
         if((cc_transaction_information_p[index] != NULL) && 
            (((cc_transaction_information_p[index]->cc_state == CC_STATE_U4) ||  
            (cc_transaction_information_p[index]->cc_state == CC_STATE_U7) ||
            (cc_transaction_information_p[index]->cc_state == CC_STATE_U8)) ||
            (cc_transaction_information_p[index]->cc_state == CC_STATE_U10 &&   
			 Multicall_Status[other_index].call_is_mpty == FALSE)))
         {
             /* build in the invoke components */
             invoke_id =  MN_request_invoke_id(explicitCT);  /* new */

             if(invoke_id != INVOKE_FAIL)
             {
                 /* Build the facility INVOKE component */
                 components[0] = INVOKE;    /* 0xa1 */
                 components[1] = 0x06;      /* length */

                 components[2] = INVOKE_ID_TAG;      /*Invoke ID tag */
                 components[3] = 0x01;      /* Invoke ID length */
                 components[4] = invoke_id; /* Invoke ID - for hold_MPTY */

                 components[5] = OP_CODE_TAG;      /*Operational_code tag */
                 components[6] = 0x01;      /*Operational code length */
                 components[7] = explicitCT;

                 ss_version[0] = SS_VERSION_PHASE1;

                 /* Send facility data to CC */
                 MN_put_MNCC_FACILITY_REQ((connection_id_T)ect_connection_id,
                                      components,
                                      8,          /* the component length */
                                      ss_version, /* the ss_version */
                                      1);         /* the ss_version_length */

                 MN_start_cc_timer((connection_id_T)ect_connection_id, 
                               CCE_CALL_TRANSFER_REQ); /* T2 duration */
  
             }
             else 
             {

                 /* CD response error condition */              
                 cm_ss_error.present = TRUE;
                 cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                 cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

                 MN_put_cm_manage_calls_conf(
                     FALSE,
#if defined(FEATURE_DUAL_SIM )
                     pointer->as_id,
#endif
                     cm_ss_error);

                 /* Can't perform call deflect */
                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ExplictCT-2nd invalid invoke, conn=%d, invoke=%d", 
                                          Multicall_Status[0].connection_id[0], 
                                          invoke_id);
             } /* CALL_TRANSFER */

          } /* ACTIVE or ALERTING */
          else
          {
                 /* CD response error condition */              
                 cm_ss_error.present = TRUE;
                 cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                 cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

                 MN_put_cm_manage_calls_conf(
                      FALSE,
#if defined(FEATURE_DUAL_SIM )
                      pointer->as_id,
#endif
                      cm_ss_error);
                 if(cc_transaction_information_p[index] != NULL)
                 {
                   /* Can't perform call deflect */
                   MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ExplictCT-2nd invalid state, conn=%d, state=%d", 
                                          Multicall_Status[0].connection_id[0], 
                                          cc_transaction_information_p[index]->cc_state);
                 }
                 else
                 {
                   MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ExplictCT-2nd invalid state, conn=%d, state=%d", 
                                          Multicall_Status[0].connection_id[0], 
                                          CC_STATE_U0);                   
                 }

          }
    } /* Multicall_Status */
    else
    {

        /* CD response error condition */              
        cm_ss_error.present = TRUE;
        cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
        cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

        MN_put_cm_manage_calls_conf(
                      FALSE,
#if defined(FEATURE_DUAL_SIM )
                      pointer->as_id,
#endif
                      cm_ss_error);

        /* Can't perform call deflect */
        MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ExplictCT-required at least one held call, conn=%d, invoke=%d, type = %d", 
                                          Multicall_Status[0].connection_id[0], 
                                          pointer->message_header.message_id);


     }

   }
#if defined(FEATURE_DUAL_SIM )
   else if(MNCNM_IS_SINGLE_STACK)
   {
      MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", pointer->as_id,mn_as_id); 
      cm_ss_error.present = TRUE;
      cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
      cm_ss_error.error_code = AS_REJ_ABORT_RADIO_UNAVAILABLE;
      MN_put_cm_manage_calls_conf(FALSE, pointer->as_id ,cm_ss_error);
   }
#endif
}

#ifdef FEATURE_CCBS
/*
 *  Function name : MN_process_cm_5_send
 *  ---------------
 *
 *  Description:
 *  ------------
 *   Handle CCBS activation request
 *
 *  Parameters:
 *  -----------
 *   IN message
 *
 *  Returns:
 *
 */
VOID_FUNC   MN_process_cm_5_send(cm_5_send_req_T *msg)
{
   byte              ivk_id, cid=msg->connection_id;
   boolean           msg_ok=FALSE, send_error=TRUE;
   ie_cm_ss_error_T  error;
   ie_facility_T     *ptr=NULL;
   ie_facility_T     fac = {0,0,{0}};

   mn_call_information_T        **mn_info = mn_call_information_p;
   cc_transaction_information_T  **cc_info = cc_transaction_information_p;
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (msg->as_id == mn_as_id))
#endif     
   {
   /* ******************************************************* */
   /*  Prepare facility IE                                    */
   /* ******************************************************* */
   fac.Iei     = IEI_FACILITY;
   fac.Length  = 0x08;
   fac.data[0] = INVOKE;
   fac.data[1] = 0x06;
   fac.data[2] = INVOKE_ID_TAG;
   fac.data[3] = 0x01;
   fac.data[4] = 0x00; /* will be overwritten */
   fac.data[5] = OP_CODE_TAG;
   fac.data[6] = 0x01;
   fac.data[7] = accessRegisterCCEntry;

   /* ******************************************************* */
   /*  Check error first                                      */
   /* ******************************************************* */
   if ( cid                       <  0x07 &&  // CCBS act req is only for MO calls
        cc_info[cid] != NULL &&
        cc_info[cid]->cc_state     == CC_STATE_U12  && 
        mn_info[cid] != NULL &&
        mn_info[cid]->ccbs_allowed            )
   {
      msg_ok = TRUE;
   }
      
   /* ******************************************************* */
   /*  Send messages accordingly                              */
   /* ******************************************************* */
   if (msg_ok)
   {
      ie_cm_cc_cause_T  cm_cc_cause;

      ivk_id =  MN_request_invoke_id (accessRegisterCCEntry);

      if (ivk_id != INVOKE_FAIL)
      {
         fac.data[4] =  ivk_id;
         ptr         = &fac;
         send_error  =  FALSE;
      }

      /* request a disconnect call for specific call */
      memset((VOID_DATA *)&cm_cc_cause, NULL, sizeof(ie_cm_cc_cause_T));

      /* Build cause IE for DISCONNECT */
      cm_cc_cause.location               = STD_LOCATION;
      cm_cc_cause.coding_standard        = STD_CODESTANDARD;
      cm_cc_cause.recommendation_present = FALSE;
      cm_cc_cause.cause_value            = NORMAL_CALL_CLEARING; 
      cm_cc_cause.diagnostic_length      = 0;

      MN_put_MNCC_REL_REQ(msg->connection_id, &cm_cc_cause, NULL, 1, ptr);
   }

   if (send_error)       // send CM_5_SEND_CONF to CM
   {
      error.present        = TRUE;
      error.error_code_tag = 0;
      error.error_code     = 0;
      error.error_type     = UNKNOWN_ERR;

       MN_put_cm_5_send_conf ( msg->connection_id, 
                                CCBS_RET_ERR, 
                                NULL, 
#if defined(FEATURE_DUAL_SIM )
                                msg->as_id,
#endif
                                &error );
   }
   }
#if defined(FEATURE_DUAL_SIM )
   else if(MNCNM_IS_SINGLE_STACK)
   {
     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", msg->as_id,mn_as_id); 
     error.present        = TRUE;
     error.error_code_tag = MN_SS_ERROR_TAG;
     error.error_code     = AS_REJ_ABORT_RADIO_UNAVAILABLE;
     
     MN_put_cm_5_send_conf ( msg->connection_id, 
                             CCBS_RET_ERR, 
                             NULL, 
                             msg->as_id,
                             &error );
   }
#endif
}
#endif /* FEATURE_CCBS */

#ifdef FEATURE_UUS

/*
 *   Function name : MN_process_cm_mo_user_data_req
 *   ---------------
 *
 *   Description:
 *   ------------
 *   This function processes the cm_mo_user_data_req message from the 
 *   cm to MN.
 *
 *   Transfer user-user information to CC. CC will store the the MS -> Network
 *   user-user information and the append it to the next OTA message with
 *   an available field (see GSM 04.08.), pending on the UUS class 1,2,3 
 *   and activation type.
 *  
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */

VOID_FUNC   MN_process_cm_mo_user_data_req(IMH_T    *message)

{
  /* Locals */
  cm_mo_user_data_req_T      *user_data_req, *user_data;

  user_data_req = (cm_mo_user_data_req_T *)message;
#if defined(FEATURE_DUAL_SIM )
  if((MNCNM_IS_SINGLE_STACK == FALSE) ||
     (
     (user_data_req != NULL) &&
     (user_data_req->as_id != SYS_MODEM_AS_ID_NONE && (mn_as_id == SYS_MODEM_AS_ID_NONE || user_data_req->as_id == mn_as_id))
    ))
#endif
    {
    if (user_data_req != NULL)
    {
      connection_id_T  connection_id = user_data_req->connection_id;

      /* Activate Type */
      if (user_data_req->type < UUS_MAX)
      {
        if (connection_id <= MAX_MO_CONNECTION_ID)
        {
          if (mn_call_information_p[connection_id] != NULL)
          {
            /* Implicit activation type */
            if ((!mn_call_information_p[connection_id]->user_user_data.present) && 
                (user_data_req->type != UUS_DATA))
            {
              mn_call_information_p[connection_id]->user_user_data.type = (byte)user_data_req->type;
            }

            /* Check Explicit Activation */
            if ((user_data_req->type == UUS3_REQUIRED) || (user_data_req->type == UUS3_NOT_REQUIRED))
              MN_handle_cm_activate_user_data_req(user_data_req);

            /* Check for any User Data */
            if (user_data_req->data_length &&
              ( ((user_data_req->type <= UUS1_IMPLICIT) && 
                 (user_data_req->data_length <= MAX_USER_USER_DATA_1_LENGTH)) ||
                ((user_data_req->type > UUS1_IMPLICIT) && 
                 (user_data_req->data_length <= MAX_USER_USER_DATA_LENGTH)) ))
            {
              mn_call_information_p[connection_id]->user_user_data.present = TRUE;  /* marked user data */

              user_data = MN_alloc_user_data();

              if (user_data)    /* from user user data pool */
              {
                /* copy & queue it */
                memscpy(user_data, sizeof(cm_mo_user_data_req_T),user_data_req, sizeof(cm_mo_user_data_req_T));

                MN_put_user_data(connection_id, user_data); 
              }
              else
              {
                /* Ignore it */
                MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= cause is CNM_REJ_NO_RESOURCES", 0, 0, 0);

                MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= Out of user data buffer=%d, type=%d, id=%d",(uint32) user_data,
                        (int) user_data_req->type, 
                        connection_id);
              } /* user_data */

            } /* data_length */
            else
            {
              MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= cause is CNM_INVALID_USER_DATA", 0, 0, 0);

              MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= Invalid User Data length=%d, Conn Id=%d", user_data_req->data_length, 
                      connection_id, 0);
            }

            /* Check User User Data transport */
            if (mn_call_information_p[connection_id]->user_user_data.present == TRUE)
            {
              (void)MN_put_USER_USER_DATA_REQ(connection_id);
            }
          }
          else
          {
            MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cannot process MO UUS as mn_call_information_p[%d] is NULL",connection_id);
          }

        } /* connection_id */
        else
        {
          MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= cause is CNM_INVALID_USER_DATA", 0, 0, 0);

          MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= Invalid connection id: data len=%d, Conn Id=%d", user_data_req->data_length, 
                  connection_id, 0);
        }

      } /* user_data_req->type */
      else
      {
        MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= Invalid User Data Type=%d, conn id=%d, cause=%d", (int) user_data_req->type, connection_id, CNM_INVALID_USER_DATA);
      }

    } /* cm_mo_user_data_req */
    else
    {
      MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= Invalid User User Data Request", 0, 0, 0);
    }
  }
#if defined(FEATURE_DUAL_SIM )
  else if((MNCNM_IS_SINGLE_STACK)&&(user_data_req != NULL))
  {
    MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", user_data_req->as_id,mn_as_id); 
  }
#endif
}

/*
 *   Function name : MN_put_USER_USER_DATA_REQ()
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Transfer user-user information if any to CC. The CC will store the MS -> Network
 *   user-user information and the append it to the next OTA message with
 *   an available field (see GSM 04.08.), pending on the UUS class 1,2,3 
 *   and activation type.
 *  
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


byte MN_put_USER_USER_DATA_REQ(connection_id_T connection_id)
{
    /* Locals */
    int                        Multicall_Status_index, index;
    byte                       cause=FALSE;
    cm_mo_user_data_req_T      *user_data;

    /* Get call session */
    Multicall_Status_index = MN_whereami(connection_id);

    if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
    {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
    }
    else if((Multicall_Status[Multicall_Status_index].call_activity != CALL_INACTIVE) &&
       (mn_call_information_p[connection_id] != NULL) &&
       (mn_call_information_p[connection_id]->user_user_data.type > (byte)UUS1_REQUIRED) &&
       (mn_call_information_p[connection_id]->user_user_data.activated != FALSE))
    {
       while((user_data = MN_get_user_data(connection_id)) != NULL)    /* transport all user user data */
       {
         if (user_data->data_length <= MAX_USER_USER_DATA_LENGTH)
         {
           ie_user_user_T   user_user_data;

           user_user_data.Iei = IEI_USER_USER;

           user_user_data.Length = user_data->data_length+1;

           user_user_data.UuProtocolDescriminator = user_data->protocol_discriminator;

           for(index=0; index < user_data->data_length; index++)
           {
               user_user_data.UserUserInformation[index] = user_data->data[index];
           }

           MN_put_MNCC_USER_USER_DATA_REQ(connection_id, 
                      &user_user_data, 
                      user_data->more_data_indicator);

           MN_free_user_data(user_data);

         } /* if (DATA_LENGTH ) */
         else
         {
           cause = CNM_INVALID_USER_DATA;

           MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid User Data length=%d, or Conn Id=%d", user_data->data_length, connection_id);
         }

       }/* while */

    } /* if */

    return cause;

}

/*
 * Function name :  MN_handle_cm_activate_ss_req 
 * -------------
 *
 * Description :
 * -----------  
 *   Activate UUS3 Explicitly.
 *
 *
 * Parameters: cm_abort_ss_req
 * ----------  
 *
 *
 *
 * Returns:    none
 * --------
 *
 */
VOID_FUNC
MN_handle_cm_activate_user_data_req(cm_mo_user_data_req_T *message_ptr)
{
    int                        Multicall_Status_index;
    byte         invoke_id;
    byte         components[16];
    byte         ss_version[MAX_SS_VERSION_IND_LENGTH];

    /* Get call session */
    Multicall_Status_index = MN_whereami(message_ptr->connection_id);
   
    if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
    {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
    }
    else if((Multicall_Status[Multicall_Status_index].call_activity != CALL_INACTIVE) &&
       (message_ptr->type >= UUS3_REQUIRED))
    {
         byte uUS, uUSRequired;

         if(message_ptr->type == UUS3_REQUIRED)
         {
             uUS = 3; 
             uUSRequired = TRUE;
         }
         else if (message_ptr->type == UUS3_NOT_REQUIRED)
         {
             uUS = 3; 
             uUSRequired = FALSE;
         }
         else
         {
             uUS = 3;
             uUSRequired = FALSE;

             MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid USS3 type=%d, conn id=%d", message_ptr->type, message_ptr->connection_id);
         }

         /* 
          * successful change of call status - send a build multiparty 
          * request to the network 
          */
         invoke_id = MN_request_invoke_id(userUserService);

         components[0] = INVOKE; /* Component type tag */
         components[1] = 14; /* Component length */
         components[2] = INVOKE_ID_TAG;   /* Invoke id tag */
         components[3] = 0x01;   /* Invoke id length */
         components[4] = invoke_id;
         components[5] = OP_CODE_TAG;   /* op-code tag */
         components[6] = 0x01;   /* op-code length */
         components[7] = userUserService;  /* op-code */
         components[8] = 0x30;   /* userUserService Sequence tag */
         components[9] = 0x06; /* userUserService Sequence length */
         components[10] = 0x80; /* uUS-Service Enumerated tag */
         components[11] = 0x01; /* uUS-Service Enumerated length */
         components[12] = uUS; /* uUS-Service */
         components[13] = 0x81; /* uUS-Required tag */
         components[14] = 0x01; /* uUS-Required tag */
         components[15] = uUSRequired; /* uUS-Required value */

         /* set up the ss version data */

         ss_version[0] = 0;

         MN_put_MNCC_FACILITY_REQ(
               message_ptr->connection_id,  /*Anyone will do */
               components,
               16,            /* component_length */
               ss_version,
               1);

         /* Start the timer */
         MN_start_ss_timer(message_ptr->connection_id, invoke_id, 10000); /* 10 sec */

    } /* USS3 */
    else
    {
        ERR("Invalid connection ID = %d or UUS type =%d",message_ptr->connection_id, message_ptr->type, 0);
    }

}

#endif  /* FEATURE_UUS */


