/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_process_cnm_sms_msgs.c_v   1.7   17 Jun 2002 17:07:24   rgurumoo  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_process_cnm_sms_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/10/01     CD     Removed unnecessary header file includes

5/23/01     CD     Corrected call to memset in MN_handle_MNSMS_ERROR_IND to
                   avoid lint error.  Deleted null_ptr since it was not accessed

                   Removed #includes for cause.h and ms.h since they are not used

6/8/01      CD     In MN_handle_MNSMS_ERROR_IND replaced call to MN_put_mmi_submit_sm_conf
                   by MN_put_uasms_sm_report_ind

7/18/01     CD     Moved external declaration of sm_rl_transaction_information to mn_sms.h

8/10/01     CD     Removed unnecessary queue.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

09/27/01    CD     In MN_handle_MNSMS_ERROR_IND, changed use of SM_RL_REPORT_IND due to changes to the structure
                   definition
                   Provided enum for error_status

6/17/02     RG     As a part of fixing CR 22313 reviewed all instances of failure in MN layer and
                   layers below MN that can happen during an MT SMS that need to be reported 
                   to the higher layer. Enabled reporting to the higher layer for the 
                   SMS_CP_ERROR and the SMS_LL_ERROR cases. For MT SMS, the 
                   SM_RL_REPORT_IND.short_message_identifier is used to pass the connection_id
                   of the transaction to the higher layer.

06/02/05    AB     Added SMS link control processing, when MM connection estbalishment
                   is rejected for SMS service.
                  
04/09/07    RD     SMS optimization changes are incorporated
10/05/11   gps     Initiated some primitives with all zeros

07/05/11   PM      Changed handling of MNSMS_ERROR_IND depending upon the 
                   lower_layer_cause sent by CNM in MNSMS_ERROR_IND_T

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include <string.h>
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "nasutils.h"



/*
 * Function name :  MN_handle_MNSMS_DATA_IND
 * -------------
 *
 * Description :  Forward a recieved short message response to the TP LAYER
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: IMH_T
 * ----------  
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_handle_MNSMS_DATA_IND(IMH_T *message_header)
{
   connection_id_T   connection_id;
   int               message_length;
   rpdu_T            cp_user_data;
   MNSMS_DATA_IND_T *message_ptr;

   /* Cast message header to message type */

   message_ptr = (MNSMS_DATA_IND_T *)message_header;

   /* Load data from message in to local data variables */

   message_length = (int)(message_ptr->message_header.message_len_lsb +
       (message_ptr->message_header.message_len_msb * 256));
   cp_user_data = message_ptr->rpdu;
   connection_id = message_ptr->connection_id;

   MN_get_cp_user_data(connection_id,
       message_length,
       cp_user_data,
	   NAS_MSG_TRANSPORT_TYPE_GW);

}



