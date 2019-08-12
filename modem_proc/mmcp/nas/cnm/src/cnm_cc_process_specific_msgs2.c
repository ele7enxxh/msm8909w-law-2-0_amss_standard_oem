/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2011, 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_cc_process_specific_msgs2.c_v   1.13   12 Jul 2002 17:13:30   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_cc_process_specific_msgs2.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

3-Dec-2001  VT    Bug Fixes: 
                    In CNM_process_mo_disc_request(): 
                      Moved calls to CNM_stop_timer() and 
                        CNM_cc_respond_to_event() to the bottom of if (action != NULL)

                      Changed the nested if...else to switch (cc_state)...case...

                      Introduced a new case for CC_STATE_U6, to send RELEASE_COMPLETE.

5-Dec-2001  VT    Bug Fixes:  In CNM_process_mo_disc_request(), in case CC_STATE_U0_1,
                    also send MNCC_REL_IND to MN.

5-Dec-2001  VT    Bug Fixes: In CNM_process_mo_disc_request(), in case CC_STATE_U0_1,
                    set mm_connection_pending.CNM_service_type to NO_CNM_SERVICE.

12-Mar-2002 VT    Removed lint errors

12-Mar-2002  AB   Insert Call Deflection invoke component in the FACILITY IE, 
                  if available, when transaction is in CC_STATE_U7 or CC_STATE_U9 state 
                  in CNM_process_mo_disc_request().

02-April-2002 AB  The cause IE in DISCONNECT message does not have type, just
                  length and value - adjusted accordingly.

04/19/02   CD     Changed  CNM_process_mo_disc_request() processing for state CC_STATE_U0_1

                  Changed processing of CNM_process_mt_call_reject() to include 
                  sending a MMCNM_REL_REQ to MM when the last connection is processed
    
04/30/02   CD     When calling CNM_send_mn_rel_cnf(), replaced argument 0x00 by NULL
                  Updated Copyright

                  Added comments to CNM_process_mo_disconnect_req()

05/07/02    CD    Changed release type enum due to name collision with RR

05/15/02    CD    In CNM_process_mo_call_request(), added comments to clarify when
                  T303 is started

06/28/02    CD    In CNM_process_mt_call_reject(), added the cause to the 
                  MNCC_REL_CNF primitive.

07/01/02    CD    Removed 'unusual pointer cast lint' errors
                  Removed 'loss of sign in promotion' lint errors

09/16/02    AB    Added CNM_process_mo_release_complete_request(),
                  to sent a RELEASE_COMPLETE message to the network.

02/12/03    AB    In CNM_process_mo_call_request(), corrected the 
                  bytes_remaining calculation per Lint.

05/15/05    HS    Added support for FEATURE_CCBS

03/27/06    NR    Hack for CR 73905, accelerating the release procedure when RAB is released in active state. 
 
07/04/11    AM    Putting range check for fixing buffer overflow of cc_IE_info.

07/06/11    PM    Guarding against NULL pointer access
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "com_iei_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"
#include "ms.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"
#include "cause.h"

#include "nasutils.h"
#include "cmlib.h"
#include "cnm.h"
#include <stringl/stringl.h>


/**************** Begin of   CNM_process_status_message ****************************
 *
 *  Function name: CNM_process_status_message()
 *  -----------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a status message received from the network. It
 *  checks the syntax of each IE, generating a status message if incorrect,
 *  or, if the reported status is 98 or 101, returns a release complete
 *  message with cause 98.
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_status_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{
   byte                       IEI;
   byte                       IE_count;
   IE_count = 2;              /* start at first IE after message type */

   if ( no_of_IEs > MAXNO_IES_IN_MESSAGE )
   {
     ERR("no_of_IEs exceeds MAXNO_IES_IN_MESSAGE", 0, 0, 0);
     no_of_IEs = MAXNO_IES_IN_MESSAGE;
   }

   while (( syntax_ok != FALSE ) && ( IE_count < no_of_IEs ))
   {

         /*
         * get information of the next IE from global store
         */

         IEI       = cc_IE_info[IE_count].IEI;

         if(cc_IE_info[IE_count].syntax_ok == TRUE)
         {
           switch (IEI)
           {
              case IEI_CAUSE: 
              /*
               * Processing of this IE has been done earlier in function CNM_perform_check_on_IEs()
               */
              case IEI_CALL_STATE:
              case IEI_AUXILIARY_STATES:
                 /* does nothing */
                 break;

              default:                            /* this cannot happen! */
                 ERR("Unexpected IE in status msg: received IEI = %d\n",
                      (dword) IEI, 0, 0);

                 syntax_ok = FALSE;
                 break;
           }
         }

         IE_count++;       /* index next IE in the message */

   }   /* END WHILE more IEs and Syntax is OK */

   if ( !syntax_ok )
   {
      ERR("Syntax error in Status Message\n", 0, 0, 0);
      CNM_send_status ( connection_id,
                           PD_CC,
                           cause );
   }
}
/**************** End of   CNM_process_status_message ****************************/


/**************** Begin of   CNM_process_status_enquiry_message ********************
 *
 *  Function name: CNM_process_status_enquiry_message()
 *  --------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a status enquiry message received from the 
 *  network. It responds by returning a status message to the network
 *  with cause response to status enquiry
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none 
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_status_enquiry_message( connection_id_T connection_id ) 
{

   CNM_send_status ( connection_id, 
                     PD_CC,
                     RESPONSE_TO_STATUS_ENQUIRY );

}

/**************** End of   CNM_process_status_enquiry_message ****************************/

#ifdef FEATURE_UUS

/**************** Begin of    CNM_process_user_information_message ****************
 *
 *  Function name: CNM_process_user_information_message()
 *  ----------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a user information message received from the 
 *  network. It checks the syntax, then,if ok, sends the data to MN 
 *  (Otherwise a syntax error, a status message is returned to the 
 *  network and no data is passed on)
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none 
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_user_information_message( connection_id_T connection_id, boolean syntax_ok, byte cause) 
{
   MNCC_USER_USER_DATA_IND_T  mncc_user_user_data_ind;
   byte                       *p_IE;
   byte                       IEI;
   byte                       IE_count;
   byte                       IE_length ;
   byte                       *p_write_point;
   byte                       *p_end_write_point; /* last octet where appending of IE data is allowed.*/

   dword                      len;

   p_IE = (byte *) 0;
   IE_count = 2;              /* start at first IE after message type */

   mncc_user_user_data_ind.message_header.message_set = MS_MN_CC;
   mncc_user_user_data_ind.message_header.message_id  = 
                                                MNCC_USER_USER_DATA_IND;
   mncc_user_user_data_ind.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_user_user_data_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*
    * set up write pointer ready to append the IEs 
    */

   p_write_point = (byte *)mncc_user_user_data_ind.data;
   p_end_write_point = p_write_point + sizeof(mncc_user_user_data_ind.data) - 1;

   if ( no_of_IEs > MAXNO_IES_IN_MESSAGE )
   {
     ERR("no_of_IEs exceeds MAXNO_IES_IN_MESSAGE", 0, 0, 0);
     no_of_IEs = MAXNO_IES_IN_MESSAGE;
   }

   while (( syntax_ok != FALSE ) && ( IE_count < no_of_IEs ))
   {

      /*
       * get information of the next IE from global store
       */

      IEI       = cc_IE_info[IE_count].IEI;
      p_IE      = cc_IE_info[IE_count].start;
      IE_length = cc_IE_info[IE_count].length;
      
      if(cc_IE_info[IE_count].syntax_ok == TRUE)
      {
        switch (IEI)
        {
            
           /*
            * accept the following two IEs
            * IEI_USER_USER is stored in TLV format 
            * p_IE[1] contains the length.
            */

           case IEI_USER_USER:
            {
              if(CNM_append_IE_to_message( p_IE, 
                                     (byte **)&p_write_point, 
                                     (byte)(p_IE[1] + 2),
                                     p_end_write_point) )
              {

                /*
                 * fill in missing IEI
                 * CNM_append_IE_to_message() has moved pointer p_write_point by (p_IE[1] + 2), 
                 * so subtract (p_IE[1] + 2) from current p_write_point position 
                 * to obtain the correct position for writing IEI value.
                 */

                *(p_write_point - (p_IE[1] + 2)) = IEI_USER_USER;
              }

            }

            break;

           case IEI_MORE_DATA:
            {
              (void) CNM_append_IE_to_message( p_IE, 
                                       (byte **)&p_write_point, 
                                       IE_length,
                                       p_end_write_point);
            }
            break;

           default:                            /* this cannot happen! */
            ERR("Unexpected IE in user information msg: received IEI = %d\n",
                 (dword) IEI, 0, 0);

            syntax_ok = FALSE;
            break;
        }
      }

      IE_count++;       /* index next IE in the message */

   }   /* END WHILE more IEs and Syntax is OK */

   if ( syntax_ok )
   {
      len = (dword)( (byte *)p_write_point -
            (byte *)&mncc_user_user_data_ind.as_id);

      PUT_IMH_LEN( len,
                   &mncc_user_user_data_ind.message_header);

      CNM_send_message( (IMH_T *)&mncc_user_user_data_ind, 
                        GS_QUEUE_MN_CM );
   }
   else
   {
      ERR("Syntax error in User Information Message\n", 0, 0, 0);

      CNM_send_status ( connection_id, 
                                      PD_CC,
                                      cause );
   }
}

