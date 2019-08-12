/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_route_sms_msgs.c_v   1.9   17 Jun 2002 17:14:14   rgurumoo  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_route_sms_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/8/01      CD     Changed calling sequence for MN_handle_mmi_check_for_smma_req
                   to avoid compile warning 

5/10/01     CD     Removed unnecessary header file includes

5/24/01     CD     Added #define RAM_SMS_STORAGE to case MMI_ERASE_RAM_SM_REQ

5/25/01     CD     Removed ms.h and cause.h since they were not being used

                   Changed parameter SMS_to_MN_primitive to SMS_primitive in MN_route_SMS_to_MN_primitives
                   as it was also being declared globally in mn_cc.h

6/7/01      CD     Changed function name of MN_route_SMS_MMI_to_MN_primitives()
                   to MN_route_UASMS_to_MN_primitives()

                   In MN_route_UASMS_to_MN_primitives(), removed switch cases 
                   MMI_SUBMIT_SM_FROM_SIM_REQ, MMI_READ_SHORT_MESSAGE_REQ, 
                   MMI_ERASE_RAM_SM_REQ, and MMI_CONFIGURE_SM_ROUTING

                   In MN_route_UASMS_to_MN_primitives() added SMR functionality to the
                   switch cases UASMS_SUBMIT_SM_REQ, UASMS_SM_COMMAND_REQ,
                   UASMS_MEMORY_AVAIL_NOTIFICATION_REQ, UASMS_ABORT_SM_REQ

                   In MN_route_UASMS_to_MN_primitives() add switch case for
                   UASMS_SM_REPORT_REQ, which implies a call to MN_get_RP_REPORT()

7/16/01     CD     In case UASMS_ABORT_SM_REQ, added check of the MN_control_SM-RL 
                   return value

                   In case UASMS_MEMORY_AVAIL_NOTIFICATION_REQ, corected implementation 
                   when the abort fag is set. 

7/17/01     CD     Changed MN_process_SM_RL_MEM_AVAIL_REQ calling sequence due to 
                   function protoype change

7/18/01     CD     Removed duplicate external declaration of sm_rl_transaction_information

8/10/01     CD     Removed unnecessary queue.h

8/13/01     CD     Added check for boolean sm_rl_report in case UASMS_ABORT_SM_REQ

08/16/01   AB      Renamed MMI references to CM.

9/10/01     CD     Corrected formatting of messages to avoid use of %s

9/27/01     CD     In MN_route_UASMS_to_MN_primitives, corrected use of SM_RL_REPORT_IND_T 
                   and SM_RL_DATA_REQ_T due to changes in structure dedinition.  
                   Provided enum for error_status, and fixed use of cause_value

1/29/02     RG     Pass the short_message_identifier received in the SM_RL_MEM_AVAIL_REQ 
                   to the MN_process_SM_RL_MEM_AVAIL_REQ routine that is called to process 
                   the SM_RL_MEM_AVAIL_REQ message. Initialized the SM_RL_REPORT_IND.tpdu 
                   arrays to '\0'.

8/12/04     AB     Added supported for the SMS link control primitives.

04/09/07    RD     SMS optimization changes are incorporated

===========================================================================*/


/* includes */

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "nasutils.h"
#include "err.h"
#include <string.h>
#include "cnm.h"

/*
 *   Function name : MN_route_UASMS_to_MN_primitives
 *   ---------------
 *
 *   Description:  Route SMS primitives from UASMS to the correct MN function
 *  
 * 
 *  
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


VOID_FUNC   MN_route_UASMS_to_MN_primitives(IMH_T          *uasms_sms_req)

{

   /* Locals */
   SM_RL_DATA_REQ_T                *SM_RL_DATA_REQ;
   SM_RL_MEMORY_AVAILABLE_REQ_T    *SM_RL_MEM_AVAIL_REQ;
   SM_RL_REPORT_REQ_T              *SM_RL_REPORT_REQ;
   SM_RL_LINK_CONTROL_REQ_T        *SM_RL_LINK_CONTROL_REQ;
   SM_RL_UPDATE_TIMERF_T           *SM_RL_UPDATE_TIMERF;
   SM_RL_REPORT_IND_T              SM_RL_REPORT_IND;
   connection_id_T                 connection_id;
   control_sm_rl_return_data_T     smr_return_data;

   switch(uasms_sms_req->message_id)
   {
   case UASMS_SM_DATA_REQ:
      {
         /*  For MO SMS requests (UASMS_SUBMIT_SM_REQ)  
          *  or to send a command to the SC (UASMS_SM_COMMAND_REQ)
          *  just forward the SMRL primitive for processing
          */
         SM_RL_DATA_REQ = (SM_RL_DATA_REQ_T *) uasms_sms_req;
#if defined(FEATURE_DUAL_SIM )
         if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_sms_as_id(SM_RL_DATA_REQ->as_id, SM_RL_DATA_REQ->short_message_identifier, TRUE)))
