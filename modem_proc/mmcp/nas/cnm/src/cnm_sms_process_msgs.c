/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2011, 2002-2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_sms_process_msgs.c_v   1.17   17 Jun 2002 16:03:00   rgurumoo  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_sms_process_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/11    AM     Adding range check on rpdu_len to avoid buffer overflow
7/16/01     CD     Fixed data overrun in CNM_process_received_cp_message

7/17/01     CD     Added log message when mo retransmit counter changes value
                   Included msg.h

08/07/01   CD      Clean-up of include header files

08/13/01   CD      Added check for sms_state = MM_CONNECT_PENDING in
                   CNM_process_abort_request

08/14/01   AB      Renamed connection manager references to CNM from CM.

8/31/01    CD      Corrected ADS110 compiler warnings

9/17/01    CD      Added check for MMCNM_EST_CNF, MMCC_REEST_CNF, MMCNM_ABORT_IND
                   MMCNM_ERROR_IND in CNM_handle_short_message_services

                   Added processing for MMCC_REEST_CNF, MMCNM_ERROR_IND,
                   MNCNM_ABORT_IND in CNM_SMS_process_SMS()

27-Sep-2001  VT    For GSM: In CNM_process_mt_sms(), in the call to rex_timed_wait(),
                     changed the second param from mn_cnm_sleep_timer to
                     &mn_cnm_sleep_timer. Also, added
                     extern  rex_timer_type mn_cnm_sleep_timer;
                     at the top of this file.
                   Removed extern  rex_timer_type mn_cnm_sleep_timer; from inside
                     CNM_process_mt_sms().

9/28/01    CD     Removed extern definition of mn_cnm_sleep_timer from this file.

10/01/01   CD     Deleted check for state = SMS_MM_CONN_PEND for message_id
                  MNCNM_ERROR_IND in CNM_SMS_process_SMS(), beforet he state
                  machine is called.  The state machine handles all states

11/01/01   CD     In CNM_handle_short_message_services(), fixed case where a error in a
                  received CP-DATA did not results in a release request

                   Fixed lint errors

                   Replaced all instances of %l by %d

01/29/02   RG     Fixes to handle the multiple SMS transactions properly.

04/19/02   CD     In CNM_SMS_process_SMS(), changed how the rel_cause in a
                  MMCNM_REL_IND is processed due to the new MM-CNM interface

                  Replaced MMCC_REEST_CNF by MMCC_REEST_REJ, since
                  CNM_handle_short_message_services() used to be called only when a
                  MMCC_REEST_CNF was received with a reestablish_success flag set
                  to FALSE

                  Deleted processing of MMCNM_ABORT_IND.  It's now part of a
                  MMCNM_REL_IND w/rel_cause set to ABORT_MSG_RECEIVED

                  Deleted processing of MMCNM_ERR_IND.  It's now part of a
                  MMCNM_REL_IND w/rel_cause set to OTHER_CAUSE

                  Modified processing of MMCNM_REL_IND in CNM_SMS_process_SMS()
                  based on the reest_possible flag

                  Added processing for MMCNM_EST_REJ

                  In CNM_SMS_process_SMS(), changes how the connection_id is
                  assigned

                  In CNM_process_mt_sms(), merged processing of MMCNM_EST_IND
                  into MMCNM_DATA_IND

                  Changed argument in CNM_SMS_process_message_type()

                  Updated Copyright

                  Removed Lint errors

04/30/02          Corrected history comments from 04/19/02 about processing of
                  MMCNM_REL_IND

05/07/02   CD     Changed release type enum due to name collision with RR

05/15/02   CD     Correcting processing of MMCNM_EST_REJ to clear the
                  mm_connection_pending.CNM_service_type

05/23/02   RG     In the CNM_process_mo_sms_request() routine, set
                  CNM_mo_cp_data_retransmit_counter to CP_DATA_RETRANSMIT_COUNT
                  instead of CP_DATA_RETRANSMIT_COUNT-1 as the CP-DATA for MO
                  SM has to be retransmitted CP_DATA_RETRANSMIT_COUNT times.
                  In the CNM_process_received_cp_message() routine, removed call
                  to the CNM_check_for_pending_message() routine. (The
                  CNM_check_for_pending_message() routine should be called
                  only once during the incoming CP-DATA(RP-ACK) event processing.
                  It is called by the sms state machine in the CNM_sms_respond_to_event()
                  routine)

07/29/02   CD     Modified processing of  MMCC_REEST_REJ and MMCNM_REL_IND for
                  call reestablishment scenarios

09/17/02   RG     Fixed CR 24448: Send CP_ERROR when an MT SMS CP-DATA is received
                  from the network with invalid transaction identifier.

02/10/03   AB     Reset the prefer domain, pref_rl_domain, in
                  CNM_process_sms_release_message(), when SMS transaction is released.

07/23/04   AB     Fixed CR46760, CP_DATA send on invalid domain on T1CM_MT Timer expired.

01/20/05   AB     Added valid MNCNM_REL_IND primitives to SMS messages
                     handler, CNM_SMS_process_SMS().

09/01/05   HS     CNM_emulate_sms_TC1M_MO_timeout() introduced to cut SMS TC1M timer short
                  in G2G HO while waiting CP_ACK

07/14/06    NR    Adding changes for SMS optimization CR 93099 , removing redundant variables
                  global_flag_cpdata_arriving & global_flag_ccrelease_arriving

17/01/07   RD     Fixed CR 109163, a security vulnerability

04/09/07   RD     SMS optimization changes are incorporated

07/14/11   PM     Fix for CR 294952. NAS sends error report to WMS for thrashed SMS.
                  Also, when CNM receives MMCNM_REL_IND from MM, MMCNM_EST_REQ for next SMS
                  is not sent to MM. See other change in cnm_sms_state_machine.c

09/27/11   PM      Fix for CR 308048. Sending CP-ERROR, when network sends CP-DATA
                   in UE state Wait_for_CP_ACK. When UE receives CP_ERROR in WAIT for CP_ACK state
                   it is considered as CP_ACK_FALSE event, rather than CP_ERROR event. In this scenario,
                   CP_ERROR to NW is not triggered.