/**************** End of   CNM_process_user_information_message ****************************/


/**************** Begin of   CNM_process_congestion_control_message ***************
 *
 *  Function name: CNM_process_congestion_control_message()
 *  ------------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a congestion control message received from the
 *  network. It checks the syntax, then does nothing else. Future usage
 *  possible.
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_congestion_control_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{
   byte                       IEI;
   byte                       IE_count;

   IE_count = 2;              /* start at first IE after message type */

   if ( no_of_IEs > MAXNO_IES_IN_MESSAGE )
   {
     ERR("no_of_IEs exceeds MAXNO_IES_IN_MESSAGE", 0, 0, 0);
     no_of_IEs = MAXNO_IES_IN_MESSAGE;
   }

   /*
    * set up write pointer ready to append the IEs
    */

   while (( syntax_ok != FALSE ) && ( IE_count < no_of_IEs ))
   {

      /*
       * get information of the next IE from global store
       */

      IEI       = cc_IE_info[IE_count].IEI;
      if(cc_IE_info[IE_count].syntax_ok == TRUE)
      {
           switch (IEI)
           {

              /*
               * accept the following two IEs
               */


              case IEI_CONGESTION_CONTROL:  /* NOTE: this IE has no IEI here */
              case IEI_CAUSE:
                 break;

              default:                            /* this cannot happen! */
                 ERR("Unexpected IE in Congestion Control msg: received IEI = %d\n",
                       (dword) IEI, 0, 0);

                 syntax_ok = FALSE;
                 break;
           }
      }


      IE_count++;       /* index next IE in the message */

   }   /* END WHILE more IEs and Syntax is OK */

   if ( !syntax_ok )
   {
      ERR("Syntax error in Congestion Control msg\n",  0, 0, 0);
      CNM_send_status ( connection_id,
                           PD_CC,
                           cause );
   }
}
/**************** End of   CNM_process_congestion_control_message ****************************/
#endif  /* FEATURE_UUS */


/**************** Begin of   CNM_process_notify_message ***************************
 *
 *  Function name: CNM_process_notify_message()
 *  -----------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a notify message received from the network. It 
 *  checks the syntax of each IE, generating a status message if incorrect,
 *  or collects the notification indicator information element and adds it to
 *  an MNCC_NOTIFY_IND message which is sent to MN.
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none 
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_notify_message( connection_id_T connection_id, boolean syntax_ok, byte cause) 
{
   MNCC_NOTIFY_IND_T          mncc_notify_ind;
   byte                       *p_IE;
   byte                       IEI;
   byte                       IE_count;
   byte                       IE_length ;
   byte                       *p_write_point;
   byte                       *p_end_write_point;

   dword                      len;

   p_IE = (byte *) 0;
   IE_count = 2;              /* start at first IE after message type */

   mncc_notify_ind.message_header.message_set = MS_MN_CC;
   mncc_notify_ind.message_header.message_id  = MNCC_NOTIFY_IND;
   mncc_notify_ind.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_notify_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   /*
    * set up write pointer ready to append the IEs 
    */

   p_write_point = (byte *)&mncc_notify_ind.notification_indicator;
   p_end_write_point = p_write_point + sizeof(mncc_notify_ind.notification_indicator) - 1;

   if ( no_of_IEs > MAXNO_IES_IN_MESSAGE )
   {
     ERR("no_of_IEs exceeds MAXNO_IES_IN_MESSAGE", 0, 0, 0);
     no_of_IEs = MAXNO_IES_IN_MESSAGE;
   }
   while (( syntax_ok != FALSE ) && ( IE_count < no_of_IEs ))
   {

      /*
       * get information of the next IE from global store
       */

      IEI       = cc_IE_info[IE_count].IEI;
      p_IE      = cc_IE_info[IE_count].start;
      IE_length = cc_IE_info[IE_count].length;
      if(cc_IE_info[IE_count].syntax_ok == TRUE)
      {
        switch (IEI)
        {
           case IEI_NOTIFICATION_IND:
              {
                if( CNM_append_IE_to_message( p_IE, 
                                         (byte **)&p_write_point, 
                                         IE_length,
                                         p_end_write_point))
                {
                  /*
                   * fill in the missing IEI for the norification indicator field
                   */

                  mncc_notify_ind.notification_indicator.Iei = 
                     IEI_NOTIFICATION_IND;
                }

              }
              break;

           default:                            /* this cannot happen! */
              ERR("Unexpected IE in notify msg: received IEI = %d\n",
                   (dword) IEI, 0, 0);

              syntax_ok = FALSE;
              break;
        }
      }

 
      IE_count++;       /* index next IE in the message */

   }   /* END WHILE more IEs and Syntax is OK */

   if ( syntax_ok )
   {
      len = (dword)( (byte *)p_write_point -
               (byte *)&mncc_notify_ind.as_id);

      PUT_IMH_LEN( len,
                   &mncc_notify_ind.message_header);

      CNM_send_message( (IMH_T *)&mncc_notify_ind, 
                        GS_QUEUE_MN_CM );
   }
   else
   {
      ERR("Syntax error in Notify msg\n", 0, 0, 0);

      CNM_send_status ( connection_id, 
                                      PD_CC,
                                      cause );
   }
}
/**************** End of   CNM_process_notify_message ****************************/


