/*===========================================================================
            COPYRIGHT INFORMATION
 *   Returns:

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_timers.c_v   1.17   17 Jun 2002 17:37:08   rgurumoo  $   
<$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_timers.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $>

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/3/01      CD     Corrected several compile warnings in MN_start_sms_timer
                   and MN_stop_sms_timer.  
                       C4244 - conversion from int to byte
                       C4701 - local variables used without being initialized
                               (num_milli_seconds, timer_offset)
                     
                   Added error handling logic for unknown sms timer id

5/9/01      CD     Removed unnecesary header file includes

5/22/01     CD     Removed #include for cause.h and ms.h since they are not used

6/8/01      CD     In MN_handle_timeouts changed call to MN_put_cm_submit_sm_conf
                   by MN_put_uasms_sm_report_ind

7/5/01      CD     Added get_timer_name function
                   Added log messages

7/18/01     CD     Removed duplicate external declaration of ss_invoke_data
                   Removed duplicate external declaration of sm_rl_transaction_information

8/3/01      CD     Added log messages on timer expirations
                   Removed unnecessary nesting in mn_handle_timeouts()
                   Removed unnecesary include queue.h

8/10/01     CD     Changed ERR to MSG_ERROR in start/stop sms timer functions

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

9/10/01     CD     Corrected formatting of messages to avoid use of %s

9/15/01     CD     Corrected MSG_HIGH fomatting

09/27/01    CD     In MN_handle_timeouts, corrected use of SM_RL_REPORT_IND_T due to
                   changes in structure dedinition.  Provided enum for error_status, 
                   and fixed use of cause_value.

12/01/01    AB     Changed the MN_start_ss_timer() interface accept 
                      different relative timer value.

01/25/02    CD     Updated copyright
                   Replaced calls to ERR() by MSG_ERR()
                   Replaced IEI by boolean flag

01/29/02    RG     Initialized the SM_RL_REPORT_IND to CM_NULL_INFO(=0). The timer_id values 
                   for the TR1M timer should go from f1 to f4 since there are 4 MO connections that
                   are allowed (connection ids 0, 1, 2, and 3). The condition that checks 
                   for TR1M timer_id range should allow timer_id f4 to go through since 
                   it could correspond to a valid connection id.

02/04/02    CD     Replaced CM_NULL_INFO by 0x00, since it is no longer defined

02/22/02    AB     Added MN_call_clearing_on_t2_expired() in MN_handle_timeout() 
                   tear down any pending calls due to T2 timer expired 
                   on HOLD_REQ.
                     
02/25/02    AB     Modified the log message comments in MN_handle_timeout() to
                   fit target F3 message output.

03/12/02    AB     Added T(CD) and T(ect) timeout handling.

06/12/02    RG     Part of the fix for CR 22313. The connection_identifier is encoded in the 
                   SM_RL_REPORT_IND.short_message_identifier sent to the WMS when
                   the TR2M timer times out.

06/17/02    RG     Removed obsolete comments on TR1M timer id values.
 
                   Change in MN_start_sms_timer() function: Offset for TR1M timer should
                   be +1 (not +2). This will fix the problem with the calculation of the
                   connection_id during TR1M expiry processing. The TR1M timer_id range
                   will be f1-f7 mapping to connection_ids 0-6 (and hence the SMR's 0-6)
                   respectively. 

                   Used the MN_WMS_MT_MESSAGE_IDENTIFIER_BASE to encode the connection identifier
                   during the TR2M timer expiry.

07/19/02    AB     In MN_handle_timeouts(), restored all connection in MPTY 
                   to their original state for both the holdMPTY and retrieveMPTY 
                   operation, whose timer had been expired.

08/20/02    AB     In MN_handle_timeouts(), restored all connection in MPTY 
                   to their original state for buildMPTY & splitMPTY operation, 
                   whose timer had been expired.

03/10/02    AB     In MN_call_clearing_on_t2_expired(), reject the pending 
                   MO call if it in CC_STATE_U0 state. Otherwise, proceed to normal
                   call clearing.
 
04/15/03    sbs    Added support for Location Services related SS messages

12/15/04    AB     Added supports for AT+CMMS=1, ATCOP multiple MO SMS.

05/10/05    HS     Added support for FEATURE_CCBS

06/02/05    AB     Removed the TRAM from MN_SMS_TIMER+TRAM in 
                   MN_handle_timeouts_handling().

06/14/05    HS     Multicall status update to send CM correct active call list

04/09/07    RD     SMS optimization changes are incorporated

25/05/11    AM     Putting range check for call_session

09/19/11    PM     Fix for CR 305648. Correcting subscription id retrieval for
                   calls in DSDS when buildMpty timer expires.
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include <string.h>
#include "environ.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "mn_cnmtask.h"
#include "mn_cnmtask_int.h"
#include "ms_timer_v.h"
#include "msg.h"
#include "err.h"
#include "cause.h"
#include "ms.h"
#include "cnm.h"
#include "nasutils.h"


/*
 *   Function name : MN_start_cc_timer
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Starts timers for MN-CC functions 
 *  
 *  
 * 
 * 
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *     message_IEI
 *
 *  Returns:
 *     void
 *
 */
VOID_FUNC MN_start_cc_timer(connection_id_T connection_id, byte message_IEI)
{
   gs_status_T gs_status;
   timer_id_T timer_id = MN_CC_TIMER + connection_id;
   rex_timer_cb_type func_ptr = (rex_timer_cb_type)mn_cnm_timer_expiry;
   gs_queue_id_T queue_id = GS_QUEUE_MN_CM;

   /*  START APPROPRIATE TIMER FOR A CC NETWORK REQUEST  */

   /*  CLEAN THE CONNECTION ID's  */

   connection_id &= 0x0f;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)

     {
       if((!MNCNM_IS_SINGLE_STACK) && (mn_dsda_as_id == (mn_dsda_as_id_e_type)SYS_MODEM_AS_ID_2)) /* set the timer only if the device mode is DSDA */
       {
         func_ptr = mn_cnm_timer_expiry_subs_2;
         queue_id = GS_QUEUE_MN_CM_1;
       }
     }