#endif
         {
           MN_process_SM_RL_DATA_REQ(SM_RL_DATA_REQ);
         }
         break;
      }

   case UASMS_MEMORY_AVAIL_NOTIFICATION_REQ:
      {
         /* For memory available requests (UASMS_MEMORY_AVAIL_NOTIFICATION_REQ),
          * If the abort flag is set to FALSE, forward primitive for processing.
          * If the abort flag is set to TRUE, process the requests for 
          * transactions in the WAIT FOR RETRANS  or WAIT FOR RP_ACK states
          */
         SM_RL_MEM_AVAIL_REQ = (SM_RL_MEMORY_AVAILABLE_REQ_T *) uasms_sms_req;
#if defined(FEATURE_DUAL_SIM ) 
         if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_sms_as_id(SM_RL_MEM_AVAIL_REQ->as_id, SM_RL_MEM_AVAIL_REQ->short_message_identifier, TRUE)))
#endif
         {
           if (SM_RL_MEM_AVAIL_REQ->abort == FALSE)
           {
               MN_process_SM_RL_MEM_AVAIL_REQ(SM_RL_MEM_AVAIL_REQ->short_message_identifier
#if defined(FEATURE_DUAL_SIM )
                                     ,SM_RL_MEM_AVAIL_REQ->as_id
#endif
                                     ,SM_RL_MEM_AVAIL_REQ->sms_transport_type
                                     );
           }
           else /* abort is set to TRUE */
           {
               for (connection_id = 0; connection_id < MAXNO_SMS_TRANSACTIONS; connection_id++)
               {
                   smr_return_data.success = TRUE; 
  
                   if(sm_rl_transaction_information[connection_id].rl_state == WAIT_FOR_RETRANS_TIMER)
                   {
                       smr_return_data = MN_control_SMS_RL(connection_id, s_mo_smma_abort_req);
  
                       /* Send a Report back to UASMS*/
                       SM_RL_REPORT_IND.short_message_identifier = 
                           sm_rl_transaction_information[connection_id].short_message_identifier;
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
                       SM_RL_REPORT_IND.error_status = NO_ERROR_SMMA_ABORT_SENT;
                       SM_RL_REPORT_IND.cause_value = 0;
                       SM_RL_REPORT_IND.diagnostics_present = FALSE;
                       SM_RL_REPORT_IND.tpdu_length = 0;
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                       SM_RL_REPORT_IND.permanent_error = 0;
                       SM_RL_REPORT_IND.current_domain = MMCC_SMS_CS_PS;
#endif
                       memset(SM_RL_REPORT_IND.tpdu,'\0',sizeof(SM_RL_REPORT_IND.tpdu));
                       SM_RL_REPORT_IND.ims_cause_value = (word)0;
                       MN_put_uasms_sm_report_ind(&SM_RL_REPORT_IND);
                   }
                   else if (sm_rl_transaction_information[connection_id].rl_state == WAIT_FOR_RP_ACK)
                   {
                       smr_return_data = MN_control_SMS_RL(connection_id, s_mo_smma_abort_req);
                   }
                   else
                   {
#ifdef FEATURE_MN_UNIT_TEST
                       MSG_HIGH_DS_2(MN_SUB,"=MNCNM= UASMS MEM AVAIL NOTIF ABORT for SMR-%d in wrong state %s",
                                 connection_id,
                                 show_rl_state(sm_rl_transaction_information[connection_id].rl_state));
#else
                       MSG_HIGH_DS_2(MN_SUB,"=MNCNM= UASMS MEM AVAIL NOTIF ABORT for SMR-%d in wrong state %d",
                                 connection_id,
                                 sm_rl_transaction_information[connection_id].rl_state);
#endif
                   }
  
                   if (smr_return_data.success != TRUE)
                   {
#ifdef FEATURE_MN_UNIT_TEST
                       MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Problem in aborting MEM AVAIL REQ for SMR-%d in state %s ",
                                 connection_id,
                                 show_rl_state(sm_rl_transaction_information[connection_id].rl_state));
#else
                       MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Problem in aborting MEM AVAIL REQ for SMR-%d in state %d ",
                                 connection_id,
                                 sm_rl_transaction_information[connection_id].rl_state);
#endif
                   }
               }
           }
         }
         break;
      }

   case UASMS_ABORT_SM_REQ:
      {
#if defined(FEATURE_DUAL_SIM ) 
         if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_sms_as_id(((SM_RL_ABORT_SM_REQ_T *)uasms_sms_req)->as_id, 0, FALSE)))