/**************** Begin of   CNM_process_mo_call_request **************************
 *
 *  Function name: CNM_process_mo_call_request()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle a setup request from MN. The function
 *  checks that the message can be handled in the current state,
 *  and if so, forms a setup request and sends it on.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC  CNM_process_mo_call_request( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_SETUP_REQ_T     *p_mncc_setup_req;
   setup_msg_T          *p_setup_msg;
   boolean              action;
   unsigned int         length;
   unsigned int         bytes_remaining;

   p_setup_msg = (setup_msg_T*) CNM_alloc( sizeof( setup_msg_T ));

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_setup_req = (MNCC_SETUP_REQ_T *)((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_setup_req->connection_id;

   action =
        CNM_cc_check_respond_to_event(connection_id, MNCC_SETUP_REQ_event);

   if ( action != 0 )
   {
      p_setup_msg->msg_head.IMH.message_set = MS_CC_MM;
      p_setup_msg->msg_head.IMH.message_id  = MMCNM_DATA_REQ;
      p_setup_msg->msg_head.ti_pd = (unsigned char) (FORM_TI_PD(connection_id, PD_CC));
      p_setup_msg->msg_head.type = SETUP;

      /*
       * override defaults for special cases
       */

      switch (p_mncc_setup_req->CNM_service_type)
      {
         case MO_CALL_ESTABLISHMENT:
         case MO_CALL_ESTABLISHMENT_FULL_SPEECH:
         case MO_CALL_ESTABLISHMENT_HALF_SPEECH:
         case MO_CALL_ESTABLISHMENT_DATA_9600:
         case MO_CALL_ESTABLISHMENT_DATA_4800:
         case MO_CALL_ESTABLISHMENT_DATA_2400:
            break;

         case EMERGENCY_CALL_ESTABLISHMENT:
            p_setup_msg->msg_head.type = EMERGENCY_SETUP;
            break;

         case SHORT_MESSAGE_SERVICE_ESTABLISHMENT:
            p_setup_msg->msg_head.ti_pd = (unsigned char) (FORM_TI_PD(connection_id, PD_SMS));
            break;

         case SUPPLEMENTARY_SERVICE_ACTIVATION:
            p_setup_msg->msg_head.ti_pd = (unsigned char) (FORM_TI_PD(connection_id, PD_SS));
            break;

         default:
            ERR("Unexpected CNM_service_type in MO call_request: received CNM_service_type = %d\n",
                 (p_mncc_setup_req->CNM_service_type), 0, 0);
            break;
      }

      GET_IMH_LEN(length, &p_mncc_setup_req->message_header);

      bytes_remaining = length;

      bytes_remaining -= sizeof(sys_modem_as_id_e_type); /* because of as_id addition in the structure*/

      /* Check why length is not decremented because of connection_id */

      if (bytes_remaining)
      {
         (void) memscpy( p_setup_msg->data,
                        sizeof(p_setup_msg->data),
                        p_mncc_setup_req->data,
                        bytes_remaining );

         PUT_IMH_LEN( bytes_remaining, &p_setup_msg->msg_head.IMH)

         /*
          * Change the CC state to CC_STATE_U0_1
          */

         CNM_cc_respond_to_event( connection_id, action );

#ifdef FEATURE_CCBS
         if (last_state == CC_STATE_U0_6) /* setup due to recall */
         {
             /* no need to send MMCNM_EST_REQ and move to CC_STATE_U0.1 */

             CNM_send_primitive_to_mm(connection_id, PD_CC, (IMH_T *)p_setup_msg);
         }
         else
#endif /* FEATURE_CCBS */
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
         if ( (last_state == CC_STATE_U0) && 
              (cc_transaction_information_p[connection_id] != NULL) &&
              (cc_transaction_information_p[connection_id]->repeat == TRUE) )
         {
             action =
                 CNM_cc_check_respond_to_event(connection_id, MMCC_EST_CONF_event);

             if(action != 0)
             {
                 CNM_cc_respond_to_event(connection_id, action);

                 /* no need to send MMCNM_EST_REQ and move to CC_STATE_U0.1 */

                 CNM_send_primitive_to_mm(connection_id, PD_CC, &p_setup_msg->msg_head.IMH );
             }
             else
             {
                 MSG_ERROR_DS_3(MN_SUB,"=MNCNM= CC: %d, state: %d, event: %d", connection_id, 
                            cc_transaction_information_p[connection_id]->cc_state, 
                            MMCC_EST_CONF_event);
             }
         }
         else
#endif /* FALLBACK */
         {
            /* 
             * If there are no pending mm connection establishment request, send the
             * request and start T303 timer
             */

            CNM_manage_cnm_service_requests(p_mncc_setup_req->CNM_service_type,
                                            connection_id,
                                            &p_setup_msg->msg_head.IMH);
         }
      }
      else
      {
         ERR("Setup req msg incomplete: remaining bytes = %d\n",
              (dword) bytes_remaining, 0, 0);
      }

   }
   else
   {
      ERR("Incorrect/Unacceptable setup req: leads to no action\n",
            0, 0, 0);
   }

   CNM_free( p_setup_msg );
}
/**************** End of   CNM_process_mo_call_request ****************************/


/**************** Begin of   CNM_process_mt_call_confirm **************************
 *
 *  Function name: CNM_process_mt_call_confirm()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle acceptance of a mobile terminated call setup.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a call_confirmed message and sends it on to
 *  the network.
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mt_call_confirm( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_CALL_CONF_REQ_T *p_mncc_call_conf;
   call_confirmed_msg_T call_confirmed_msg;
   byte                 action;
   unsigned int         length;
   unsigned int         bytes_remaining;

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_call_conf = (MNCC_CALL_CONF_REQ_T *)((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_call_conf->connection_id;

   action =
        CNM_cc_check_respond_to_event(connection_id, MNCC_CALL_CONF_REQ_event );

   if ( action != 0 )
   {

      CNM_cc_respond_to_event(connection_id, action );

      call_confirmed_msg.msg_head.type = CALL_CONFIRMED;


      GET_IMH_LEN(length, &p_mncc_call_conf->message_header );

      bytes_remaining = length - sizeof(connection_id_T) - sizeof(sys_modem_as_id_e_type);

      if ( bytes_remaining != 0 )
      {
         bytes_remaining = memscpy( &call_confirmed_msg.data[0],
                           sizeof(call_confirmed_msg.data),
                           p_mncc_call_conf->data,
                           bytes_remaining );
      }

      PUT_IMH_LEN(bytes_remaining +
                  sizeof(call_confirmed_msg.msg_head.ti_pd) +
                  sizeof(call_confirmed_msg.msg_head.type),
                  &call_confirmed_msg.msg_head.IMH );

      CNM_send_primitive_to_mm(connection_id,
                              PD_CC,
                              (IMH_T *)&call_confirmed_msg );

   }
}
/**************** End of   CNM_process_mt_call_confirm ****************************/


/**************** Begin of   CNM_process_mt_call_reject ***************************
 *
 *  Function name: CNM_process_mt_call_reject()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle rejection of a mobile terminated call setup.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a call rejection and sends it on.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mt_call_reject( IMH_T * message )
{
   connection_id_T         connection_id;
   MNCC_REJ_REQ_T          *p_mncc_rej_req;
   release_complete_msg_T  *p_data_req_msg;
   byte                    action;
   byte                    cause[GSM_CAUSE_SIZE];
   unsigned int            cause_size = 0;

   p_data_req_msg = (release_complete_msg_T*) CNM_alloc( sizeof( release_complete_msg_T ));

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_rej_req = (MNCC_REJ_REQ_T *) ((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_rej_req->connection_id;

   action =
        CNM_cc_check_respond_to_event(connection_id, MNCC_REJ_REQ_event );

   if ( action != 0 )
   {
      CNM_cc_respond_to_event(connection_id, action );

      /* 
       * Start clearing procedures 
       */

      p_data_req_msg->msg_head.type  = RELEASE_COMPLETE;

      cause_size  = p_mncc_rej_req->cause[1]+2*sizeof(byte);

      if(cause_size > sizeof( p_mncc_rej_req->cause ))
      {
          cause_size = sizeof( p_mncc_rej_req->cause );
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= cause_size Length > sizeof( p_mncc_rej_req->cause ) which is  GSM_CAUSE_SIZE, truncating\n");
      }

      (void)memscpy(p_data_req_msg->data,
                   sizeof(p_data_req_msg->data),
                   p_mncc_rej_req->cause,
                   cause_size);

      PUT_IMH_LEN(cause_size +
                  sizeof(p_data_req_msg->msg_head.ti_pd) +
                  sizeof(p_data_req_msg->msg_head.type),
                  &p_data_req_msg->msg_head.IMH );

      CNM_send_primitive_to_mm(connection_id,
                              PD_CC,
                              (IMH_T *)p_data_req_msg );

      /* Inform MN that CNM has started release procedures 
       * Include the same cause in the message
       */
       (void)memscpy(cause, 
                    sizeof(cause),
                    p_mncc_rej_req->cause,
                    cause_size);

       CNM_send_mn_rel_cnf(connection_id, 0, (int)cause_size, cause);

      /* If this is the last connection,  send MM a request to release the
       * MM connection 
       */
      (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE
#if defined(FEATURE_DUAL_SIM )
                                        , mn_as_id
#endif
                                          );

   }

   CNM_free( p_data_req_msg );

}
/**************** End of   CNM_process_mt_call_reject ****************************/