10/10/11   PM      Need to store AS_ID for SMS tranactions well before checking for protocol errors
                   as that AS_ID would be used to send CP_ERROR if protocol is not followed by NW.

11/16/11   PM      Reset CNM state for SMS before sending MNSMS_ERROR_IND to MN from CNM.          
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "com_iei_v.h"
#include "ms.h"
#include "mn.h"
#include "mnglobal.h"
#include "mn_cnmtask.h"
#include "mn_cnmtask_int.h"

#include "cmlib.h"
#include "cnm.h"
#include "cause.h"

#include "nasutils.h"
#include "cm_sms.h"
#include "msg.h"
#include "mm.h"
#include <stringl/stringl.h>
#define MM_AS_ID_1 1

/*
 * global data declaration
 */
extern boolean is_cs_umts_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_gsm_mode( sys_modem_as_id_e_type as_id );

extern boolean is_cs_lte_mode( sys_modem_as_id_e_type as_id ); 

/**************** Begin of   CNM_handle_short_message_services ********************
 *
 *  Function name: CNM_handle_short_message_services ()
 *  --------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function routes all sms input primitives to appropriate
 *  functions which deal with them. If the message came from MM, then
 *  additional checking of the message is performed.
 *
 *  Parameters:
 *  -----------
 *  CNM_input_primitive : IN - input message
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_handle_short_message_services ( IMH_T * CNM_input_primitive )
{
   boolean          message_ok;
   mmcnm_data_ind_T *p_mmcnm_data_ind;

#if defined(FEATURE_DUAL_SIM )
   unsigned int     length;
   connection_id_T connection_id = UNKNOWN_CONNECTION_ID;
#endif

   message_ok = TRUE;            /* default to no errors */
   p_mmcnm_data_ind = (mmcnm_data_ind_T *) ((void *)CNM_input_primitive);

   if  (CNM_input_primitive->message_id == MMCNM_DATA_IND)
   {

#if defined(FEATURE_DUAL_SIM )
      /*
       * Setting of mn_as_id here is required because if SMS message is not according to
       * protocol then UE needs to send CP_ERROR to NW. While sending Data Req, CNM requires
       * the as_id on which ERROR needs to be sent. But, in erroneous scenarios, AS_ID won't
       * get set later leading to a crash in MM.
       * To avoid that crash, CNM needs to store as_id.
       */

      GET_IMH_LEN (length, &(p_mmcnm_data_ind->msg_head.IMH));
      connection_id = CNM_get_connection_id( p_mmcnm_data_ind ) ;
      if((connection_id < MAXNO_SMS_TRANSACTIONS) && 
         (sm_rl_transaction_information[connection_id].as_id == SYS_MODEM_AS_ID_NONE))
      {
        if(MNCNM_IS_SINGLE_STACK)
        {
          sm_rl_transaction_information[connection_id].as_id = (sys_modem_as_id_e_type)p_mmcnm_data_ind->data[length -3];
        }
      }
#endif
       
      message_ok = CNM_SMS_process_message_type( p_mmcnm_data_ind );

      if ( message_ok == TRUE )
      {
         message_ok = CNM_check_for_protocol_errors( p_mmcnm_data_ind );

      }

      if ( message_ok == FALSE )
      {
            /*
             * TS 24.011, 5.3.2.1
             * If CP-DATA cannot be accepted, send CP_ERROR (already sent) and a release
             * request (sent below)
             */
             (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE
#if defined(FEATURE_DUAL_SIM )
                                               , mn_as_id
#endif
                                                 );
      }
   }

   if ( message_ok == TRUE )
   {
       CNM_SMS_process_SMS( CNM_input_primitive );
   }

}
/**************** End of   CNM_handle_short_message_services ***********************************/


/**************** Begin of   CNM_SMS_process_message_type *************************
 *
 *  Function name: CNM_SMS_process_message_type ()
 *  ---------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function checks for a valid message type in the SMS MMCNM_DATA_IND message
 *  received from the network
 *
 *  Parameters:
 *  -----------
 *  CNM_input_primitive : IN - input message
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

boolean CNM_SMS_process_message_type ( mmcnm_data_ind_T * SMS_input_primitive )
{
   boolean                 return_value;
   connection_id_T         connection_id = 0;
   int                     length;
   struct MsgDetailTag    *p_message_details;
   mmcnm_data_ind_T        *p_mmcnm_data_ind;

   return_value = FALSE;


   if(SMS_input_primitive != NULL)
   {
       p_mmcnm_data_ind = (mmcnm_data_ind_T *) SMS_input_primitive;

       connection_id = CNM_get_connection_id( p_mmcnm_data_ind ) ;

       /* set domain: CS/PS */
       if(connection_id < MAXNO_SMS_TRANSACTIONS)
       {
          if(p_mmcnm_data_ind->msg_head.IMH.message_set == MS_CC_MM_PS)
          {
             sms_transaction_information[connection_id].pref_rl_domain = MS_CC_MM_PS;
          }
          else
          {
             sms_transaction_information[connection_id].pref_rl_domain = MS_CC_MM;
          }
       }
       else
       {
           /*
            *  3GPP TS 24.011, Section 9.2.2
            *  The Mobile Station shall ignore a CP message (CP-DATA, CP-ACK, CP-ERROR) received with TI value
            *  "111".
            */

           MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid TI = %d", connection_id);
           return FALSE; /* Ignoring the message here */
       }

       GET_IMH_LEN( length, &SMS_input_primitive->msg_head.IMH )

       if ( length >= 2 )      /* there is a message type field in the message */
       {
          /*
           * set up pointers for determining the allowed ies in the message
           */
          p_message_details =
             CNM_setup_IE_data_read( p_mmcnm_data_ind->msg_head.type, MT_CALL);

          /*
           * if return value is null, something wrong with message
           */

          if (p_message_details  != (struct MsgDetailTag *) 0 )
          {
             if ( (connection_id & 0x07) != 7 )  /* disallowed id */
             {
                switch ( p_mmcnm_data_ind->msg_head.type )
                {
                   case CP_DATA:
                   case CP_ACK:
                   case CP_ERROR:
                   return_value = TRUE;
                   break;

                   default:
                   CNM_send_cp_error( connection_id, MESSAGE_TYPE_NON_EXISTENT );

                   MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected SMS message type: received message_type = %d\n",
                             p_mmcnm_data_ind->msg_head.type);
                  break;
                }
             }
             else
             {
                // CR 24448
                CNM_send_cp_error( connection_id, CP_CAUSE_INVALID_TI );

                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Connection id should not be 7: received connection_id = %d\n",
                            connection_id);
             }
          }
          else
          {
               if((p_mmcnm_data_ind->msg_head.type != CP_DATA) &&
                  (p_mmcnm_data_ind->msg_head.type != CP_ACK) &&
                  (p_mmcnm_data_ind->msg_head.type != CP_ERROR))
               {
                    CNM_send_cp_error( connection_id,  MESSAGE_TYPE_NON_EXISTENT );  /* SMS message type */
               }
               else
               {
                    CNM_send_cp_error( connection_id,  CP_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE );  /* other mandatory IE */
               }

               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Syntax error in SMS message \n");
          }
       }
       else
       {
           CNM_send_cp_error( connection_id,  CP_CAUSE_INVALID_MANDATORY_INFORMATION );

           MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Received SMS message is too small: message length = %d\n", length);
       }
   }
   else
   {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Revieved a NULL ptr for SMS_input_primitive");
   }

   return return_value;
}