#endif

   gs_status = gs_start_timer(queue_id, /* For the return messages */ 
                              (timer_id_T)(timer_id), /* Defined in timers.h */
                              func_ptr,
                              (milli_secs_T)(10000),  /* 10s */ 
                              (byte)message_IEI); 

   if(gs_status != GS_SUCCESS)
   {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to start cc timer");
    
   } 
}


/*
 *   Function name : MN_start_ss_timer
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Starts timers for MN-SS functions 
 *  
 *  
 * 
 * 
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *     invoke_id
 *
 *  Returns:
 *     void
 *
 */



VOID_FUNC MN_start_ss_timer(connection_id_T connection_id, byte invoke_id, milli_secs_T time)
{
   gs_status_T gs_status;
   timer_id_T timer_id = (MN_SS_TIMER + invoke_id);

   rex_timer_cb_type func_ptr = (rex_timer_cb_type)mn_cnm_timer_expiry;
   gs_queue_id_T queue_id = GS_QUEUE_MN_CM;


   /*  START APPROPRIATE TIMER FOR AN SS NETWORK REQUEST  */

   /*  CLEAN THE INVOKE ID's  */

   invoke_id &= 0x0f;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)

     {
       if((!MNCNM_IS_SINGLE_STACK) && (mn_dsda_as_id == (mn_dsda_as_id_e_type)SYS_MODEM_AS_ID_2)) /* set the timer only if the device mode is DSDA */
       {
         func_ptr = mn_cnm_timer_expiry_subs_2;
         queue_id = GS_QUEUE_MN_CM_1;
       }
     }
#endif


   gs_status = gs_start_timer(queue_id,   /* For the return messages */
                              (unsigned char)(timer_id), /* Defined in timers.h */
                              func_ptr,
                             (time),                   /* --------- "" ------ */
                              connection_id);


   if(gs_status != GS_SUCCESS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to start ss timer");;
   } 
}


/*
 *   Function name : MN_stop_cc_timer
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Stops timers for MN-CC functions 
 *  
 *  
 * 
 * 
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *     
 *
 *  Returns:
 *     void
 *
 */



VOID_FUNC MN_stop_cc_timer(connection_id_T connection_id)
{
   gs_status_T gs_status;
   gs_queue_id_T queue_id = GS_QUEUE_MN_CM;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     if(mn_dsda_as_id == (mn_dsda_as_id_e_type)SYS_MODEM_AS_ID_2)
     {
        queue_id = GS_QUEUE_MN_CM_1;
     }
   }
#endif

   /*  START APPROPRIATE TIMER FOR AN SS NETWORK REQUEST  */

   /*  CLEAN THE CONNECTION ID's  */

   connection_id &= 0x0f;

   gs_status = gs_cancel_timer( (unsigned char)(queue_id),     /* For the return messages */
                  (unsigned char)(MN_CC_TIMER + connection_id));  /* Defined in timers.h */


   if(gs_status != GS_SUCCESS)
   {
     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to stop cc timer");
   }

}


/*
 *   Function name : MN_stop_ss_timer
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Stops timers for MN-SS functions 
 *  
 *  
 * 
 * 
 *  
 *
 *  Parameters:
 *  -----------
 *  IN invoke_id
 *     
 *
 *  Returns:
 *     void
 *
 */



VOID_FUNC MN_stop_ss_timer(byte invoke_id)
{
   gs_status_T gs_status;
   gs_queue_id_T queue_id = GS_QUEUE_MN_CM;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     if(mn_dsda_as_id == (mn_dsda_as_id_e_type)SYS_MODEM_AS_ID_2)
     {
        queue_id = GS_QUEUE_MN_CM_1;
     }
   }
#endif

   /*  START APPROPRIATE TIMER FOR AN SS NETWORK REQUEST  */

   /*  CLEAN THE CONNECTION ID's  */

   invoke_id &= 0x0f;

   gs_status = gs_cancel_timer(queue_id,  /* For the return messages */
   (unsigned char)(MN_SS_TIMER + invoke_id));                  /* Defined in timers.h */


   if(gs_status != GS_SUCCESS)
   {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Failed to stop ss timer");
   }
}


/*
 *   Function name : MN_start_sms_timer
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Starts timers for MN-SMS functions 
 *  
 *  
 * 
 * 
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *     sms_timer_id
 *
 *  Returns:
 *     void
 *
 */



VOID_FUNC
MN_start_sms_timer(connection_id_T connection_id,
                     byte            sms_timer_id)
{
   gs_status_T return_value;
   dword       num_milli_seconds;
   byte        timer_offset;
   timer_id_T  timer_id;

   rex_timer_cb_type func_ptr = (rex_timer_cb_type)mn_cnm_timer_expiry;
   gs_queue_id_T queue_id = GS_QUEUE_MN_CM;

   connection_id &= 0x0f;

   return_value = GS_SUCCESS;

   if (sms_timer_id == TR1M)
   {
      /* when SMS is over IMS, NAS should start Timerf value if it is greater than TR1M . Other wise, NAS should start TR1M timer */
      num_milli_seconds = ((mo_sms_transport_type == NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS) ? (MAX(sip_config_timer_f,SMS_TR1M_TIMEOUT_VALUE )) : SMS_TR1M_TIMEOUT_VALUE );
      /* corrected compile warning C4244 conversion from int to byte */
      timer_offset = TR1M_TIMER_OFFSET;
   }
   else if (sms_timer_id == TR2M)
   {
      num_milli_seconds = SMS_TR2M_TIMEOUT_VALUE;
      timer_offset = TR2M_TIMER_OFFSET;
   }
   else if (sms_timer_id == TRAM)
   {
      num_milli_seconds = SMS_TRAM_TIMEOUT_VALUE;
      timer_offset = TRAM_TIMER_OFFSET;
   }
   else if (sms_timer_id == TLC1M)
   {
      num_milli_seconds = sms_link_control.duration * 1000; 
      timer_offset = TLC1M_TIMER_OFFSET;
   }
   else
   {
       num_milli_seconds = 0;
       timer_offset = 0;
       return_value = GS_FAILURE;
       MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR:  SMR-%d received a cmd to start unknown SMS timer ID %d",
                  connection_id,sms_timer_id);
   }

   timer_id = (MN_SMS_TIMER + timer_offset);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     if((!MNCNM_IS_SINGLE_STACK) && (mn_dsda_as_id == (mn_dsda_as_id_e_type)SYS_MODEM_AS_ID_2)) /* set the timer only if the device mode is DSDA */
     {
       func_ptr = mn_cnm_timer_expiry_subs_2;
       queue_id = GS_QUEUE_MN_CM_1;
     }
   }