/**************** Begin of   CNM_process_mt_call_answer ***************************
 *
 *  Function name: CNM_process_mt_call_answer()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle connection of a mobile terminated call setup.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a call connection and sends it on.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mt_call_answer( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_SETUP_RES_T     *p_mncc_setup_res;
   connect_msg_T        *p_data_req_msg;
   byte                 action;
   unsigned int         length;
   unsigned int         bytes_remaining;

   p_data_req_msg = (connect_msg_T*) CNM_alloc( sizeof( connect_msg_T ));

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_setup_res = (MNCC_SETUP_RES_T *) ((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_setup_res->connection_id;

   action =
        CNM_cc_check_respond_to_event(connection_id, MNCC_SETUP_RSP_event );

   if ( action != 0 )
   {
      CNM_cc_respond_to_event(connection_id, action );

      p_data_req_msg->msg_head.type  = CONNECT;


      GET_IMH_LEN(length, &p_mncc_setup_res->message_header );

      bytes_remaining = length - sizeof(connection_id_T) - sizeof(sys_modem_as_id_e_type);

      if ( bytes_remaining != 0 )
      {
         bytes_remaining = memscpy( p_data_req_msg->data,
                           sizeof(p_data_req_msg->data),
                           p_mncc_setup_res->data,
                           bytes_remaining );

      }

      PUT_IMH_LEN(bytes_remaining +
                   sizeof(p_data_req_msg->msg_head.ti_pd) +
                   sizeof(p_data_req_msg->msg_head.type),
                   &p_data_req_msg->msg_head.IMH );


      CNM_send_primitive_to_mm(connection_id,
                              PD_CC,
                              (IMH_T *)p_data_req_msg );
   }

   CNM_free( p_data_req_msg );
}
/**************** End of   CNM_process_mt_call_answer ****************************/


/**************** Begin of   CNM_process_user_conn_confirmation *******************
 *
 *  Function name: CNM_process_user_conn_confirmation()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle message indicating that MN has attached
 *  the user connection.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a call connection and sends it on.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_user_conn_confirmation( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_SETUP_COMPL_REQ_T  *p_mncc_setup_compl;
   connect_ack_msg_T    data_req_msg;
   byte                 action;

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_setup_compl = (MNCC_SETUP_COMPL_REQ_T *) ((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_setup_compl->connection_id;

   action = 
        CNM_cc_check_respond_to_event(connection_id, 
                                     MNCC_SETUP_COMPL_RES_event );

   if ( action != 0 )
   {
      CNM_cc_respond_to_event(connection_id, action );

      data_req_msg.msg_head.type  = CONNECT_ACK;                     


      PUT_IMH_LEN(sizeof(data_req_msg.msg_head.ti_pd) +
                      sizeof(data_req_msg.msg_head.type),
                      &data_req_msg.msg_head.IMH );
         

      CNM_send_primitive_to_mm(connection_id,
                              PD_CC,
                              (IMH_T *)&data_req_msg);

   }

}
/**************** End of   CNM_process_user_conn_confirmation **************************/


/**************** Begin of   CNM_process_mo_alert_request *************************
 *
 *  Function name: CNM_process_mo_alert_request()
 *  --------------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle message indicating that MN has attached
 *  the user connection.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a call connect and sends it on.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mo_alert_request( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_ALERT_REQ_T     *p_mncc_alert_req;
   connect_ack_msg_T    data_req_msg;
   byte                 action;

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_alert_req = (MNCC_ALERT_REQ_T *) ((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_alert_req->connection_id;

   action = 
        CNM_cc_check_respond_to_event(connection_id, MNCC_ALERT_REQ_event );

   if ( action != 0 )
   {
      CNM_cc_respond_to_event(connection_id, action );

      data_req_msg.msg_head.type  = ALERTING;


      PUT_IMH_LEN(sizeof(data_req_msg.msg_head.ti_pd) +
                      sizeof(data_req_msg.msg_head.type),
                      &data_req_msg.msg_head.IMH );
         

      CNM_send_primitive_to_mm(connection_id,
                              PD_CC,
                              (IMH_T *)&data_req_msg );

   }

}
/**************** End of   CNM_process_mo_alert_request ****************************/