/**************** End of   CNM_SMS_process_message_type ***********************************/


/**************** Begin of   CNM_SMS_process_SMS **********************************
 *
 *  Function name: CNM_SMS_process_SMS ()
 *  ------------------------------------
 *
 *  Description:
 *  ------------
 *  This function determines if the message is part of a MO or MT transaction
 *  and routes it accordingly
 *
 *  Parameters:
 *  -----------
 *  CNM_input_primitive : IN - input message
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_SMS_process_SMS ( IMH_T * SMS_input_primitive )
{
   connection_id_T         connection_id = 0;
   boolean                 valid_connection_id;
   byte                    cause;
   mmcnm_rel_ind_T         *p_mmcnm_rel_ind;
   cp_data_msg_T           mmcnm_data_req;

   valid_connection_id = TRUE;
   connection_id = UNKNOWN_CONNECTION_ID;

   /*
   * Find the connection id associated with the SMS input primitive.
   * Note that MM REL indications and REST CNF are
   * associated with all active connection ids and therefore processed
   * differently
   */

   switch(SMS_input_primitive->message_set)
   {
   case MS_CC_MM:
   case MS_CC_MM_PS:
       {
           switch (SMS_input_primitive->message_id)
           {
           case MMCNM_EST_CNF:
           case MMCNM_EST_REJ:
               connection_id = mm_connection_pending.connection_id;
               sms_transaction_information[connection_id].pref_rl_domain =
                                                      SMS_input_primitive->message_set;
               break;

           case MMCNM_DATA_IND:
               /* All other message ids */
               connection_id = CNM_get_connection_id((mmcnm_data_ind_T *) ((void *)SMS_input_primitive));
               if(connection_id >= MAXNO_SMS_TRANSACTIONS)
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds buffer overflow");
               }
               else
               {
                  sms_transaction_information[connection_id].pref_rl_domain =
                                                        SMS_input_primitive->message_set;
               }
               break;

           case MMCNM_REL_IND:
                MSG_LOW_DS_0(MN_SUB,"=MNCNM= Received MMCNM_REL_IND from MM");
               break;

           default:
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected SMS message id = %d received from MM", SMS_input_primitive->message_id);
               break;
           }
       }

       break;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   case MS_TIMER:
       connection_id = (unsigned char) (GET_TI(((timer_expiry_T *)((void *)SMS_input_primitive))->data));
       break;