/*
 * Function name :  MN_handle_MNSMS_ERROR_IND
 * -------------
 *
 * Description :  Process error recieved at the CP layer
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: IMH_T
 * ----------  
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_handle_MNSMS_ERROR_IND(IMH_T *message_header)
{
   MNSMS_ERROR_IND_T          *message_ptr;
   SM_RL_REPORT_IND_T          SM_RL_REPORT_IND;
   control_sm_rl_return_data_T return_data;
   byte                        rl_state = 0;
   
   /* Initialize SM_RL_REPORT_IND  */
   memset(&SM_RL_REPORT_IND,0x00,sizeof(SM_RL_REPORT_IND_T));

   /* Cast message_header to message type */

   message_ptr = (MNSMS_ERROR_IND_T *)message_header;

   rl_state = sm_rl_transaction_information[message_ptr->connection_id].rl_state;
   
   return_data = MN_control_SMS_RL(message_ptr->connection_id,
       s_cnm_error);

   if ((return_data.success == TRUE) &&
       (return_data.sm_rl_report == TRUE))
   {
      /* Build the 'message' to send to RL */

       /* Fix for CR 22313 */
       if (message_ptr->connection_id > MAXNO_SMS_MO_TRANSACTIONS) 
       {
             /* 
             * MT transaction: Since the short_message_identifier field in the
             * SM_RL_REPORT_IND is unused, encode the connection_id in it, as
             * follows. The encoding is 0xXXXN, where N is the connection_id and
             * 0xXXX0 is MN_WMS_MT_MESSAGE_IDENTIFIER_BASE
             */

            if(rl_state == WAIT_FOR_CP_ACK)
            {
               SM_RL_REPORT_IND.short_message_identifier = 
                   MN_WMS_ACK_REPORT_IDENTIFIER_BASE | message_ptr->connection_id;
            }
            else
            {   /* to distinguish between error for MT CP_ACK[for RP_ACK sent]  
                       and other cases*/
                SM_RL_REPORT_IND.short_message_identifier = 
                    MN_WMS_MT_MESSAGE_IDENTIFIER_BASE | message_ptr->connection_id; 
            }
            mt_sms_transport_type = NAS_MSG_TRANSPORT_TYPE_MAX;
       }
       else
       {
            SM_RL_REPORT_IND.short_message_identifier = 
                sm_rl_transaction_information[message_ptr->connection_id].
                    short_message_identifier;
       }

       MSG_LOW_DS_2(MN_SUB,"=MNCNM= SMR-%d Set short_message_identifier to %d",
                         message_ptr->connection_id, SM_RL_REPORT_IND.short_message_identifier);

      if ( message_ptr->cp_cause.Iei == IEI_CP_CAUSE )
      {
         SM_RL_REPORT_IND.error_status = SMS_CP_ERROR;
         SM_RL_REPORT_IND.cause_value = message_ptr->cp_cause.cause_value;
      }
      else
      {
         if(message_ptr->lower_layer_cause == NO_RESPONSE_FROM_NEWTWORK)
         { 
            SM_RL_REPORT_IND.error_status = SMS_NO_RESPONSE_FROM_NETWORK;
            SM_RL_REPORT_IND.cause_value = 0;
         }
#if defined(FEATURE_DUAL_SIM )
         else if((MNCNM_IS_SINGLE_STACK) && (message_ptr->lower_layer_cause == AS_REJ_ABORT_RADIO_UNAVAILABLE))
         { 
            SM_RL_REPORT_IND.error_status = SMS_RPT_RADIO_UNAVAILABLE;
            SM_RL_REPORT_IND.cause_value = 0;
         }
#endif

         else if(message_ptr->lower_layer_cause == NO_RESPONSE_FROM_NEWTWORK_RLF)
         { 
            SM_RL_REPORT_IND.error_status = SMS_RADIO_LINK_ENDED_FOR_SUBMIT_REPORT;
            SM_RL_REPORT_IND.cause_value = 0;
         }

         else if(message_ptr->lower_layer_cause == NO_RESPONSE_FROM_NEWTWORK_TC1M_MT)
         { 
            SM_RL_REPORT_IND.error_status = SMS_NO_RESPONSE_FROM_NEWTWORK_FOR_SUBMIT_REPORT;
            SM_RL_REPORT_IND.cause_value = 0;
         }
         
         else
         {
            SM_RL_REPORT_IND.error_status = SMS_LL_ERROR;
            SM_RL_REPORT_IND.cause_value = message_ptr->lower_layer_cause;
         }
      }
      //Set the ims cause value from MNSMS_ERROR_IND
      SM_RL_REPORT_IND.ims_cause_value = message_ptr->lower_layer_cause_value;
      SM_RL_REPORT_IND.diagnostics_present = FALSE;
      SM_RL_REPORT_IND.diagnostics = 0;
      SM_RL_REPORT_IND.tpdu_length = 0;
#if defined(FEATURE_DUAL_SIM )
      if(MNCNM_IS_SINGLE_STACK)
      {
        SM_RL_REPORT_IND.as_id = sm_rl_transaction_information[message_ptr->connection_id].as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        SM_RL_REPORT_IND.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
#endif
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
      SM_RL_REPORT_IND.current_domain = (mmcc_domain_name_T)message_ptr->domain;
      SM_RL_REPORT_IND.permanent_error = message_ptr->permanent_error;
#endif
      memset(SM_RL_REPORT_IND.tpdu,'\0',sizeof(SM_RL_REPORT_IND.tpdu));

      if(message_ptr->retry_interval.present == TRUE)
      {
        SM_RL_REPORT_IND.retry_interval.present = TRUE;
        SM_RL_REPORT_IND.retry_interval.value = message_ptr->retry_interval.value;
      }
      MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Retry Interval for SMS, present = %d, value = %d",SM_RL_REPORT_IND.retry_interval.present, SM_RL_REPORT_IND.retry_interval.value);

      MN_put_uasms_sm_report_ind(&SM_RL_REPORT_IND);

      /* SMS Link control timer processing */
      if(sms_link_control.mode != SMS_LINK_CONTROL_DISABLED) 
      {
          /* stop any pending TLC */
          MN_stop_sms_timer(sms_link_control.connection_id, TLC1M);

          /* Update the link status */
          sms_link_control.status = SMS_LINK_CONTROL_ACTIVE;  /* prevent TLC from starting */

          /* Reset link control mode */
          sms_link_control.mode = ((sms_link_control.mode == SMS_LINK_CONTROL_ENABLED_ONE) ? 
                                   SMS_LINK_CONTROL_DISABLED : sms_link_control.mode);
         
          /* send report the UASMS */
          MN_put_uasms_link_control_report_ind(
                              sms_link_control.mode, 
#if defined(FEATURE_DUAL_SIM )
                              sm_rl_transaction_information[sms_link_control.connection_id].as_id,
#endif
                              SMS_LINK_CONTROL_CONNECTION_RELEASED
                              );

      } /* sms_link_control */

   }

}