/**************** Begin of   CNM_process_mo_disc_request **************************
 *
 *  Function name: CNM_process_mo_disc_request()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle MN is requested disconnection of the user connection.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a call rejection and sends it on.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mo_disc_request( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_DISC_REQ_T      *p_mncc_disc_req;
   disconnect_msg_T     *p_data_req_msg;
   release_msg_T        *p_release_msg;
   release_complete_msg_T  *p_release_complete_msg;
   byte                 action, cause_length;
   cc_state_T           cc_state;
   boolean              cc_rab_released = FALSE;
   int                  min_size;

   p_data_req_msg = (disconnect_msg_T*) CNM_alloc( sizeof( disconnect_msg_T ));
   p_release_msg = (release_msg_T*) CNM_alloc( sizeof( release_msg_T ));
   p_release_complete_msg = (release_complete_msg_T*) CNM_alloc( sizeof(release_complete_msg_T));

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_disc_req = (MNCC_DISC_REQ_T *) ((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_disc_req->connection_id;

   action =
        CNM_cc_check_respond_to_event(connection_id,
                                     MNCC_DISC_REQ_event );

   if ( action != NULL )    /* actually comparing with A_BAD_EVENT in 
                             * CNM_cc_state_machine.c  
                             */
   {
      cc_state = cc_transaction_information_p[connection_id]->cc_state;

      /* Check for CC call clearing due to RR/RRC RAB RELEASED */
      if(p_mncc_disc_req->data[3] == CNM_MM_REL_PENDING)
      {
          cc_rab_released = TRUE;

          p_mncc_disc_req->data[3] = CHANNEL_UNACCEPTABLE | 0x80;
      }

      switch (cc_state)
      {
          case CC_STATE_U0_1:
#if defined(FEATURE_DUAL_SIM )
              if(MNCNM_IS_SINGLE_STACK)
              {
                 if((mn_as_id == SYS_MODEM_AS_ID_NONE) && (mn_call_information_p[connection_id] != NULL))
                 {
                   mn_as_id = mn_call_information_p[connection_id]->as_id;
                 }
              }
#endif
              CNM_set_state_to_U0( connection_id );

              /* [24.008, 4.5.1.7]
               * If there is other active connections, start the normal clearing procedures
               * by sending a DISCONNECT message
               * Else, send a mm connection release request to MM and 
               * inform MN is informed ot his action
               */

              CNM_check_for_last_connection(CNM_ESTABLISHMENT_IN_PROGRESS
#if defined(FEATURE_DUAL_SIM )
                                                                     , mn_as_id
#endif
                                                                       );
             /*
              * Inform MN that CNM has sent a release req to MM
              */
              CNM_send_mn_rel_cnf(connection_id, 0,0,NULL);

              break;

          case CC_STATE_U12:

              min_size = (int)MIN( sizeof(cc_transaction_information_p[connection_id]->cc_cause) , sizeof(p_mncc_disc_req->data));

              if( ( p_mncc_disc_req->data[1] + 2 * sizeof(byte) ) > (byte)min_size)
              {
                   p_mncc_disc_req->data[1] = min_size - ( 2 * sizeof(byte) );
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid p_mncc_disc_req->data[1] value, truncating\n");
              }
              (void)memscpy(cc_transaction_information_p[connection_id]->cc_cause,
                            sizeof(cc_transaction_information_p[connection_id]->cc_cause),
                            p_mncc_disc_req->data,
                            p_mncc_disc_req->data[1] + 2 * sizeof(byte) );

              p_release_msg->msg_head.type  = RELEASE;

              min_size = (int) MIN(sizeof(p_release_msg->data) , sizeof(p_mncc_disc_req->data));
              if( ( p_mncc_disc_req->data[1] + 2 * sizeof(byte) ) >(byte) min_size)
              {
                   p_mncc_disc_req->data[1] = min_size - ( 2 * sizeof(byte) );
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid p_mncc_disc_req->data[1] value, truncating\n");
              }
              (void)memscpy(&p_release_msg->data[0],
                            sizeof(p_release_msg->data),
                            p_mncc_disc_req->data,
                            p_mncc_disc_req->data[1] + 2 * sizeof(byte) );

              PUT_IMH_LEN(sizeof(p_release_msg->msg_head.ti_pd) +
                           sizeof(p_release_msg->msg_head.type ) +
                           p_mncc_disc_req->data[1] + 2*sizeof(byte),
                           &p_release_msg->msg_head.IMH );

              CNM_send_primitive_to_mm(connection_id,
                                        PD_CC,
                                        (IMH_T *)p_release_msg );
              break;

          case CC_STATE_U6:

              min_size = (int) MIN(sizeof(cc_transaction_information_p[connection_id]->cc_cause) , sizeof(p_mncc_disc_req->data));
              if( ( p_mncc_disc_req->data[1] + 2 * sizeof(byte) ) >(byte) min_size)
              {
                   p_mncc_disc_req->data[1] = min_size - ( 2 * sizeof(byte) );
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid p_mncc_disc_req->data[1] value, truncating\n");
              }
              (void)memscpy(cc_transaction_information_p[connection_id]->cc_cause,
                            sizeof(cc_transaction_information_p[connection_id]->cc_cause),
                            p_mncc_disc_req->data,
                            p_mncc_disc_req->data[1] + 2 * sizeof(byte) );

              p_release_complete_msg->msg_head.type  = RELEASE_COMPLETE;

              min_size = (int) MIN( sizeof(p_release_complete_msg->data) , sizeof(p_mncc_disc_req->data));
              if( ( p_mncc_disc_req->data[1] + 2 * sizeof(byte) ) > (byte)min_size)
              {
                   p_mncc_disc_req->data[1] = min_size - ( 2 * sizeof(byte) );
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid p_mncc_disc_req->data[1] value, truncating\n");
              }
              (void)memscpy(&p_release_complete_msg->data[0],
                            sizeof(p_release_complete_msg->data),
                            p_mncc_disc_req->data,
                            p_mncc_disc_req->data[1] + 2 * sizeof(byte) );

              PUT_IMH_LEN(sizeof(p_release_complete_msg->msg_head.ti_pd) +
                           sizeof(p_release_complete_msg->msg_head.type ) +
                           p_mncc_disc_req->data[1] + 2*sizeof(byte),
                           &p_release_complete_msg->msg_head.IMH );

              CNM_send_primitive_to_mm(connection_id,
                                        PD_CC,
                                        (IMH_T *)p_release_complete_msg );
              /* To clear call objects */
              CNM_send_mn_rel_cnf(connection_id, 0,0,NULL);
              break;

         case CC_STATE_U7:
         case CC_STATE_U9:

              cause_length = (byte)(p_mncc_disc_req->data[1] + 2*sizeof(byte)); /* cause tag + length octet */

              if(cause_length >= sizeof(facility_invoke_component_data_T))
              {
                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
              }
              else if(p_mncc_disc_req->data[cause_length] == IEI_FACILITY)
              {

                /* Save cause in cc_transaction_info */
                if(cause_length > GSM_CAUSE_SIZE)
                {
                  cause_length = GSM_CAUSE_SIZE;
                }
                (void)memscpy(cc_transaction_information_p[connection_id]->cc_cause,
                             sizeof(cc_transaction_information_p[connection_id]->cc_cause),
                             p_mncc_disc_req->data,
                             cause_length ); /* save the cause iei */

                /* Message Type */
                p_data_req_msg->msg_head.type  = DISCONNECT;

                /* OTA Disconnect Req data */


                min_size = (int) MIN( sizeof(p_data_req_msg->data) , (sizeof(p_mncc_disc_req->data)-1));
                if( ( p_mncc_disc_req->message_header.message_len_lsb - sizeof(connection_id_T) - sizeof(sys_modem_as_id_e_type) ) > (byte)min_size)
                {
                     p_mncc_disc_req->message_header.message_len_lsb = min_size + sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type);
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid p_mncc_disc_req->message_header.message_len_lsb value, truncating\n");
                }
                (void)memscpy(&p_data_req_msg->data[0],
                              sizeof(p_data_req_msg->data),
                             &p_mncc_disc_req->data[1],  /* don't copy IEI tag octet per spec */
                              p_mncc_disc_req->message_header.message_len_lsb - sizeof(connection_id_T) - sizeof(sys_modem_as_id_e_type)); /* -  Cause IEI Tag */

                /* Set data length */
                PUT_IMH_LEN(sizeof(p_data_req_msg->msg_head.ti_pd) +
                            sizeof(p_data_req_msg->msg_head.type ) +
                            p_mncc_disc_req->message_header.message_len_lsb - sizeof(connection_id_T) - sizeof(sys_modem_as_id_e_type),
                            &p_data_req_msg->msg_head.IMH);

                /* Set the TI_PD */
                CNM_send_primitive_to_mm(connection_id, PD_CC,
                                         (IMH_T *)p_data_req_msg );

              }
              else
              {
                 min_size = (int) MIN( sizeof(cc_transaction_information_p[connection_id]->cc_cause),sizeof(p_mncc_disc_req->data));
                 if( ( p_mncc_disc_req->data[1] + 2 * sizeof(byte) ) > (byte)min_size)
                 {
                      p_mncc_disc_req->data[1] = min_size - ( 2 * sizeof(byte) );
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid p_mncc_disc_req->data[1] value, truncating\n");
                 }
                  
                (void)memscpy(cc_transaction_information_p[connection_id]->cc_cause,
                             sizeof(cc_transaction_information_p[connection_id]->cc_cause),
                             p_mncc_disc_req->data,
                             p_mncc_disc_req->data[1] + 2*sizeof(byte) );

                p_data_req_msg->msg_head.type  = DISCONNECT;

                min_size = (int) MIN(sizeof(p_data_req_msg->data) , (sizeof(p_mncc_disc_req->data)-1));

                if( ( p_mncc_disc_req->data[1] + 2 * sizeof(byte)-1 ) > (byte)min_size)
                {
                      p_mncc_disc_req->data[1] = min_size - ( 2 * sizeof(byte)-1 );
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid p_mncc_disc_req->data[1] value, truncating\n");

                }

                (void)memscpy(&p_data_req_msg->data[0],
                             sizeof(p_data_req_msg->data),
                             &p_mncc_disc_req->data[1],  /* don't copy IEI octet */
                             p_mncc_disc_req->data[1] + 2*sizeof(byte) - 1 );

                PUT_IMH_LEN(sizeof(p_data_req_msg->msg_head.ti_pd) +
                            sizeof(p_data_req_msg->msg_head.type ) +
                            p_mncc_disc_req->data[1] + 2*sizeof(byte) - 1,
                            &p_data_req_msg->msg_head.IMH );

                CNM_send_primitive_to_mm(connection_id, PD_CC,
                                         (IMH_T *)&p_data_req_msg->msg_head );

              }

              break;

          default:   /* Send a DISCONNECT msg in all other states  */
              {
                  word data_length;

                  GET_IMH_LEN(data_length, &p_mncc_disc_req->message_header)

                  data_length -= (sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type));

                  min_size = (int) MIN( sizeof(cc_transaction_information_p[connection_id]->cc_cause) , sizeof(p_mncc_disc_req->data));

                  if( ( p_mncc_disc_req->data[1] + 2 * sizeof(byte) ) > (byte)min_size)
                  {
                      p_mncc_disc_req->data[1] = min_size - ( 2 * sizeof(byte) );
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid p_mncc_disc_req->data[1] value, truncating\n");
                  }

                  /* store the cc cause */
                  (void)memscpy(cc_transaction_information_p[connection_id]->cc_cause,
                            sizeof(cc_transaction_information_p[connection_id]->cc_cause),
                            p_mncc_disc_req->data,
                            p_mncc_disc_req->data[1] + 2*sizeof(byte) );

                 /* build the message */

                  p_data_req_msg->msg_head.type  = DISCONNECT;

                  min_size = (int) MIN(sizeof(p_data_req_msg->data) , (sizeof(p_mncc_disc_req->data)-1));
                  if( ( data_length-1 ) > min_size)
                  {
                      data_length = min_size + 1 ;
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid data_length value, truncating\n");
                  }

                  (void)memscpy(&p_data_req_msg->data[0],
                                sizeof(p_data_req_msg->data),
                               &p_mncc_disc_req->data[1], 
                               (data_length-1));

                  PUT_IMH_LEN(data_length+1, &p_data_req_msg->msg_head.IMH );  /* type */

                  CNM_send_primitive_to_mm(connection_id, PD_CC,
                                        (IMH_T *)p_data_req_msg );
              }
              break;

      }  /* End of switch (cc_state)  */

      CNM_stop_timer(ALL_CC_TIMERS, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));

      CNM_cc_respond_to_event(connection_id, action );

      /* Accelerate the release of MM connection for CC call clearing w/ RR/RRC TCH/RAB released */
      if((cc_state > CC_STATE_U0_1) && 
         (cc_rab_released == TRUE) && 
         (mm_rel_req_sent == FALSE) )
      {
          mmcnm_rel_req_T    mmcnm_rel_req;
                      
          /* Send a release request to MM */
          mmcnm_rel_req.header.message_set = MS_CC_MM;
          mmcnm_rel_req.header.message_id  = MMCNM_REL_REQ;

          mmcnm_rel_req.rel_type = CNM_NORMAL_RELEASE;
    
#if defined(FEATURE_DUAL_SIM )
          if(MNCNM_IS_SINGLE_STACK)
          {
             if (mn_call_information_p[connection_id] != NULL)
             {
               mmcnm_rel_req.as_id = mn_call_information_p[connection_id]->as_id;
             }
             else if(mn_as_id != SYS_MODEM_AS_ID_NONE)
             {
               mmcnm_rel_req.as_id = mn_as_id;
             }
             else
             {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= call object and mn_as_id are null!");
             }
           }
#ifdef FEATURE_DUAL_ACTIVE
          else
          {
             mmcnm_rel_req.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
          }
#endif
          
          PUT_IMH_LEN(sizeof(mmcnm_rel_req_T) - sizeof(IMH_T),
                       &mmcnm_rel_req.header);

#else    
          PUT_IMH_LEN(sizeof(mmcnm_rel_req.rel_type), &mmcnm_rel_req.header);
#endif 
    
          CNM_send_message((IMH_T *)&mmcnm_rel_req, GS_QUEUE_MM);
    
          /* Start the CNM_MM_REL_TIMER 
           * Use default values for TI and PD 
           */
          CNM_start_timer(CNM_MM_REL_TIMER, 0, 0
#if defined(FEATURE_DUAL_SIM )
                         , mmcnm_rel_req.as_id //we want to give this as data to timer module in this timer
#endif
                          );
    
          /* Set the mm_rel_req_sent flag to true */
          mm_rel_req_sent = TRUE;

#if defined(FEATURE_DUAL_SIM )
          if(MNCNM_IS_SINGLE_STACK)
          {
            if( (CNM_check_if_any_service_object_exists() == FALSE) && 
                (mn_as_id != SYS_MODEM_AS_ID_NONE) )
            {
              MSG_HIGH_DS_0(MN_SUB,"=MNCNM= =MN= Reset the mn_as_id here\n");
              mn_as_id = SYS_MODEM_AS_ID_NONE;
            }
          }
#endif
      }

   }    /* END if action != NULL */

   CNM_free( p_data_req_msg );
   CNM_free( p_release_msg );
   CNM_free( p_release_complete_msg );
}
/**************** End of   CNM_process_mo_disc_request ****************************/