#endif /*defined(FEATURE_WCDMA) || defined(FEATURE_GSM)*/

   case MS_MN_SMS:
       connection_id = ((MNSMS_DATA_REQ_T *)((void *)SMS_input_primitive ))->connection_id;
       break;

   default:
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= CNM SMS received unexpected msg set %d",
                  SMS_input_primitive->message_set);

       valid_connection_id = FALSE;
       break;
   }

   /*
   * Process a MMCC_REEST_REJ and set the valid_connection_id flag to false
   * since it does not apply.
   */

   if (SMS_input_primitive->message_id == MMCC_REEST_REJ)
   {
      for ( connection_id = 0;
            connection_id < MAXNO_SMS_TRANSACTIONS;
            connection_id++)
      {
          CNM_sms_respond_to_event(MMSMS_REEST_REJ_event,
              sms_transaction_information[connection_id].sms_state,
              connection_id);
      }
      valid_connection_id = FALSE;
   }

   /*
   * Process a MMCNM_EST_REJ and set the valid_connection_id flag to false
   * since it does not apply.
   */

   if ((connection_id < MAXNO_SMS_TRANSACTIONS) &&
       (SMS_input_primitive->message_id == MMCNM_EST_REJ))
   {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
       mmcnm_est_rej_T *mmcnm_est_rej;
#endif
       mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;

       CNM_sms_respond_to_event(MMSMS_EST_REJ_event,
                                sms_transaction_information[connection_id].sms_state,
                                connection_id);
#if defined(FEATURE_DUAL_SIM )
       if((MNCNM_IS_SINGLE_STACK) && ((mmcnm_est_rej_T *)SMS_input_primitive)->cause_value == AS_REJ_ABORT_RADIO_UNAVAILABLE)
       {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
         mmcnm_est_rej = (mmcnm_est_rej_T *) ((void *)SMS_input_primitive);
         CNM_send_MNSMS_ERROR_IND( connection_id, (byte)0, AS_REJ_ABORT_RADIO_UNAVAILABLE, mmcnm_est_rej->cause_value, mmcnm_est_rej->domain);
#else
         CNM_send_MNSMS_ERROR_IND( connection_id, (byte)0, AS_REJ_ABORT_RADIO_UNAVAILABLE);
#endif
       }
       else
#endif
       {
         if(((mmcnm_est_rej_T *)SMS_input_primitive)->cause_value  == EMM_REJ_TEMP_STATE)
         {
           
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
           mmcnm_est_rej = (mmcnm_est_rej_T *) ((void *)SMS_input_primitive);
           CNM_send_MNSMS_ERROR_IND( connection_id, (byte)0, EMM_REJ_TEMP_STATE, mmcnm_est_rej->cause_value, mmcnm_est_rej->domain);
#else
           CNM_send_MNSMS_ERROR_IND( connection_id, (byte)0, EMM_REJ_TEMP_STATE);
#endif
         }

         else
         {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mmcnm_est_rej = (mmcnm_est_rej_T *) ((void *)SMS_input_primitive);
            CNM_send_MNSMS_ERROR_IND( connection_id, (byte)0, NORMAL_UNSPECIFIED, mmcnm_est_rej->cause_value, mmcnm_est_rej->domain);
#else
            CNM_send_MNSMS_ERROR_IND( connection_id, (byte)0, NORMAL_UNSPECIFIED);
#endif
         }
       }
       valid_connection_id = FALSE;
   }

   /*
   * Process a MMCNM_REL_IND and set the valid_connection_id flag to false
   * since it does not apply.
   */

   if (SMS_input_primitive->message_id == MMCNM_REL_IND)
   {
      boolean ps_sms_present = FALSE;         /*this identifier is declared to fix CR 104321*/
#if defined(FEATURE_DUAL_SIM )
      boolean reset_mn_as_id = FALSE; 
      if(MNCNM_IS_SINGLE_STACK)
      {
        if(mn_as_id == SYS_MODEM_AS_ID_NONE)
        {
          mn_as_id = ((mmcnm_rel_ind_T *) ((void *)SMS_input_primitive))->as_id;
          reset_mn_as_id = TRUE;
        }
      }
#endif
      p_mmcnm_rel_ind = (mmcnm_rel_ind_T *) ((void *)SMS_input_primitive);
      cause = p_mmcnm_rel_ind->rel_cause;

      if (SMS_input_primitive->message_set != MS_CC_MM_PS )
      {
        for ( connection_id = 0;
              connection_id < MAXNO_SMS_TRANSACTIONS;
              connection_id++)
        {
          if(sms_transaction_information[connection_id].sms_state != SMS_IDLE)
          {
              /*if (p_mmcnm_rel_ind->reest_possible == TRUE)
              {
                  event = MMSMS_REL_IND_event;
              }
              else
              {
                  event = MMCNM_ERR_IND_event;
              }*/
             if (sms_transaction_information[connection_id].pref_rl_domain != MS_CC_MM_PS)                 
             {
                CNM_sms_respond_to_event(MMSMS_REL_IND_event,
                  sms_transaction_information[connection_id].sms_state,
                  connection_id);

                CNM_send_error_indication_to_rp_layer( connection_id, cause);
                /*The domain and type info must be got from REL_IND*/
             }
             else
             {
                ps_sms_present = TRUE;
             }             
          }
        }
      }
      
      if (SMS_input_primitive->message_set == MS_CC_MM_PS )
      {
        for ( connection_id = 0;
              connection_id < MAXNO_SMS_TRANSACTIONS;
              connection_id++)
        {
          if ((sms_transaction_information[connection_id].sms_state != SMS_IDLE) && 
              (sms_transaction_information[connection_id].pref_rl_domain == MS_CC_MM_PS))
          {
                CNM_sms_respond_to_event(MMSMS_REL_IND_event,
                  sms_transaction_information[connection_id].sms_state,
                  connection_id);

                CNM_send_error_indication_to_rp_layer( connection_id, cause);
          }
        }
      }

      /* Fix for CR 104321
       * The code is changed to have the SMS segments in the queue in case there is
       * any PS SMS pending. If there is no PS SMS pending, then the queue is flushed.
       */       
      if ((ps_sms_present != TRUE) ||(SMS_input_primitive->message_set == MS_CC_MM_PS)) 
      {
         /* Fix for CR26456
           * The CNM service request queue is emptied by the CC processing of the
           * MMCNM_REL_IND.
           * Here, remove all the messages in the sms_request_queue. Then loop through the
           * sms_transaction_information table to transition the transactions to IDLE.
         */
         FOREVER
         {
           if ( CNM_read_sms_request_queue(  &mmcnm_data_req, &connection_id ) == FALSE )
           {
             break;
           }
           else
           {
                CNM_sms_respond_to_event(MMSMS_REL_IND_event,
                  sms_transaction_information[connection_id].sms_state,
                  connection_id);
           
                CNM_send_error_indication_to_rp_layer( connection_id, cause);
           
           }
       
         }
      }
      valid_connection_id = FALSE;
#if defined(FEATURE_DUAL_SIM )
      if(reset_mn_as_id == TRUE)
      {
        mn_as_id = SYS_MODEM_AS_ID_NONE;
      }
#endif
   }

   /*
   * Process all other messages with a valid_connection_id flag set to TRUE
   */
   if ( valid_connection_id != FALSE )
   {
      if ( connection_id < 0x08 )
      {
         CNM_process_mo_sms( SMS_input_primitive, connection_id);
      }
      else
      {
         CNM_process_mt_sms( SMS_input_primitive, connection_id);
      }
   }

   if(sms_transaction_information[connection_id].sms_state == SMS_IDLE)
   {
     cp_cause_value = 0;
   }

}
/**************** End of   CNM_SMS_process_SMS ***********************************/