#endif


   if(return_value == GS_SUCCESS)
   {

       return_value = gs_start_timer(queue_id,
                                  (unsigned char)timer_id,
                                  func_ptr,
                                  num_milli_seconds,
                                  connection_id);

#ifdef FEATURE_MN_UNIT_TEST
       MSG_MED_DS_2(MN_SUB,"=MNCNM= SMR-%d started timer %s", connection_id,
            get_timer_name(sms_timer_id));
#else
       MSG_MED_DS_2(MN_SUB,"=MNCNM= SMR-%d started timer %d", connection_id, sms_timer_id);
#endif

       if(return_value != GS_SUCCESS)
       {
#ifdef FEATURE_MN_UNIT_TEST
          MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: SMR-%d failed to start sms timer %s",
              connection_id, get_timer_name(sms_timer_id));
#else
          MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: SMR-%d failed to start sms timer %d",
              connection_id, sms_timer_id);
#endif
       }
   }
}



/*
 *   Function name : MN_stop_sms_timer
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Stops timers for MN-SMS functions 
 *  
 *  
 * 
 * 
 *  
 *
 *  Parameters:
 *  -----------
 *  IN connection_id
 *     sms_timer_id
 *
 *  Returns:
 *     void
 *
 */



VOID_FUNC
MN_stop_sms_timer(connection_id_T connection_id,
                   byte sms_timer_id)
{
   gs_status_T   return_value;
   byte        timer_offset;
   gs_queue_id_T queue_id = GS_QUEUE_MN_CM;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     if(mn_dsda_as_id == (mn_dsda_as_id_e_type)SYS_MODEM_AS_ID_2)
     {
        queue_id = GS_QUEUE_MN_CM_1;
     }
   }
#endif

   return_value = GS_SUCCESS;
   connection_id &= 0x0f;

   if (sms_timer_id == TR1M)
   {
       /* corrected compile warning C4244 conversion from int to byte */
       timer_offset = TR1M_TIMER_OFFSET;
   }
   else if (sms_timer_id == TR2M)
   {
      timer_offset = TR2M_TIMER_OFFSET;
   }
   else if (sms_timer_id == TRAM)
   {
      timer_offset = TRAM_TIMER_OFFSET;
   }
   else if (sms_timer_id == TLC1M)
   {
      timer_offset = TLC1M_TIMER_OFFSET;
   }
   else
   {
       timer_offset = 0;
       return_value = GS_FAILURE;
       MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR:  SMR-%d received a cmd to stop unknown SMS timer ID %d",
                  connection_id,sms_timer_id);
   }

   if(return_value == GS_SUCCESS)
   {
        return_value = gs_cancel_timer(queue_id,
                                  (unsigned char)(MN_SMS_TIMER + timer_offset));

#ifdef FEATURE_MN_UNIT_TEST
        MSG_MED_DS_2(MN_SUB,"=MNCNM= SMR-%d stopped timer %s", connection_id,
                 get_timer_name(sms_timer_id));
#else
        MSG_MED_DS_2(MN_SUB,"=MNCNM= SMR-%d stopped timer %d", connection_id, sms_timer_id);
#endif

        if (return_value != GS_SUCCESS)
        {
#ifdef FEATURE_MN_UNIT_TEST
            MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: SMR-%d failed to stop sms timer %s",connection_id,
               get_timer_name(sms_timer_id));
#else
            MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: SMR-%d failed to stop sms timer %d",connection_id,
               sms_timer_id);
#endif
        }
   }

}

/*
 *   Function name : MN_handle_t2_expired
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Clear any pending associated with the T2 Timer expiry.
 *  
 *   Parameters:
 *   -----------
 *   IN connection_id
 *     message_IEI
 *
 *   Returns:
 *     void
 *
 */

VOID_FUNC MN_call_clearing_on_t2_expired (connection_id_T  connection_id)
{

   ie_cm_cc_cause_T  cc_cause;
   connection_id_T   disconnect_id;
   mn_call_information_T    *call_data = NULL;
   MNCC_REJ_IND_T    cc_call_reject;
 

   /* Set up call clearing cuase IEI */
   memset((VOID_DATA *)&cc_cause,'\0',sizeof(ie_cm_cc_cause_T));
   memset((VOID_DATA *)&cc_call_reject, 0x00, sizeof(MNCC_REJ_IND_T));

   cc_cause.present = TRUE;
   cc_cause.location = STD_LOCATION;
   cc_cause.coding_standard = STD_CODESTANDARD;
   cc_cause.recommendation_present = FALSE;
   cc_cause.cause_value = RECOVERY_ON_TIMER_EXPIRY;
   cc_cause.diagnostic_length = 0;

   /* clear MT call waiting */ 
   if (Multicall_Status[MN_CALL_WAITING_SESSION].call_activity == CALL_WAITING)
   {
       disconnect_id = Multicall_Status[2].connection_id[0];

       MN_put_MNCC_DISC_REQ(disconnect_id, &(cc_cause), NULL);

       mn_call_information_p[disconnect_id]->mo_call_clear_pending = TRUE;
   }
   else /* clear any pending MO calls */
   { 
       for(disconnect_id = 0; disconnect_id < MAXNO_CC_TRANSACTIONS; disconnect_id++) 
       {
           call_data =  mn_call_information_p[disconnect_id];

           if((disconnect_id != connection_id) && (call_data != NULL) && (call_data->mo_setup_pending == TRUE))
           {
               if((cc_transaction_information_p[disconnect_id] != NULL) &&
                  (cc_transaction_information_p[disconnect_id]->cc_state != CC_STATE_U0))
               { 
                   /* normal call clearing */
                   MN_put_MNCC_DISC_REQ(disconnect_id, &(cc_cause), NULL);

                   mn_call_information_p[disconnect_id]->mo_setup_pending = FALSE;
                   mn_call_information_p[disconnect_id]->mo_call_clear_pending = TRUE;
                   
               }
               else
               {
                   /* reject the call and clear MN database */
                   cc_call_reject.message_header.message_set = MS_MN_CC;
                   cc_call_reject.message_header.message_id = MNCC_REJ_IND;
                   cc_call_reject.message_header.message_len_lsb = 6;
                   cc_call_reject.message_header.message_len_msb = 0;
                   cc_call_reject.connection_id = disconnect_id;
                   cc_call_reject.cause = RECOVERY_ON_TIMER_EXPIRY;
                   cc_call_reject.data[0] = IEI_CAUSE;
                   cc_call_reject.data[1] = 2;
                   cc_call_reject.data[2] = 0x80 | STD_CODESTANDARD | STD_LOCATION;
                   cc_call_reject.data[3] = 0x80 | RECOVERY_ON_TIMER_EXPIRY;

                   if(mn_call_information_p[disconnect_id] != NULL)
                   {
                     mn_call_information_p[disconnect_id]->call_reject_cause_diag_packet.present = TRUE;
                     mn_call_information_p[disconnect_id]->call_reject_cause_diag_packet.cause_value = NORMAL_UNSPECIFIED; // 0x713D event needs cause as 31 in this scenario
                   }


                   MN_handle_MNCC_REJ_IND(&cc_call_reject.message_header);
               }
           }
       } /* disconnect_id */  
   }
} /* VOID MN_handle_t2_expired */