/**************** Begin of   CNM_process_mo_release_complete_request ***********************
 *
 *  Function name: CNM_process_mo_release_request()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle MN request to release of a call.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a release request and sends it to MM.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/
VOID_FUNC CNM_process_mo_release_complete_request( IMH_T * message )
{
  MNCC_REL_COMPL_REQ_T       *p_mncc_rel_complete;
  release_complete_msg_T     *p_release_complete;
  byte                        connection_id;  
#ifdef FEATURE_CCBS
  byte                        cause;
  cc_state_T                  cc_state;
#endif /* FEATURE_CCBS */

  p_mncc_rel_complete = (MNCC_REL_COMPL_REQ_T *)((void *)message);
  connection_id = p_mncc_rel_complete->connection_id;

#if defined(FEATURE_DUAL_SIM )
  if(MNCNM_IS_SINGLE_STACK)
  {
     if(mn_call_information_p[connection_id] == NULL)
     {
#ifdef FEATURE_MODEM_HEAP
        mn_call_information_p[connection_id] = (mn_call_information_T*)modem_mem_calloc(1, sizeof(mn_call_information_T),MODEM_MEM_CLIENT_NAS);
        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= ALLOC - mn_call_information_p\n");
#else
        mn_call_information_p[connection_id] = &mn_call_information[connection_id];
#endif
     }
     mn_call_information_p[connection_id]->as_id = p_mncc_rel_complete->as_id;
  }
#endif

#ifdef FEATURE_CCBS
  connection_id = p_mncc_rel_complete->connection_id;
  cause         = p_mncc_rel_complete->cause;
  if(cc_transaction_information_p[connection_id] != NULL)
  {
    cc_state  = cc_transaction_information_p[connection_id]->cc_state;
  }
  else
  {
     cc_state = CC_STATE_U0;
  }
  if (cc_state == CC_STATE_U0_6) /* user rejected recall */
  {
    CNM_send_release_complete( connection_id, PD_CC, cause, NULL );
    CNM_set_state_to_U0( connection_id );
  }
  else
  {
#endif /* FEATURE_CCBS */

    p_release_complete = (release_complete_msg_T*) CNM_alloc( sizeof( release_complete_msg_T ));

    if ( last_state != CC_STATE_U19 )
    {
      int len = 0;
      /*
       * send release complete message without a cause IE, as this is
       * not being sent to initiate a call clearing. Note that
       * this section of code cannot use the CNM_send_release_complete
       * message as there is no cause
       */
      p_release_complete->msg_head.type = RELEASE_COMPLETE;

/*
#ifdef FEATURE_DUAL_SIM
      p_release_complete->data[0] = p_mncc_rel_complete->as_id;
      PUT_IMH_LEN ( (int)((byte *)&p_release_complete->data[0] -
                    (byte *)&p_release_complete->msg_head.ti_pd)
                    + sizeof(byte),  
                    &p_release_complete->msg_head.IMH);

#else
*/
     len = NAS_STD_OFFSETOF(release_complete_msg_T, data[0]);
     len = len - NAS_STD_OFFSETOF(release_complete_msg_T, msg_head.ti_pd);

      PUT_IMH_LEN(
        len,
        &p_release_complete->msg_head.IMH
        );

/*#endif*/

      CNM_send_primitive_to_mm ( p_mncc_rel_complete->connection_id,
                                 PD_CC,
                                 (IMH_T *)p_release_complete);
    }

    CNM_free( p_release_complete );
#if defined(FEATURE_DUAL_SIM )
    if(MNCNM_IS_SINGLE_STACK)
    {
       if(mn_call_information_p[connection_id] != NULL)
       {
#ifdef FEATURE_MODEM_HEAP
         modem_mem_free(mn_call_information_p[connection_id], MODEM_MEM_CLIENT_NAS);
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= FREE - mn_call_information_p\n" );
#endif
         mn_call_information_p[connection_id] = NULL;
       }
    }
#endif

#ifdef FEATURE_CCBS
  }