/*
 * Function name :  MN_handle_MNSMS_EST_IND
 * -------------
 *
 * Description :  Forward a recieved short message response to the TP LAYER
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: IMH_T
 * ----------  
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_handle_MNSMS_EST_IND(IMH_T *message_header)
{
   connection_id_T   connection_id;
   int               message_length;
   rpdu_T            cp_user_data;
   MNSMS_EST_IND_T  *message_ptr;

   /* Cast message header to message type */

   message_ptr = (MNSMS_EST_IND_T *)message_header;

   /* Load data from message in to local data variables */

   message_length = (int)(message_ptr->message_header.message_len_lsb +
       (message_ptr->message_header.message_len_msb * 256));
   cp_user_data = message_ptr->rpdu;
   connection_id = message_ptr->connection_id;

   if (NAS_MSG_TRANSPORT_TYPE_MAX          == mt_sms_transport_type || 
       NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS != message_ptr->sms_transport_type)
   {
     mt_sms_transport_type = message_ptr->sms_transport_type;

     if (NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == message_ptr->sms_transport_type)
     {
       sm_rl_transaction_information[connection_id].as_id = message_ptr->as_id;
     }

     MN_get_cp_user_data(connection_id,
                         message_length,
                         cp_user_data,
                         message_ptr->sms_transport_type);
   }
   else
   {
     MSG_ERROR_DS_1(MN_SUB,"=MNCNM= MT_SMS from CNM already going on, drop IMS MT_SMS: %d", mt_sms_transport_type);
   }
}

/*
 * Function name :  MN_handle_MNSMS_CPACK_IND
 * -------------
 *
 * Description :  Forward a recieved CP_ACK in response to RP_ACK for MT_SMS
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: IMH_T
 * ----------  
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_handle_MNSMS_CPACK_IND(IMH_T *message_header)
{
   connection_id_T   connection_id;
   MNSMS_CPACK_IND_T *message_ptr;
   SM_RL_REPORT_IND_T               SM_RL_REPORT_IND;
   boolean                          diagnostics_present = FALSE;
   byte                             diagnostics = 0;
   byte                             tpdu_length= 0;   
   control_sm_rl_return_data_T      return_data;

   /* Initialise SM_RL_REPORT_IND */
   memset(&SM_RL_REPORT_IND,0x00,sizeof(SM_RL_REPORT_IND_T));
   SM_RL_REPORT_IND.error_status = NO_ERROR_RP_ACK;
   SM_RL_REPORT_IND.diagnostics_present = diagnostics_present;
   SM_RL_REPORT_IND.diagnostics = diagnostics;
   SM_RL_REPORT_IND.tpdu_length = tpdu_length;
   
   /* Cast message header to message type */
   message_ptr = (MNSMS_CPACK_IND_T *)message_header;

   connection_id = message_ptr->connection_id;
   return_data = MN_control_SMS_RL(connection_id, s_cnm_ack);
   if(return_data.success == TRUE)
   {
       SM_RL_REPORT_IND.short_message_identifier = MN_WMS_ACK_REPORT_IDENTIFIER_BASE | connection_id;
       SM_RL_REPORT_IND.error_status = NO_ERROR_CP_ACK;
       SM_RL_REPORT_IND.cause_value = message_ptr->cause_value;
       SM_RL_REPORT_IND.ims_cause_value = (word)0;
#if defined(FEATURE_DUAL_SIM )
       if(MNCNM_IS_SINGLE_STACK)
       {
         SM_RL_REPORT_IND.as_id = sm_rl_transaction_information[connection_id].as_id;
       }
#ifdef FEATURE_DUAL_ACTIVE
       else
       {
         SM_RL_REPORT_IND.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
       }
#endif
#endif
       MN_put_uasms_sm_report_ind(&SM_RL_REPORT_IND);

       mt_sms_transport_type = NAS_MSG_TRANSPORT_TYPE_MAX;
   }

}