#endif
         {
           /* For abort requests, forward the primitive to all transactions */
           for (connection_id = 0; connection_id < MAXNO_SMS_TRANSACTIONS; connection_id++)
           {
               smr_return_data = MN_control_SMS_RL(connection_id, s_mo_abort_req);
  
               if (smr_return_data.success != TRUE)
               {
#ifdef FEATURE_MN_UNIT_TEST
                   MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Problem in aborting SMR-%d in state %s ",
                            connection_id,
                            show_rl_state(sm_rl_transaction_information[connection_id].rl_state));
#else
                   MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Problem in aborting SMR-%d in state %d ",
                            connection_id,
                            sm_rl_transaction_information[connection_id].rl_state);
#endif
               }
               if (smr_return_data.sm_rl_report == TRUE)
               {
                  /* Send a Report back to UASMS*/
                  SM_RL_REPORT_IND.short_message_identifier = 
                  sm_rl_transaction_information[connection_id].short_message_identifier;
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
                  SM_RL_REPORT_IND.error_status = NO_ERROR_ABORT_SM_COMPLETE;
                  SM_RL_REPORT_IND.cause_value = 0;
                  SM_RL_REPORT_IND.diagnostics_present = FALSE;
                  SM_RL_REPORT_IND.tpdu_length = 0;
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                  SM_RL_REPORT_IND.permanent_error = 0;
                  SM_RL_REPORT_IND.current_domain = MMCC_SMS_CS_PS;
#endif
                  memset(SM_RL_REPORT_IND.tpdu,'\0',sizeof(SM_RL_REPORT_IND.tpdu));
                  SM_RL_REPORT_IND.ims_cause_value = (word)0;
                  MN_put_uasms_sm_report_ind(&SM_RL_REPORT_IND);
               }
           }
         }
         break;
      }

   case UASMS_SM_REPORT_REQ:
      {
          /* For SM Report request, forward the primitive for processing */
         SM_RL_REPORT_REQ = (SM_RL_REPORT_REQ_T *) uasms_sms_req;
#if defined(FEATURE_DUAL_SIM )
         if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_sms_as_id(SM_RL_REPORT_REQ->as_id, SM_RL_REPORT_REQ->connection_id, FALSE)))
#endif
         {
            MN_get_rp_report(SM_RL_REPORT_REQ);
         }
         break;
      }

   case UASMS_SM_LINK_CONTROL_REQ:
      {
          /* For SM Link Control request, forward the primitive for processing */
         SM_RL_LINK_CONTROL_REQ = (SM_RL_LINK_CONTROL_REQ_T *) uasms_sms_req;
#if defined(FEATURE_DUAL_SIM )
         if(MNCNM_IS_SINGLE_STACK)
         {
           if(MN_validate_sms_as_id(SM_RL_LINK_CONTROL_REQ->as_id, 0, FALSE))
           {
             MN_process_SM_RL_LINK_CONTRL_REQ(SM_RL_LINK_CONTROL_REQ);
           }
           else
           {
             MN_put_uasms_link_control_report_ind(
                               SM_RL_LINK_CONTROL_REQ->mode, 
                               SM_RL_LINK_CONTROL_REQ->as_id, 
                               SMS_LINK_CONTROL_RADIO_UNAVAILABLE);
           }
         }
         else
#endif
         {
           MN_process_SM_RL_LINK_CONTRL_REQ(SM_RL_LINK_CONTROL_REQ);
         }

         break;
     
      }

   case UASMS_SM_UPDATE_TIMERF:
      {
        SM_RL_UPDATE_TIMERF = (SM_RL_UPDATE_TIMERF_T *)uasms_sms_req;
        /* For MO SMS over IMS, Timer f value will be sent to NAS 
                * when the SMS is over IMS, NAS has to start Timer f instead of TR1M timer */           
        {  
           sip_config_timer_f = (SM_RL_UPDATE_TIMERF -> timerf_value)+1000;  //To avoid race conditions, increase timerf value by 1sec
           MSG_HIGH_DS_1(MN_SUB," =MNCNM= Received timer f value from WMS = %d \n" ,sip_config_timer_f );
        }
        break;
      }

   default:
      {
         ERR("ERROR: Unexpected UASMS->MN SMS message received",0,0,0); 
         break;
      }

   }

}


/*
 *   Function name : MN_route_SMS_to_MN_primitives
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function generates the cm_mo_call_conf message to the 
 *  mmi.
 *  
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


VOID_FUNC   MN_route_SMS_to_MN_primitives(IMH_T *SMS_primitives)

{

   /* Locals */

   switch(SMS_primitives->message_id)
   {

   case MNSMS_DATA_IND:

      {

         MN_handle_MNSMS_DATA_IND(SMS_primitives);
         break;
      }

   case MNSMS_EST_IND:

      {

         MN_handle_MNSMS_EST_IND(SMS_primitives);
         break;
      }

   case MNSMS_ERROR_IND:

      {

         MN_handle_MNSMS_ERROR_IND(SMS_primitives);
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

         break;
      }

   case MNSMS_CPACK_IND:

      {

         MN_handle_MNSMS_CPACK_IND(SMS_primitives);
         break;
      }
    

   default:

      {
         ERR("ERROR: Unexpected SMS->MN message received",0,0,0); 

      }
   }

}