/*
 *   Function name : MN_handle_timeouts
 *   ---------------
 *
 *   Description:
 *   ------------
 *       This function handles all timeouts instantiated
 *       by the MN layer
 * 
 *
 *
 *  Parameters:
 *  -----------
 *  IN Timer_expiry_message
 *
 *  Returns:
 *
 *
 */
VOID_FUNC MN_handle_timeouts(IMH_T *message)
{
   byte                             invoke_id;
   boolean                          s_respond_to_event = FALSE;
   connection_id_T                  connection_id = 0;
   timer_expiry_T                   *message_ptr;
   byte                             cc_command;
   byte                             ss_code;
   byte                             cm_ss_ref;
   ie_cm_ss_error_T                 cm_ss_error;
   control_sm_rl_return_data_T      return_data;
   SM_RL_REPORT_IND_T               SM_RL_REPORT_IND;
   sm_rl_transaction_information_T  *sm_data_ptr;
   ie_facility_T                    facility_data;
   int                              call_session, index;
   int                              Multicall_Status_index;
   unsigned int                     CC_auxiliary_state;
   boolean                          is_valid_cmd = FALSE;

   /* Initialize the SM_RL_REPORT_IND */

   memset(&SM_RL_REPORT_IND, 0x00, sizeof(SM_RL_REPORT_IND));

   message_ptr = (timer_expiry_T *)message;

   /* Clean the cm_ss_error memory */

   memset(&cm_ss_error,'\0',sizeof(cm_ss_error));

   cm_ss_error.present = TRUE;

   /* Initialise the facility data */

   memset(&facility_data, '\0', sizeof(ie_facility_T));

   if(message_ptr->IMH.message_id == MID_TIMER_EXPIRY)
   {
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN_[x]_TIMER expired ID = %d", message_ptr->timer_id); 

      if((message_ptr->timer_id >= MN_CC_TIMER) && (message_ptr->timer_id < MN_SS_TIMER))
      {
         /*  A MN CC TIMER HAS TIMED OUT  */
         connection_id = (connection_id_T)(message_ptr->timer_id - MN_CC_TIMER);
         cc_command = message_ptr->data;

         switch(cc_command)
         {
            case CCE_CALL_HOLD_REQ:
            {

             if((cc_transaction_information_p[connection_id] != NULL) && (connection_id < MAXNO_CC_TRANSACTIONS))
             {
               CC_auxiliary_state = (byte)(cc_transaction_information_p[connection_id]->mpty_auxiliary_state +
                                                            (4* cc_transaction_information_p[connection_id]->hold_auxiliary_state));

               if(CC_auxiliary_state==AUX_HOLD_REQUEST)
               {
                 is_valid_cmd = TRUE;
               }

             }

             if(is_valid_cmd)
             {
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= HOLD timer expired for conn = %d, CCE_CALL_HOLD_REQ", connection_id);

               /* Performances call clearing per TS 24.08 */
               MN_call_clearing_on_t2_expired(connection_id);

               if (mn_alternate_proc_status.is_alternate_procedure == TRUE)
               {
                  /* Restore orginial state */
                  s_respond_to_event = 
                    MN_manage_CC_auxiliary_state(connection_id, CCE_CALL_HOLD_REJ);
                 if(s_respond_to_event == TRUE)
                 {
                    if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED)
                    {
                      mn_alternate_proc_status.status = MN_SS_ALT_NACKED;
                    }
                    else if (mn_alternate_proc_status.status == MN_SS_ALT_NACKED || 
                             mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_AWAITED)
                    {
                      if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_AWAITED)
                      {
                        Multicall_Status_index = (int)MN_whereami(connection_id);

                        /*the other has been already retrieved, update the auxiliary states */
                        /*Point at the other call */
                        Multicall_Status_index = (Multicall_Status_index + 1) % 2;

                        if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
                        {
                          /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
                          for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
                          {
                            (void)MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].
                                     connection_id[index], CCE_RETRIEVE_MPTY_REJ);
                            
                          }
                        }
                        else
                        {
                          s_respond_to_event = MN_manage_CC_auxiliary_state (Multicall_Status[Multicall_Status_index].connection_id[0],
                                                                                                      CCE_CALL_RETRIEVE_REJ);/*c_call_event */
                          if (s_respond_to_event == FALSE)
                          {
                            MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR:ALT Proc on,CC Aux state of %d call in wrong state",
                                                               Multicall_Status[Multicall_Status_index].connection_id[0]);
                          }
                        }
                      }

                      mn_alternate_proc_status.is_alternate_procedure = FALSE;
                      mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;

                      cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                      cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

                      MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                       mn_call_information_p[connection_id]->as_id, 
#endif 
                                                  cm_ss_error);
                    }
                 }
                 else
                 {
                    MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: CCE_CALL_HOLD_REJ event ignored",message_ptr->timer_id);
                 }
               }
               else
               {
                 /* Restore orginial state */
                 s_respond_to_event = 
                   MN_manage_CC_auxiliary_state(connection_id, CCE_CALL_HOLD_REJ);

                 /* inform CM */
                 if(s_respond_to_event == TRUE)
                 {
                   cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                   cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

                   MN_MULTICALL_LOG(MN_read_multicall_status());                                         

                   MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                            mn_call_information_p[connection_id]->as_id, 
#endif 
                                            cm_ss_error);
               }
                 else
                 {
                   MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: CCE_CALL_HOLD_REJ event ignored",message_ptr->timer_id);
                 }
               }
             }
             else
             {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected Hold Timer Expiry");
             }

               break;
            }

            case CCE_CALL_RETRIEVE_REQ:
            {

               if((cc_transaction_information_p[connection_id] != NULL) && (connection_id < MAXNO_CC_TRANSACTIONS))
               {
                 CC_auxiliary_state = (byte)(cc_transaction_information_p[connection_id]->mpty_auxiliary_state +
                                                            (4* cc_transaction_information_p[connection_id]->hold_auxiliary_state));

                 if(CC_auxiliary_state==AUX_RETRIEVE_REQUEST)
                 {
                   is_valid_cmd = TRUE;
                 }

               }

              if(is_valid_cmd)
              {

               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= RETRIEVE timer expired conn = %d, CCE_CALL_RETRIEVE_REQ",
                                                           connection_id);
               if (mn_alternate_proc_status.is_alternate_procedure == TRUE)
               {
                 s_respond_to_event =
                   MN_manage_CC_auxiliary_state(connection_id, 
                                                CCE_CALL_RETRIEVE_REJ);
                 if(s_respond_to_event == TRUE)
                 {
                    if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED)
                    {
                      mn_alternate_proc_status.status = MN_SS_ALT_NACKED;
                    }
                    else if (mn_alternate_proc_status.status == MN_SS_ALT_NACKED || 
                             mn_alternate_proc_status.status == MN_SS_ALT_RETRIEVE_ACK_AWAITED)
                    {
                      if (mn_alternate_proc_status.status == MN_SS_ALT_RETRIEVE_ACK_AWAITED)
                      {
                        Multicall_Status_index = (int)MN_whereami(connection_id);

                        /*the ACK for other call has been already Received, update the auxiliary states */
                        /*Point at the other call */
                        Multicall_Status_index = (Multicall_Status_index + 1) % 2;

                        if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
                        {
                          /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
                          for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
                          {
                            (void)MN_manage_CC_auxiliary_state(
                                              Multicall_Status[Multicall_Status_index].connection_id[index], 
                                                  CCE_HOLD_MPTY_REJ);
                          }
                        }
                        else
                        {
                          s_respond_to_event = MN_manage_CC_auxiliary_state (Multicall_Status[Multicall_Status_index].connection_id[0],
                                                                                                            CCE_CALL_HOLD_REJ);/*c_call_event */
                          if (s_respond_to_event == FALSE)
                          {
                            MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR:ALT Proc on,CC Aux state of %d call in wrong state",
                                                                             Multicall_Status[Multicall_Status_index].connection_id[0]);
                          }
                        }
                      }
                    
                      mn_alternate_proc_status.is_alternate_procedure = FALSE;
                      mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;

                      cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                      cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

                      MN_MULTICALL_LOG(MN_read_multicall_status());                      

                      MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                          mn_call_information_p[connection_id]->as_id, 
#endif 
                                            cm_ss_error);
                    }
                 }
                 else
                 {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: CCE_CALL_RETRIEVE_REJ event ignored");
                 }
               }
               else
               {
                 s_respond_to_event =
                   MN_manage_CC_auxiliary_state(connection_id, 
                                                CCE_CALL_RETRIEVE_REJ);
                 if(s_respond_to_event == TRUE)
                 {
                   cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                   cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

                   MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                               mn_call_information_p[connection_id]->as_id, 
#endif 
                                               cm_ss_error);
                 }
                 else
                 {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: CCE_CALL_RETRIEVE_REJ event ignored");
                 }
               }
              }
              else
              {
                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected RETRIEVE Timer Expiry");
              }
               break;
            }

           case CCE_CALL_DEFLECTION_REQ:
            {
#if defined(FEATURE_DUAL_SIM )
               sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;

               if(MNCNM_IS_SINGLE_STACK)
               {
                 if (mn_call_information_p[connection_id] != NULL)
                 {
                   as_id = mn_call_information_p[connection_id]->as_id;
                 }
                 else if(mn_as_id != SYS_MODEM_AS_ID_NONE)
                 {
                   as_id = mn_as_id;
                 }
               }
#endif
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= CD Timer expired for conn = %d, CCE_CALL_DEFLECTION_REQ",
                                                            connection_id);
               /* notify CM */
               cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
               cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

               MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                           as_id, 