/**************** Begin of   CNM_process_mt_sms ***********************************
 *
 *  Function name: CNM_SMS_process_mt_sms ()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *  This function calls other functions to process received messages, and
 *  performs other actions depending on the state of the transaction.
 *
 *
 *  Parameters:
 *  -----------
 *  message: IN - input message
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mt_sms ( IMH_T * message, connection_id_T connection_id )
{
   sms_state_T             current_state;
   byte                    trigger;
   boolean                 valid_msg = TRUE;
   boolean                 est_ind = FALSE;

   trigger = NO_ACTION_event;
   if (connection_id < MAXNO_SMS_TRANSACTIONS)
   {
      current_state = sms_transaction_information[connection_id].sms_state;

      switch(message->message_set)
      {
      case MS_CC_MM:
      case MS_CC_MM_PS:
          {
              if (message->message_id  == MMCNM_DATA_IND)
              {
                 if (NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS != mt_sms_transport_type)
                 {
                    est_ind = CNM_check_for_establish_ind((mmcnm_data_ind_T *)((void *)message));
                    if (est_ind == TRUE)
                    {
                       boolean is_G_RAT = FALSE;
                       boolean is_W_RAT = FALSE;
                       boolean is_L_RAT = FALSE;
   #ifdef FEATURE_DUAL_SIM
                       if(MNCNM_IS_SINGLE_STACK)
                       {
                         is_G_RAT = is_cs_gsm_mode(mn_as_id);
                         is_W_RAT = is_cs_umts_mode(mn_as_id);
                         is_L_RAT = is_cs_lte_mode(mn_as_id);
                       }
   #ifdef FEATURE_DUAL_ACTIVE
                       else
                       {
                         is_G_RAT = is_cs_gsm_mode((sys_modem_as_id_e_type)mn_dsda_as_id);
                         is_W_RAT = is_cs_umts_mode((sys_modem_as_id_e_type)mn_dsda_as_id);
                         is_L_RAT = is_cs_lte_mode((sys_modem_as_id_e_type)mn_dsda_as_id);
                       }
   
   #endif
   #else
                       {
                         is_G_RAT = is_cs_gsm_mode(MM_AS_ID_1);
                         is_W_RAT = is_cs_umts_mode(MM_AS_ID_1);                      
                         is_L_RAT = is_cs_lte_mode(MM_AS_ID_1);                      
                       }
   #endif
                       if(is_G_RAT == TRUE)
                       {
   #ifdef FEATURE_GSM
   
                          trigger = CNM_process_received_short_message( message, connection_id);
   
   #endif  /* ifdef FEATURE_GSM */
                       } /* if is_gsm_mode */
                       else if(is_W_RAT == TRUE)
                       {
                          trigger = CNM_process_received_short_message( message, connection_id);
   
                       }
   #ifdef FEATURE_LTE
                       if ((is_L_RAT == TRUE) && ((unsigned char)(GET_PD(((mmcnm_data_ind_T *)((void *)message))->msg_head.ti_pd)) == PD_SMS))
                       {
                          trigger = CNM_process_received_short_message( message, connection_id);
                       }
   #endif
                    }
                    else /* est_ind = FALSE */
                    {
                       trigger = CNM_process_received_cp_message( message, connection_id);
                    }

                    if (S_MT_SMS_REQUEST_event == trigger)
                    {
                       mt_sms_transport_type = NAS_MSG_TRANSPORT_TYPE_GW;
                    }
                 }
                 else
                 {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Ongoing MT SMS on IMS, CP_ERR for this message");
                    /*
                    * Section 3.2, Spec 24.011 - Parallel message transfer in same direction is prohibited.
                    */
                    CNM_send_cp_error(connection_id, CP_CAUSE_MSG_NOT_COMPATIBLE_WITH_SMS_STATE);
                    valid_msg = FALSE;
                 }
              }
              else
              {
                 valid_msg = FALSE;
              }
              break;
          }
      case MS_MN_SMS:
          {
              if (message->message_id  == MNSMS_DATA_REQ)
              {
                 trigger = CNM_send_rp_message( message, connection_id);
              }
              else if (message->message_id  == MNSMS_ABORT_REQ)
              {
                 trigger = CNM_process_abort_request(connection_id);
              }
              else if (message->message_id  == MNSMS_REL_REQ)
              {
                 trigger = CNM_process_sms_release_message( connection_id);
              }
              else
              {
                 valid_msg = FALSE;
              }
              break;
          }
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
      case MS_TIMER:
          {
              if (message->message_id  == MID_TIMER_EXPIRY)
              {
                 trigger = SMS_TIMER_EXPIRY_event;
              }
              else
              {
                 valid_msg = FALSE;
              }
              break;
          }
#endif /*(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))*/
      default:
          valid_msg = FALSE;
          break;
      }

      if (valid_msg == FALSE)
      {
         MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received unknown SMS msg_set=%d msg_id=%d\n",
                 message->message_set, message->message_id);

         trigger = NO_ACTION_event;
      }
      current_state = sms_transaction_information[connection_id].sms_state;
      CNM_sms_respond_to_event( trigger, current_state, connection_id );
   }
   else
   {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Connection id exceeds the maximum permissible value, connection_id = %d\n",connection_id);
   }


}
/**************** End of   CNM_process_mt_sms ***********************************/


/**************** Begin of   CNM_check_validity_and_process_short_message *******************
 *
 *  Function name: CNM_check_validity_and_process_short_message ()
 *  ---------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function is called when MT SMS is received. 
 *  This checks whether another MT SMS is going on whose state is WAIT_FOR_CP_ACK. If yes
 *  then treat new MT SMS as CP-ACK for prev MT SMS.
 *  Else it checks whether another SMS is there whose state is not WAIT_CP_ACK. In this case,
 *  it rejects new MT SMS.
 *
 *  Parameters:
 *  -----------
 *  message : IN - input message
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  trigger - indicates event
 *
 ******************************************************************************/

boolean CNM_check_validity_and_process_short_message (connection_id_T conn_id_arg )
{

   connection_id_T   conn_id_local = 0x08;
   boolean           return_value = TRUE;

   /* Check if any other MT SMS session is active */
   for(conn_id_local = 0x08; conn_id_local < MAXNO_SMS_TRANSACTIONS; conn_id_local++)
   {
     if((conn_id_arg != conn_id_local) && sms_transaction_information[conn_id_local].sms_state != SMS_IDLE)
     {
        if(sms_transaction_information[conn_id_local].sms_state == SMS_WAIT_CP_ACK)
        {
           /* Treat CP-DATA for next MT SMS as CP-ACK for previous MT SMS if UE is waiting for CP_ACK from NW
                         Spec 24.011, Section  5.4
                     */

           CNM_sms_respond_to_event( S_CP_ACK_TRUE_event, SMS_WAIT_CP_ACK, conn_id_local );
        }
        else
        {
           /*
            * Section 3.2, Spec 24.011 - Parallel message transfer in same direction is prohibited.
            */
           CNM_send_cp_error(conn_id_arg,CP_CAUSE_MSG_NOT_COMPATIBLE_WITH_SMS_STATE);
           return_value = FALSE;
        }
        break;
     }
   }

   return return_value;

}