#endif /* FEATURE_CCBS */

  /* If this is the last active connection, send a request to release 
   * the MM connection
   */

  (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE
#if defined(FEATURE_DUAL_SIM )
                                       , mn_as_id
#endif
                                      );
}
/**************** End of   CNM_process_mo_release_complete_request ****************************/


/**************** Begin of   CNM_process_mo_release_request ***********************
 *
 *  Function name: CNM_process_mo_release_request()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle MN request to release of a call.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a release request and sends it on.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mo_release_request( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_REL_REQ_T       *p_mncc_rel_req;
   release_msg_T        *p_data_req_msg;
   byte                 action;
   unsigned int         length;
   unsigned int         bytes_remaining;
   byte                 old_state;

   p_data_req_msg = (release_msg_T*) CNM_alloc( sizeof( release_msg_T ));

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_rel_req = (MNCC_REL_REQ_T *) ((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_rel_req->connection_id;

   action =
        CNM_cc_check_respond_to_event(connection_id, MNCC_REL_REQ_event );

   if ( action != 0 )
   {
      old_state = cc_transaction_information_p[connection_id]->cc_state;

      CNM_cc_respond_to_event(connection_id, action );

      p_data_req_msg->msg_head.type  = RELEASE;


      GET_IMH_LEN(length, &p_mncc_rel_req->message_header );

      bytes_remaining = length - sizeof(connection_id_T) - sizeof(sys_modem_as_id_e_type);

      /*
       * copy in cause IEs if present, but not if we were in state CC_STATE_U12
       * as then this RELEASE is in response to a DISCONNECT, so no cause
       * in message
       */

      if (( bytes_remaining != 0 ) && ( old_state != CC_STATE_U12 ))
      {
         bytes_remaining =  memscpy( p_data_req_msg->data,
                            sizeof(p_data_req_msg->data),
                            p_mncc_rel_req->data,
                            bytes_remaining );

      }
      else
      {
         if(bytes_remaining <= (byte)(p_mncc_rel_req->data[1] + 2))   /* IEI + LEN  = Additional IEI */
         {
           bytes_remaining =0;
         }
         else
         {
           bytes_remaining -= (p_mncc_rel_req->data[1] + 2);

           bytes_remaining = memscpy(p_data_req_msg->data,
                             sizeof(p_data_req_msg->data),
                             &p_mncc_rel_req->data[p_mncc_rel_req->data[1] + 2],
                             bytes_remaining );
         }
      }


      PUT_IMH_LEN(bytes_remaining +
                   sizeof(p_data_req_msg->msg_head.ti_pd) +
                   sizeof(p_data_req_msg->msg_head.type),
                   &p_data_req_msg->msg_head.IMH );


      CNM_send_primitive_to_mm(connection_id,
                              PD_CC,
                              (IMH_T *)p_data_req_msg
                                      );


   }

   CNM_free( p_data_req_msg );
}
/**************** End of   CNM_process_mo_release_request ****************************/


/**************** Begin of   CNM_process_mt_call_setup_completion *****************
 *
 *  Function name: CNM_process_mt_call_setup_completion()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle the message from MN indicating that a MT
 *  call has been connected by MN
 *  The function checks that the message can be handled in the current
 *  state, which handles timer etc, otherwise there is no action.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mt_call_setup_completion( IMH_T * message )
{
   connection_id_T         connection_id;
   byte                    action;
   MNCC_SETUP_COMPL_RES_T  *p_mncc_setup_compl_res;

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_setup_compl_res = (MNCC_SETUP_COMPL_RES_T *) ((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_setup_compl_res->connection_id;

   action = 
      CNM_cc_check_respond_to_event(connection_id, MNCC_SETUP_COMPL_RES_event );

   CNM_cc_respond_to_event(connection_id, action );


}
/**************** End of   CNM_process_mt_call_setup_completion ****************************/


/**************** Begin of   CNM_process_mo_notify_request ************************
 *
 *  Function name: CNM_process_mo_notify_request()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle initiation by the mobile MN of a notify
 *  procedure.
 *  This is not used so message ignored.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mo_notify_request( void )
{
    ERR("Unexpected MNCC_NOTIFY_REQ msg received\n", 0, 0, 0);

}
/**************** End of   CNM_process_mo_notify_request ****************************/

/**************** Begin of   CNM_process_SRVCC_HO_complete_indication ************************
 *
 *  Function name: CNM_process_SRVCC_HO_complete_indication()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to send SRVCC HO Indication Message to MN.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  connection id  : 
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_SRVCC_HO_complete_indication(IMH_T *CNM_input_primitive)
{
      mmcnm_srvcc_handover_complete_ind_T   *mmcnm_srvcc_ho_complete_ind = (mmcnm_srvcc_handover_complete_ind_T *)CNM_input_primitive;
      MNCC_SRVCC_HANDOVER_COMPLETE_IND_T     mncc_srvcc_ho_compl;

      mncc_srvcc_ho_compl.message_header.message_set  =  MS_MN_CC;
      mncc_srvcc_ho_compl.message_header.message_id  =  MNCC_SRVCC_HANDOVER_COMPLETE_IND;

      mncc_srvcc_ho_compl.active_rat = mmcnm_srvcc_ho_complete_ind->active_rat;
      mncc_srvcc_ho_compl.first_trigger = mmcnm_srvcc_ho_complete_ind->first_trigger;
	  
#if defined(FEATURE_DUAL_SIM)
      if(MNCNM_IS_SINGLE_STACK)
      {
        mn_as_id = mmcnm_srvcc_ho_complete_ind->as_id;
      }
#if defined(FEATURE_DUAL_ACTIVE)
      else
      {
        mncc_srvcc_ho_compl.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
#endif

      PUT_IMH_LEN( (sizeof(MNCC_SRVCC_HANDOVER_COMPLETE_IND_T) - sizeof(IMH_T)) ,
           &mncc_srvcc_ho_compl.message_header);

      CNM_send_message((IMH_T *)&mncc_srvcc_ho_compl, GS_QUEUE_MN_CM );

}
/**************** End of   CNM_process_mo_notify_request ****************************/



#ifdef FEATURE_UUS
/**************** Begin of   CNM_process_mo_user_user_data_request ****************
 *
 *  Function name: CNM_process_mo_user_user_data_request()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle a request from MN requesting the transfer of
 *  user-user data.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a user information request and sends it on.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mo_user_user_data_request( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_USER_USER_DATA_REQ_T  *p_mncc_user_data_req;
   user_information_msg_T     data_req_msg;
   byte                 action;
   unsigned int         length;
   unsigned int         bytes_remaining;

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_user_data_req = (MNCC_USER_USER_DATA_REQ_T *) ((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_user_data_req->connection_id;

   action =
        CNM_cc_check_respond_to_event(connection_id,
                                     MNCC_USER_USER_DATA_REQ_event );

   if ( action != 0 )
   {
      data_req_msg.msg_head.type  = USER_INFORMATION;

      GET_IMH_LEN(length, &p_mncc_user_data_req->message_header );

      bytes_remaining = length - sizeof(connection_id_T) - sizeof(sys_modem_as_id_e_type);

      if ( bytes_remaining != 0 )
      {
         bytes_remaining--;   /* allow for loss of user_user_data IEI */

         CNM_cc_respond_to_event(connection_id, action );

         bytes_remaining =  memscpy( &data_req_msg.data[0],
                            sizeof(data_req_msg.data),
                            &p_mncc_user_data_req->data[1],
                            bytes_remaining );

         PUT_IMH_LEN(bytes_remaining +
                      sizeof(data_req_msg.msg_head.ti_pd) +
                      sizeof(data_req_msg.msg_head.type),
                      &data_req_msg.msg_head.IMH );


         CNM_send_primitive_to_mm(connection_id,
                                 PD_CC,
                                 (IMH_T *)&data_req_msg
                                         );

      }
      else
      {
         ERR("Incomplete MO user_user_data_req msg: remaining bytes = %d\n",
              (dword) bytes_remaining, 0, 0);
      }

   }

}