#endif 
                                           cm_ss_error);

               /* Performances call clearing per TS 24.08 */
               MN_call_clearing_on_t2_expired(connection_id);

               break;
            }

          case CCE_CALL_TRANSFER_REQ:
            {
#if defined(FEATURE_DUAL_SIM )
               sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;
               if(MNCNM_IS_SINGLE_STACK)
               {
                 if (mn_call_information_p[connection_id] != NULL)
                 {
                   as_id = mn_call_information_p[connection_id]->as_id;
                 }
                 else if(mn_as_id != SYS_MODEM_AS_ID_NONE)
                 {
                   as_id = mn_as_id;
                 }
               }
#endif
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Explicit CT Timer expired for conn = %d, CCE_CALL_TRANSFER_REQ",
                                                            connection_id);

               /* notify CM */
               cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
               cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

               MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                           as_id, 
#endif 
                                           cm_ss_error);

               break;
            }

            default:
            {
              MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unknown command event %d for MN CC timer expiration connection_id %d",
                   cc_command, connection_id);

              break;
            }

         } /* switch (cc_command) */

         /* stop all CC SS call-related timer in case of radio link failure */
         MN_stop_cc_timer(connection_id);

      } /* CC_TIMER */
      else if( (message_ptr->timer_id >= MN_SS_TIMER) &&
             (message_ptr->timer_id < (MN_SS_TIMER + MAXNO_INVOKE_IDS)) )
      {
#if defined(FEATURE_DUAL_SIM )
         sys_modem_as_id_e_type         as_id = SYS_MODEM_AS_ID_NONE;
#endif
         /*  AN SS TIMER HAS TIMED OUT  */
         connection_id = message_ptr->data;
         invoke_id = (byte)(message_ptr->timer_id - MN_SS_TIMER);

         /*  FIND OUT THE CC OPERATION  */

         ss_code = MN_retrieve_ss_operation(invoke_id, TRUE);

         call_session = MN_whereami(connection_id); /* Call Session */

         switch(ss_code)
         {
            case holdMPTY:
            {
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN_SS_TIMER expired on holdMTPY Id = %d ", invoke_id);

               if(call_session >= MULTICALL_STATUS_SIZE)
               {
                  if(1 == Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_is_mpty)
                  {
                      call_session = MN_ACTIVE_HOLD_SESSION;
                  }
                  else
                  {
                      call_session = MN_HOLD_ACTIVE_SESSION;
                  }
               }
               else if(mn_call_information_p[connection_id] != NULL)
               {
                   mn_call_information_p[connection_id]->hold_request = FALSE;
                   mn_call_information_p[connection_id]->ss_request_pending = CM_NONE_PENDING;
                   /* Performances call clearing per TS 24.08 */
                   MN_call_clearing_on_t2_expired(connection_id);
                   call_session = MN_whereami(connection_id); /* Call Session */
               }  
               if (mn_alternate_proc_status.is_alternate_procedure == TRUE)  
               {
                 /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
                 for(index = 0; index < Multicall_Status[call_session].id_index; index++)
                 {
                   s_respond_to_event = MN_manage_CC_auxiliary_state(Multicall_Status[call_session].
                     connection_id[index], CCE_HOLD_MPTY_REJ);
                 }

                 if(s_respond_to_event == TRUE)
                 {

                   if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED)
                   {
                     mn_alternate_proc_status.status = MN_SS_ALT_NACKED;
                   }
                   else if (mn_alternate_proc_status.status == MN_SS_ALT_NACKED || 
                      mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_AWAITED)
                   {
                     if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_AWAITED)
                     {
                       /*the ACK for other call (NON - MPTY) has been already Received, update the auxiliary states */
                       /*Point at the other call */
                       call_session =  (call_session + 1) % 2;
                       s_respond_to_event = MN_manage_CC_auxiliary_state (Multicall_Status[call_session].connection_id[0],
                                                             CCE_CALL_RETRIEVE_REJ);/*c_call_event */
                       if (s_respond_to_event == TRUE)
                       {
                         MN_stop_cc_timer(Multicall_Status[call_session].connection_id[0]);   
                       }
                       else
                       {
                         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR:ALT Proc on,CC Aux state of %d call in wrong state",connection_id);
                       }
                     }
                     mn_alternate_proc_status.is_alternate_procedure = FALSE;
                     mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;
                   
                     cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                     cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

                     (void)MN_read_multicall_status();
                     /* to provide CM with correct active call list */

					 /* here we are taking the as_id for which timer expired . Even if it is MPTY call , as_id will be same for the calls which are in mpty*/
                     MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                             mn_call_information_p[connection_id]->as_id, 
#endif 
                             cm_ss_error);
                   }
                 }
               }
               else
               {
                 for (index=0; index < Multicall_Status[call_session].id_index ; index++)
                 {
                   s_respond_to_event =
                      MN_manage_CC_auxiliary_state(Multicall_Status[call_session].connection_id[index], CCE_HOLD_MPTY_REJ);
                 }

                 if(s_respond_to_event == TRUE)
                 {
                   cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                   cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

                   (void)MN_read_multicall_status();
                   /* to provide CM with correct active call list */

                   MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                             mn_call_information_p[Multicall_Status[call_session].connection_id[--index]]->as_id, 
#endif 
                             cm_ss_error);
                 }
                 else
                 {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: CC_HOLD_MPTY_REJ event ignored");
                 }
               }
               break;
            }

            case retrieveMPTY:
            {
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN_SS_TIMER expired on retrieveMPTY Id = %d", invoke_id);

               if(call_session >= MULTICALL_STATUS_SIZE)
               {
                  if(1 == Multicall_Status[MN_ACTIVE_HOLD_SESSION].call_is_mpty)
                  {
                      call_session = MN_ACTIVE_HOLD_SESSION;
                  }
                  else
                  {
                      call_session = MN_HOLD_ACTIVE_SESSION;
                  }
               }
               else if(mn_call_information_p[connection_id] != NULL)
               {
                  mn_call_information_p[connection_id]->ss_request_pending = CM_NONE_PENDING;
               }
               if (mn_alternate_proc_status.is_alternate_procedure == TRUE)  
               {
                 /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
                 for(index = 0; index < Multicall_Status[call_session].id_index; index++)
                 {
                   s_respond_to_event = MN_manage_CC_auxiliary_state(Multicall_Status[call_session].
                     connection_id[index], CCE_RETRIEVE_MPTY_REJ);
                 }

                 if(s_respond_to_event == TRUE)
                 {

                   if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED)
                   {
                     mn_alternate_proc_status.status = MN_SS_ALT_NACKED;
                   }
                   else if (mn_alternate_proc_status.status == MN_SS_ALT_NACKED || 
                      mn_alternate_proc_status.status == MN_SS_ALT_RETRIEVE_ACK_AWAITED)
                   {
                     if (mn_alternate_proc_status.status == MN_SS_ALT_RETRIEVE_ACK_AWAITED)
                     {
                       /*the ACK for other call (NON - MPTY) has been already Received, update the auxiliary states */
                       /*Point at the other call */
                       call_session =  (call_session + 1) % 2;
                       s_respond_to_event = MN_manage_CC_auxiliary_state (Multicall_Status[call_session].connection_id[0],
                                                             CCE_CALL_HOLD_REJ);/*c_call_event */
                       if (s_respond_to_event == TRUE)
                       {
                         MN_stop_cc_timer(Multicall_Status[call_session].connection_id[0]);   
                       }
                       else
                       {
                         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR:ALT Proc on,CC Aux state of %d call in wrong state",connection_id);
                       }
                     }
                     mn_alternate_proc_status.is_alternate_procedure = FALSE;
                     mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;
                   
                     cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                     cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

                     (void)MN_read_multicall_status();
                     /* to provide CM with correct active call list */

					 /* here we are taking the as_id for which timer expired . Even if it is MPTY call , as_id will be same for the calls which are in mpty*/
                     MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                             mn_call_information_p[connection_id]->as_id, 
#endif 
                             cm_ss_error);
                   }
                 }
               }
               else
               {

                 for (index=0; index < Multicall_Status[call_session].id_index ; index++)
                 {
                   s_respond_to_event =
                       MN_manage_CC_auxiliary_state(Multicall_Status[call_session].connection_id[index], CCE_RETRIEVE_MPTY_REJ);
                 }

                 if(s_respond_to_event == TRUE)
                 {
                   cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                   cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

                   (void)MN_read_multicall_status();
                    /* to provide CM with correct active call list */

                    MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                            mn_call_information_p[Multicall_Status[call_session].connection_id[--index]]->as_id, 
#endif 
                                            cm_ss_error);
                 }
                 else
                 {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: CC_RETRIEVE_MPTY_REJ event ignored");
                 }
               }
               break;
            }

            case buildMPTY:
            {
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN_SS_TIMER expired on buildMPTY Id = %d", invoke_id);

               for(call_session= 0; call_session < 2; call_session++)
               {
                   for (index=0; index < Multicall_Status[call_session].id_index ; index++) /* The breaking condition of this loop determines the fate of next if condition in FEATURE DUAL SIM */
                   {
                       s_respond_to_event =
                           MN_manage_CC_auxiliary_state(Multicall_Status[call_session].connection_id[index], 
                                                        CCE_BUILD_MPTY_REJ);
                   }
#if defined(FEATURE_DUAL_SIM )
                   if(MNCNM_IS_SINGLE_STACK)
                   {
                     if((s_respond_to_event == TRUE) && (index > 0)) /* Check that s_respond_to_event got a fresh value and is TRUE */
                     {
                       index = index - 1; /* to compensate for last increment of index in above for loop */
                       if((index < sizeof(Multicall_Status[call_session].connection_id)) && 
                             (mn_call_information_p[Multicall_Status[call_session].connection_id[index]] != NULL))
                       {
                          as_id = mn_call_information_p[Multicall_Status[call_session].connection_id[index]]->as_id;
                       }
                     }
                   }
#endif
               }

               if(s_respond_to_event == TRUE)
               {
                  cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                  cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

                  MN_put_cm_manage_calls_conf( FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                            as_id,   
#endif 
                                            cm_ss_error);
               }
               else
               {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: CC_BUILD_MPTY_REJ event ignored");
               }

               break;
            }

            case splitMPTY:
            {
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN_SS_TIMER expired on splitMPTY Id = %d ", invoke_id);

               for (call_session=0; call_session < 2; call_session++)
               {
                   for (index=0; index < Multicall_Status[call_session].id_index ; index++)
                   {
                       s_respond_to_event =
                           MN_manage_CC_auxiliary_state(Multicall_Status[call_session].connection_id[index], 
                                                    CCE_SPLIT_MPTY_REJ);
                   }
#if defined(FEATURE_DUAL_SIM )
                   if(MNCNM_IS_SINGLE_STACK)
                   {
                     if((s_respond_to_event == TRUE) && (index > 0)) /* Check that s_respond_to_event got a fresh value and is TRUE */
                     {
                       index = index - 1; /* to compensate for last increment of index in above for loop */
                       if((index < sizeof(Multicall_Status[call_session].connection_id)) && 
                             (mn_call_information_p[Multicall_Status[call_session].connection_id[index]] != NULL))
                       {							 
                         as_id = mn_call_information_p[Multicall_Status[call_session].connection_id[index]]->as_id;
                       }
                     }
                   }
#endif
               }

               if(s_respond_to_event == TRUE)
               {
                  cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                  cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;

                  MN_put_cm_manage_calls_conf( FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                            as_id, 
#endif 
                                            cm_ss_error);
               }
               else
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: CC_SPLIT_MPTY_REJ event ignored");
               }
               break;
            }

            case registerSS:
            case eraseSS:
            case activateSS:
            case deactivateSS:
            case interrogateSS:
            case registerPassword:
            case processUnstructuredSS_Data:
            case processUnstructuredSS_Request:
#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
            case lcs_MOLR:
#endif
#ifdef FEATURE_CCBS
            case eraseCCEntry:
#endif /* FEATURE_CCBS */
            {

               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN_SS_TIMER expired on Call-Indepdent Id = %d ", invoke_id);

               /* Tell cm about the timeout */

               cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
               cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;
               cm_ss_ref = ss_invoke_data[invoke_id].cm_ss_ref;

               MN_select_ss_error(cm_ss_ref, 
                                  ss_code, 
#if defined(FEATURE_DUAL_SIM )
                                  ss_transaction_information[connection_id].as_id,
#endif
                                  cm_ss_error);

               /* Close the connection */

               MN_put_MNSS_END_REQ(connection_id, &facility_data);
               break;
            }

            default:
            {
                MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unknown ss_code event %d for MN SS timer expiration connection_id %d",
                ss_code, invoke_id);
            }
         }/* end switch(ss_code)*/

      } /* SS Timer */
      /* Possible SMS timeout */
      else if (message_ptr->timer_id == (MN_SMS_TIMER + TRAM_TIMER_OFFSET))  /* TRAM */
      {
         /* The MN_SMS_TIMER + TRAM timer has expired */
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= TRAM timer expiration");

         return_data = MN_control_SMS_RL(message_ptr->data, sms_TRAM_timer_expiry);

         if (return_data.success == FALSE)
         {
             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: SMS RL in wrong state for TRAM timer expiry");
         }
      }
      else if (message_ptr->timer_id == (MN_SMS_TIMER + TLC1M_TIMER_OFFSET))
      {
         /* The MN_SMS_TIMER + TLC1M timer has expired */
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= TLC1M timer expiration");

         /* update the link status */
         sms_link_control.status = SMS_LINK_CONTROL_INACTIVE;

         /* update the link control mode */
         sms_link_control.mode = ((sms_link_control.mode == SMS_LINK_CONTROL_ENABLED_ONE) ? 
                                   SMS_LINK_CONTROL_DISABLED : sms_link_control.mode);
         
         /* send MMCNM_REL_REQ */
         (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE
#if defined(FEATURE_DUAL_SIM )
                                             , mn_as_id
#endif
                                            );

         /* send LINK_CONTROL_REPORT */
         MN_put_uasms_link_control_report_ind(
                           sms_link_control.mode, 
#if defined(FEATURE_DUAL_SIM )
                           sm_rl_transaction_information[sms_link_control.connection_id].as_id,
#endif
                           SMS_LINK_CONTROL_TIMEOUT);

      }
      else if (message_ptr->timer_id == (MN_SMS_TIMER + TR1M_TIMER_OFFSET) )
      {
         connection_id = (connection_id_T)message_ptr->data;

         /* The TR1M timer has expired */
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= TR1M timer expiration for SMR-%d",connection_id);

         return_data = MN_control_SMS_RL(message_ptr->data, sms_TR1M_timer_expiry);

         if (return_data.success == FALSE)
         {
             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: SMS RL in wrong state for TR1M timer expiry");
         }
         else
         {
             if (return_data.sm_rl_report == TRUE)
             {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                 mmcc_domain_name_T domain;
#endif
                 sm_data_ptr = &(sm_rl_transaction_information[message_ptr->data]);

                 SM_RL_REPORT_IND.short_message_identifier = sm_data_ptr->short_message_identifier;
#if defined(FEATURE_DUAL_SIM )
                 if(MNCNM_IS_SINGLE_STACK)
                 {
                   SM_RL_REPORT_IND.as_id = sm_data_ptr->as_id;
                 }
#ifdef FEATURE_DUAL_ACTIVE
                 else
                 {
                   SM_RL_REPORT_IND.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                 }
#endif
#endif
                 SM_RL_REPORT_IND.error_status = SMR_ERROR_TR1M_TIMEOUT;
                 SM_RL_REPORT_IND.cause_value = 0;
                 SM_RL_REPORT_IND.diagnostics_present = FALSE;
                 SM_RL_REPORT_IND.tpdu_length = 0;
                 /*
                  * Put the domain as the one seen in message set
                  */
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                 domain = (sms_transaction_information[connection_id].pref_rl_domain == MS_CC_MM)? MMCC_SMS_CS : MMCC_SMS_PS;
                 SM_RL_REPORT_IND.current_domain = domain;
                 SM_RL_REPORT_IND.permanent_error = 0;
#endif
                 SM_RL_REPORT_IND.ims_cause_value = (word)0;
                 MN_put_uasms_sm_report_ind(&SM_RL_REPORT_IND);

             }
         }
      }
      else if (message_ptr->timer_id == (MN_SMS_TIMER + TR2M_TIMER_OFFSET) )
      {

          connection_id = (connection_id_T)message_ptr->data;

          /* The TR2M Timer has expired */
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= TR2M timer expiration for SMR-%d",connection_id);

          return_data = MN_control_SMS_RL(message_ptr->data, sms_TR2M_timer_expiry);

          if (return_data.success == FALSE)
          {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: SMS RL in wrong state for TR2M timer expiry");

          }
          else
          {
              if (return_data.sm_rl_report == TRUE)
              {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                  mmcc_domain_name_T domain;
#endif
                  sm_data_ptr = &(sm_rl_transaction_information[message_ptr->data]);

                  /* Fix for CR 22313 */
                  if (connection_id > MAXNO_SMS_MO_TRANSACTIONS) 
                  {    
                    /* 
                     * MT transaction: Since the short_message_identifier field in the
                     * SM_RL_REPORT_IND is unused, encode the connection_id in it, as
                     * follows. The encoding is 0xXXXN, where N is the connection_id and
                     * 0xXXX0 is MN_WMS_MT_MESSAGE_IDENTIFIER_BASE
                     */

                    SM_RL_REPORT_IND.short_message_identifier = MN_WMS_MT_MESSAGE_IDENTIFIER_BASE | connection_id;
                    MSG_LOW_DS_2(MN_SUB,"=MNCNM= SMR-%d TR2M timeout; Setting short_message_identifier to %d",
                         connection_id, SM_RL_REPORT_IND.short_message_identifier);
                    mt_sms_transport_type = NAS_MSG_TRANSPORT_TYPE_MAX;
                  }
                  else
                  {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR:TR2M timer expiry can happen only during a MT transaction");
                  }
#if defined(FEATURE_DUAL_SIM )
                  if(MNCNM_IS_SINGLE_STACK)
                  {
                    SM_RL_REPORT_IND.as_id = sm_data_ptr->as_id;
                  }
#ifdef FEATURE_DUAL_ACTIVE
                 else
                 {
                   SM_RL_REPORT_IND.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                 }
#endif
#endif
                  SM_RL_REPORT_IND.error_status = SMR_ERROR_TR2M_TIMEOUT;
                  SM_RL_REPORT_IND.cause_value = 0;
                  SM_RL_REPORT_IND.diagnostics_present = FALSE;
                  SM_RL_REPORT_IND.tpdu_length = 0;

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                  domain = (sms_transaction_information[connection_id].pref_rl_domain == MS_CC_MM)? MMCC_SMS_CS : MMCC_SMS_PS;
                  SM_RL_REPORT_IND.current_domain = domain;
                  SM_RL_REPORT_IND.permanent_error = 0;
#endif
                  SM_RL_REPORT_IND.ims_cause_value = (word)0;
                  MN_put_uasms_sm_report_ind(&SM_RL_REPORT_IND);
              }
          }
      }
      else
      {
          MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Invalid timer id %d",message_ptr->timer_id);
      }
   }
   else
   {
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Unexpected GSDI message %d",message_ptr->IMH.message_id);
   }
}


/*
 *   Function name : get_timer_name()
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Utility function to get timer name string based on its ID. 
 *  
 *  Parameters:
 *  -----------
 *  IN connection_id
 *     message_IEI
 *
 *  Returns:
 *     void
 *
 */
char * get_timer_name(timer_id_T timer_id)
{
   char * pstr;

   switch (timer_id)
   {

      case TR1M:
          pstr = "TR1M";
          break;
     
      case TR2M:
          pstr = "TR2M";
          break;

      case TRAM:
          pstr = "TRAM";
          break;

      case TLC1M:
          pstr = "TLC1M";
          break;
     
      default:
          pstr = "UNKNOWN_TIMER";
          break;
   }
   return pstr;
}