/**************** End of   CNM_check_validity_and_process_short_message ***********************************/





/**************** Begin of   CNM_process_received_short_message *******************
 *
 *  Function name: CNM_process_received_short_message ()
 *  ---------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function is called when the MS receives indication of the establishment
 *  of a new mobile terminating SMS transaction. If the transaction is currently
 *  inactive, then the message is send to MN and the event flagged to the
 *  calling function, otherwise an error indication is flagged
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN - input message
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  trigger - indicates event
 *
 ******************************************************************************/

byte CNM_process_received_short_message ( IMH_T * message,
                                         connection_id_T connection_id )
{
   byte              trigger;
   sms_state_T       current_state;
   MNSMS_EST_IND_T   mnsms_est_ind;
   byte              rpdu_len = 0;


   /*
   ** Clear flag, see l2/m60600.c for a description
   */



   /* If the  current state of SMS is waiting in final CP ACK, accept the new MT SMS with same TI . NAS is handling this to compatible with the Network behaviour*/
   if((connection_id >= 0x08) && (sms_transaction_information[connection_id].sms_state == SMS_WAIT_CP_ACK))
   { 
     /* Treat CP-DATA for next MT SMS as CP-ACK for previous MT SMS if UE is waiting for CP_ACK from NW
        Spec 24.011, Section  5.4
      */
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Received new MT SMS with same TI as on-going MT SMS in CP_ACK pending state conn ID =%d", connection_id);
  
      CNM_sms_respond_to_event( CP_ACK_TRUE_NEW_CP_DATA_RXD_event, SMS_WAIT_CP_ACK, connection_id );
   }

   /* Handle New SMS from here */
   current_state = sms_transaction_information[connection_id].sms_state;

   if((current_state == SMS_IDLE) && 
      (CNM_check_validity_and_process_short_message(connection_id) == TRUE))
   {

      /* Set Domain */
      if(connection_id < MAXNO_SMS_TRANSACTIONS)
      {
         sms_transaction_information[connection_id].pref_rl_domain = message->message_set;
      }
      else
      {
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid TI/conn ID = %d", connection_id);
      }

      mnsms_est_ind.message_header.message_set = MS_MN_SMS;
      mnsms_est_ind.message_header.message_id = MNSMS_EST_IND;
      mnsms_est_ind.connection_id  = connection_id;
	  mnsms_est_ind.sms_transport_type=NAS_MSG_TRANSPORT_TYPE_GW;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
      {
        mnsms_est_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif

      rpdu_len = ((cp_data_msg_T *)((void *)message))->data[0];

      (void)memscpy( (void *) &mnsms_est_ind.rpdu.data[0],
                     sizeof(mnsms_est_ind.rpdu.data),
                    (const void *) &((cp_data_msg_T *)((void *)message))->data[1],
                    rpdu_len);

      /*
       * fill in the length field in the header
       */

      PUT_IMH_LEN(((sizeof(MNSMS_EST_IND_T) -
                  sizeof(IMH_T)) -
                  sizeof(mnsms_est_ind.rpdu)) +
                  rpdu_len,
                  &mnsms_est_ind.message_header);

      /*
       * send message to MN
       */

      CNM_send_message( (IMH_T *)&mnsms_est_ind, GS_QUEUE_MN_CM );


      trigger = S_MT_SMS_REQUEST_event;

   }
   else
   {
      ERR("Specified connection already active: current SMS state = %d\n",
             (dword) current_state, 0, 0);

      trigger = NO_ACTION_event;
   }

   return trigger;
}
/**************** End of   CNM_process_received_short_message ***********************************/


/**************** Begin of   CNM_process_mo_sms ************************************
 *
 *  Function name: CNM_process_mo_sms ()
 *  -----------------------------------
 *
 *  Description:
 *  ------------
 *  This function calls other functions to process received messages, and
 *  performs other actions depending on the state of the transaction.
 *
 *
 *  Parameters:
 *  -----------
 *  message: IN - input message
 *  connection_id: IN
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mo_sms ( IMH_T * message,
                              connection_id_T  connection_id )
{
   sms_state_T             current_state;
   byte                    trigger;

   if (connection_id < MAXNO_SMS_TRANSACTIONS)
   {
      current_state = sms_transaction_information[connection_id].sms_state;
   
      if ((message->message_set == MS_MN_SMS) &&
          (message->message_id  == MNSMS_EST_REQ))
      {
         trigger = CNM_process_mo_sms_request( message, connection_id);
      }
      else
      if ((message->message_set == MS_MN_SMS) &&
          (message->message_id  == MNSMS_DATA_REQ))
      {
         trigger = CNM_send_rp_message( message, connection_id);
      }
      else
      if ((message->message_set == MS_MN_SMS) &&
          (message->message_id  == MNSMS_ABORT_REQ))
      {
         trigger = CNM_process_abort_request( connection_id);
      }
      else
      if (((message->message_set == MS_CC_MM) || (message->message_set == MS_CC_MM_PS))  &&
          (message->message_id  == MMCNM_DATA_IND))
      {
         trigger = CNM_process_received_cp_message( message, connection_id);
      }
      else
      if ((message->message_set == MS_MN_SMS) &&
          (message->message_id  == MNSMS_REL_REQ))
      {
         trigger = CNM_process_sms_release_message(connection_id);
      }
      else
      if (((message->message_set == MS_CC_MM) || (message->message_set == MS_CC_MM_PS)) &&
          (message->message_id  == MMCNM_EST_CNF))
      {
         trigger = MMSMS_EST_CNF_event;
      }
      else
   #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
      if ((message->message_set == MS_TIMER) &&
          (message->message_id  == MID_TIMER_EXPIRY))
      {
         trigger = SMS_TIMER_EXPIRY_event;
      }
      else
   #endif /*(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))*/
      {
         ERR("Unknown SMS msg: received message_set = %d\n",
                (dword) (message->message_set), 0, 0);
   
         trigger = NO_ACTION_event;
      }
   
      CNM_sms_respond_to_event( trigger, current_state, connection_id );
   }
   else
   {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Connection id exceeds the maximum permissible value, connection_id = %d\n",connection_id);
   }
}