/**************** End of   CNM_process_mo_user_user_data_request *******************/
#endif /* FEATURE_UUS */

#ifdef FEATURE_CCBS
/**************** Begin of  CNM_process_cc_establishment_message ***********
 *
 *  Function name: CNM_process_cc_establishment_message()
 *  ----------------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles CC-ESTABLISHMENT message
 *  received from the network. It walks
 *  through the IEs in the message checking for validit.
 *  if all not ok, a status message is sent to NW.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_cc_establishment_message( connection_id_T connection_id, boolean syntax_ok)
{
  cc_establishment_confirmed_msg_T  cc_est_conf;
  MNCC_CC_EST_IND_T                 cc_est_ind;
  byte                              action;
  byte                             *p_IE=NULL;
  byte                              IEI, IE_len, IE_count=2;  /* start at first IE after message type */

  memset (&cc_est_conf, 0x00, sizeof(cc_establishment_confirmed_msg_T));
  memset (&cc_est_ind,  0x00, sizeof(MNCC_CC_EST_IND_T));

  while ( syntax_ok && IE_count<no_of_IEs )
  {
    /* get information of the next IE from global store */
    IEI    = cc_IE_info[IE_count].IEI;
    p_IE   = cc_IE_info[IE_count].start;  /* points to dummy T in TLV */
    IE_len = cc_IE_info[IE_count].length; /* = 1(T)+1(L)+V */

    if(cc_IE_info[IE_count].syntax_ok == TRUE)
    {
      switch (IEI)
      {
        case IEI_SETUP_CONTAINER: /* save setup container */
          {
            cc_est_ind.message_header.message_set = MS_MN_CC;
            cc_est_ind.message_header.message_id  = MNCC_CC_EST_IND;
            cc_est_ind.connection_id              = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
            {
              cc_est_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
            }
#endif



            if(IE_len -1 > sizeof(cc_est_ind.data))
            {
            MSG_HIGH_DS_2(MN_SUB,"=MNCNM=  Truncating IEI SETUP CONTAINER length %d to size of data buffer %d",IE_len -1,sizeof(cc_est_ind.data));
            IE_len = sizeof(cc_est_ind.data)+1;
            *(p_IE + 1) = IE_len -2;
            }
            (void)memscpy(cc_est_ind.data, sizeof(cc_est_ind.data) ,p_IE+1, IE_len -1);   /* we don't want T */

            PUT_IMH_LEN((IE_len -1) + sizeof(MNCC_CC_EST_IND_T) - sizeof(IMH_T) - sizeof(ie_setup_container_T), &cc_est_ind.message_header);

            CNM_send_message( (IMH_T *)&cc_est_ind, GS_QUEUE_MN_CM );
          }
          break;

        default:                            /* this cannot happen! */
          ERR("[ccbs]Unexpected IE in cc_est: IEI = %d\n",(dword)IEI,0,0);
          syntax_ok = FALSE;
          break;
      }
    }

    IE_count++; /* index next IE in the message */
  }

  if ( syntax_ok )
  {
    action = CNM_cc_check_respond_to_event(connection_id, MNCC_CC_EST_CONF_REQ_event );

    if (action != 0/*A_BAD_EVENT*/)
    {
      CNM_cc_respond_to_event(connection_id, action );

      /* send CC-ESTABLISHMENT CONFIRMED message to NW */
      cc_est_conf.msg_head.type = CC_ESTABLISHMENT_CONFIRMED;

      /* for now no IEs */

      PUT_IMH_LEN( sizeof(cc_est_conf.msg_head.ti_pd) + sizeof(cc_est_conf.msg_head.type),
                   &cc_est_conf.msg_head.IMH );

      CNM_send_primitive_to_mm( connection_id, PD_CC, &cc_est_conf.msg_head.IMH );
    }
    else
    {
      if(cc_transaction_information_p[connection_id] != NULL)
      {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= [ccbs]CC ESTABLISHMENT rcvd in state %d",
                    cc_transaction_information_p[connection_id]->cc_state);
      }
    }
  }
  else
  {
    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= [ccbs]syntax error CC ESTABLISHMENT");
    CNM_send_status (connection_id, PD_CC, INVALID_MANDATORY_INFORMATION);
  }
}

/**************** Begin of  CNM_process_recall_message ***********
 *
 *  Function name: CNM_process_recall_message()
 *  ----------------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles RECALL message
 *  received from the network. It walks
 *  through the IEs in the message checking for validit.
 *  if all not ok, a status message is sent to NW.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_recall_message( connection_id_T connection_id, boolean syntax_ok)
{
  byte                 *p_IE=NULL;
  byte                  IEI, IE_count=2/*IE after type*/;
  IE_info_T            *pIEinfo;

  MNCC_RECALL_IND_T     recall;

  /* we're already in CC_STATE_U0.6 */
  memset (&recall,   0x00, sizeof(MNCC_RECALL_IND_T));

  while ( syntax_ok && (IE_count<no_of_IEs) )
  {
    pIEinfo = &cc_IE_info[IE_count];

    /* get information of the next IE from message */
    IEI       = pIEinfo->IEI;
    p_IE      = pIEinfo->start;

    if(cc_IE_info[IE_count].syntax_ok == TRUE)
    {
      switch (IEI)
      {
          case IEI_FACILITY:
            break;

          case IEI_RECALL_TYPE:  /* type 3 V */
            {
              recall.data[0] = p_IE[1]; /* byte after IEI is value */
            }
            break;

          default:
            MSG_ERROR_DS_1(MN_SUB,"=MNCNM= [ccbs]Unexpected IE in recall msg: IEI=%d\n",IEI);
            syntax_ok = FALSE;
            break;
      }
    }
    IE_count++;       /* index next IE in the message */
  }

  if (syntax_ok)
  {
    recall.message_header.message_set = MS_MN_CC;
    recall.message_header.message_id  = MNCC_RECALL_IND;
    recall.connection_id              = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
    {
      recall.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
    }
#endif


    PUT_IMH_LEN( sizeof(MNCC_RECALL_IND_T) - sizeof(IMH_T), &recall.message_header);

    CNM_send_message( &recall.message_header, GS_QUEUE_MN_CM );
  }
  else
  {
    /* there is no optional/conditional IE in RECALL message */
    CNM_send_status (connection_id, PD_CC, INVALID_MANDATORY_INFORMATION);
  }
}
/**************** End of   CNM_process_recall_message ******************/
#endif /* FEATURE_CCBS */


/**************** Begin of   CNM_process_srvcc_handover_fail_req *************************
 *
 *  Function name: CNM_process_srvcc_handover_fail_req()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle signalling connection release  when no calls gets tranfered during SRVCC handover
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_srvcc_handover_fail_req(IMH_T * message)
{

   (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE
#if defined(FEATURE_DUAL_SIM ) 
                                      , mn_as_id
#endif
                                      );
}

/**************** End of   CNM_process_srvcc_handover_fail_req *************************/