/**************** End of   CNM_process_mo_sms *************************************/


/**************** Begin of   CNM_process_mo_sms_request ***************************
 *
 *  Function name: CNM_process_mo_sms_request ()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function calls other functions to process received messages, and
 *  performs other actions depending on the state of the transaction.
 *
 *
 *  Parameters:
 *  -----------
 *  message: IN - input message
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

byte CNM_process_mo_sms_request ( IMH_T * message,
                                 connection_id_T connection_id )
{
   byte              return_value;
   boolean           sms_active;
   sms_state_T       current_state;
   connection_id_T   transaction_id;
   cp_data_msg_T     cp_data_msg;
   byte              rpdu_len;

   sms_active = FALSE;

   return_value = NO_ACTION_event;

   current_state = sms_transaction_information[connection_id].sms_state;

   memset((VOID_DATA *)&cp_data_msg, 0, sizeof(cp_data_msg_T));

   if ( current_state == SMS_IDLE )
   {

      /*
       * form cp_data message
       */

      cp_data_msg.msg_head.IMH.message_set = MS_CC_MM;
      cp_data_msg.msg_head.IMH.message_id = MMCNM_DATA_REQ;
      cp_data_msg.msg_head.ti_pd =  (unsigned char) (FORM_TI_PD(connection_id, PD_SMS));
      cp_data_msg.msg_head.type =  CP_DATA;

      rpdu_len = ((MNSMS_EST_REQ_T *)((void *)message))->rpdu_len;

      if(rpdu_len > (MAX_LENGTH_SMS_DATA - 3))
      {
        ERR("rpdu_len: %d overflowed", rpdu_len, 0, 0);
        rpdu_len = MAX_LENGTH_SMS_DATA - 3;
      }

      cp_data_msg.data[0] = rpdu_len;    /* length of RPDU */

      (void) memscpy( (void *) &cp_data_msg.data[1],
                      sizeof(cp_data_msg.data) -1,
                     (const void *) &((MNSMS_EST_REQ_T *)((void *)message))->rpdu.data[0],
                     rpdu_len );

      PUT_IMH_LEN( rpdu_len+
                     sizeof(byte) +          /* cp length field   */
                     sizeof(byte) +          /* ti_pd field       */
                     sizeof(byte) ,          /* type field        */
                   ((IMH_T *) &cp_data_msg.msg_head.IMH ));

      for (transaction_id=0;
           transaction_id < MAXNO_SMS_TRANSACTIONS;
           transaction_id++ )
      {
         if ( sms_transaction_information[ transaction_id ].sms_state
                != SMS_IDLE )
         {
            sms_active = TRUE;
            break;
         }
      }

      /*
       If sms_active is TRUE and
       if the active transaction is an MT transaction, there is no
       need to queue the MO sms request. Queue the MO sms request
       only when the active transaction is an MO. This is verified
       by checking the transaction id that is active.
       If it is >=8 then the active transaction is an MT and therefore
       there is no need to queue the MO SMS request. Is it possible
       to have more than one active transaction ?  Yes. But if
       transaction_id is >= 8 then it means that there was no MO
       transaction active at all, because the MO transaction id range
       is from 0 till 7.
       */

      if ( sms_active != FALSE && transaction_id < 7 )     /* other sms active */
      {
         CNM_add_to_sms_request_queue ( &cp_data_msg, connection_id );
      }
      else
      {
         CNM_manage_cnm_service_requests( SHORT_MESSAGE_SERVICE_ESTABLISHMENT,
                                        connection_id,
                                        (IMH_T *)&cp_data_msg.msg_head.IMH );

         /*
          * store message in case retransmission is required
          */

         (void) memscpy( (void * ) &CNM_mo_cp_message,
                        sizeof(mmcnm_data_req_T),
                        (const void *) &cp_data_msg,
                        sizeof( cp_data_msg ) );

         /*
          * Initialize CNM_mo_cp_data_retransmit_counter
          */

         CNM_mo_cp_data_retransmit_counter = CP_DATA_RETRANSMIT_COUNT;
         MSG_MED_DS_2(MN_SUB,"=MNCNM= SMC-CS- %d MO retx counter = %d",connection_id, CNM_mo_cp_data_retransmit_counter);
      }

      return_value = MNSMS_EST_REQ_event;
   }
   else
   {
      ERR("SMS transaction already active: received connection_id = %d\n",
             (dword) connection_id, 0, 0);
   }
   return return_value;
}
/**************** End of   CNM_process_mo_sms_request ***********************************/


/**************** Begin of   CNM_process_abort_request ****************************
 *
 *  Function name: CNM_process_abort_request ()
 *  ------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles the request from MN to abort the SMS connection.
 *
 *
 *  Parameters:
 *  -----------
 *  message: IN - input message
 *  connection_id
 *
 *  Returns:
 *  --------
 *  trigger
 *
 ******************************************************************************/

byte CNM_process_abort_request ( connection_id_T connection_id )
{
   cp_error_msg_T    cp_error;

   if ((connection_id < MAXNO_SMS_TRANSACTIONS) &&
       (sms_transaction_information[connection_id].sms_state != SMS_IDLE ) &&
       (sms_transaction_information[connection_id].sms_state != SMS_MM_CONN_PEND))
   {

      cp_error.msg_head.IMH.message_set = sms_transaction_information[connection_id].pref_rl_domain;
      cp_error.msg_head.IMH.message_id = MMCNM_DATA_REQ;
      cp_error.msg_head.ti_pd = (unsigned char) (FORM_TI_PD(connection_id, PD_SMS));

      cp_error.msg_head.type = CP_ERROR;

      cp_error.cp_cause = 111;         /* protocol error, unspecified */

      PUT_IMH_LEN((sizeof(cp_error.cp_cause) 
                   + sizeof(cp_error.msg_head.ti_pd) 
                   + sizeof(cp_error.msg_head.type)),
                     &cp_error.msg_head.IMH);

      CNM_send_message((IMH_T *) &cp_error, GS_QUEUE_MM );
   }

   return C_ABORT_SMS_event;
}
/**************** End of   CNM_process_abort_request ***********************************/


/**************** Begin of   CNM_process_sms_release_message **********************
 *
 *  Function name: CNM_process_sms_release_message ()
 *  ------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles the request from MN to release the SMS connection.
 *  It simply flags the release as pending so that it ca be actioned later
 *  at the right time, ie when entering the mm connection established state
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message: IN - input message
 *  connection_id
 *
 *  Returns:
 *  --------
 *  trigger
 *
 ******************************************************************************/

byte CNM_process_sms_release_message ( connection_id_T connection_id )
{
   if ( sms_transaction_information[connection_id].sms_state != SMS_IDLE )
   {
      sms_transaction_information[connection_id].release_pending = TRUE;
   }

   return T_SMS_RELEASE_event;

}
/**************** End of   CNM_process_sms_release_message ***********************************/


/**************** Begin of   CNM_process_received_cp_message **********************
 *
 *  Function name: CNM_process_received_cp_message ()
 *  ------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a cp mesage received from the network. If a cp_data
 *  message, then this is stored for possible transfer to the MN layer. If
 *  cp-ack or cp-error, then the appropriate trigger is returned to the
 *  caller which takes appropriate action
 *
 *  Parameters:
 *  -----------
 *  message: IN - input message
 *  connection_id
 *
 *  Returns:
 *  --------
 *  trigger
 *
 ******************************************************************************/

byte CNM_process_received_cp_message ( IMH_T * message,
                                       connection_id_T connection_id )
{
   byte           message_type;
   byte           return_value;
   word           length;

   return_value = S_CP_ERROR_event;  /* Initialize  */

   if(connection_id < MAXNO_SMS_TRANSACTIONS)
   {
       if(message->message_set != sms_transaction_information[connection_id].pref_rl_domain)
       {
           MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Missmatched domain = %d", connection_id);
       }
   }

/*
 * get message type
 */

   message_type = (( msg_head_T *) ((void *)message))->type;

   switch (message_type)
   {
      case CP_DATA:


         if (connection_id < 0x08)
         {
           (void)memscpy(CNM_mo_cp_user_data.data,
                         sizeof(CNM_mo_cp_user_data.data),
                        (((cp_data_msg_T *)((void*)message))->data + 1),
                        (MAX_LENGTH_SMS_DATA - 2) - 1);

           /*
            * get length of rp_data portion
            */

           length = ((cp_data_msg_T *)((void *)message))->data[0];

           /*
            * store actual length
            */

           CNM_mo_cp_user_data.length = length;

           return_value = S_CP_DATA_event;
         }
         else
         {
           return_value = S_CP_ERROR_event;
         }
         break;

      case CP_ACK:
         return_value = S_CP_ACK_TRUE_event;
         break;

      case CP_ERROR:
         if(connection_id > UNKNOWN_CONNECTION_ID)
         {
           /* Network was supposed to send CP_ACK */
           /* 
            * For MT SMS, if UE receives CP-ERROR, event triggered is S_CP_ACK_FALSE.
            * This event is handled in different ways in different states in SMS state machine. 
            * */         
           cp_cause_value = ((cp_error_msg_T *)((void *)message))->cp_cause;
           return_value = S_CP_ACK_FALSE_event;
         }
         else
         {
           cp_cause_value = ((cp_error_msg_T *)((void *)message))->cp_cause;
           return_value = S_CP_ERROR_event;
         }

         break;


      default:
         ERR("Unexpected CP msg type=%d, conn=%d\n",
                (dword) message_type, connection_id, 0);
         break;
   }

   return return_value;
}
/**************** End of   CNM_process_received_cp_message *********************************/

/**************** Begin of CNM_active_SMS **********************
 *
 *  Function name: CNM_active_SMS ()
 *  ------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function returns TRUE if there are any active SMS session.
 *
 *  Parameters:
 *  -----------
 *  none
 *
 *  Returns:
 *  --------
 *  boolean
 *  ti of the SMS that waits for CP ACK
 *
 ******************************************************************************/
boolean CNM_active_SMS(byte  *ti)
{
  boolean active;
  for (*ti=0, active=FALSE; *ti<MAXNO_SMS_TRANSACTIONS; (*ti)++) /* for MO & MT SMS transactions */
  {
   if (sms_transaction_information[*ti].sms_state == SMS_WAIT_CP_ACK)
    {
      active = TRUE;
      break; /* there could only be one TI waiting CP ACK */
    }
  }
  return active;
}

void CNM_sms_cp_data_l2_ack( boolean transmit_ok)
{
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   cp_data_l2_ack_sim[0] = transmit_ok;
   MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Setting cp_data_l2_ack = %d for as_id = 0",transmit_ok);
#else
   cp_data_l2_ack = transmit_ok;
   MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Setting cp_data_l2_ack = %d",transmit_ok);
#endif
}

void CNM_sms_cp_data_l2_ack_subs_2(boolean transmit_ok)
{
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   if(MNCNM_IS_SINGLE_STACK)
   {
     cp_data_l2_ack_sim[0] = transmit_ok;
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Setting cp_data_l2_ack = %d for as_id = 0",transmit_ok);
   } 
   else
   {
     cp_data_l2_ack_sim[1] = transmit_ok;
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Setting cp_data_l2_ack = %d for as_id = 1",transmit_ok);
   }
#endif
}


boolean  is_cp_data(byte *data)
{
     if(data[1]==CP_DATA)
         return TRUE;
     else
         return FALSE;
}

/**************** End of CNM_active_SMS **********************/
