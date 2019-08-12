/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_process_smrl_msgs.c_v   1.7   17 Jun 2002 17:10:08   rgurumoo  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_process_smrl_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/10/01     CD     Removed unnecessary header file includes

5/24/01     CD     Removed #include for cause.h and ms.h as they are not used

6/8/01      CD     In MN_process_SM_RL_DATA_REQ and MN_process_SM_RL_MEM_AVAIL_REQ 
                   changed call to MN_put_cm_submit_sm_conf by 
                   MN_put_uasms_sm_report_ind

7/17/01     CD     Changed MN_process_SM_RL_MEM_AVAIL_REQ function definition

7/18/01     CD     Removed duplicate external declaration of sm_rl_transaction_information
                   Removed external declaration of mo_rpdu_store

8/10/01     CD     Removed unnecessary queue.h

08/16/01    AB     Renamed MMI references to CM.

09/27/01    CD     In MN_process_SM_RL_DATA_REQ and MN_process_SM_RL_MEM_AVAIL_REQ, 
                   corrected use of SM_RL_REPORT_IND_T and SM_RL_DATA_REQ_T due to 
                   changes in structure dedinition.  Provided enum for error_status, 
                   and fixed use of cause_value

01/29/02    RG     Store the short_message_identifier received as a part of the 
                   SM_RL_MEMORY_AVAILABILITY_REQ_T in the SM transaction information, 
                   and reflect it back in the SM_RL_REPORT_IND for the 
                   SM_RL_MEMORY_AVAILABILITY_REQ.
                   
04/09/07    RD     SMS optimization changes are incorporated 

07/04/11    AM     Putting the range check on connection_id

07/17/11    AM     Putting range check on rpda_length & tpdu_length to avoid overflow

07/20/11    AM     Fixing compilation issue.

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
#include "err.h"
#include <stringl/stringl.h>
#include "nasutils.h"



extern VOID_FUNC MN_put_MNIMS_MO_RPDU(void);


/*
 * Function name :  MN_process_SM_RL_DATA_REQ
 * -------------
 *
 * Description :  Creates an RPDU for transmission to the network
 * -----------    OR sends an error message to TL
 *
 *
 *
 *
 *
 *
 * Parameters: SM_RL_DATA_REQ
 * ----------  
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_process_SM_RL_DATA_REQ(SM_RL_DATA_REQ_T *message_ptr)

{
   connection_id_T                  connection_id;
   sm_rl_transaction_information_T *sm_data_ptr = NULL;
   control_sm_rl_return_data_T      return_data;
   int                              rpda_index;
   int                              tpdu_index;
   int                              index;
   byte                             rpda_length,rpdu[RPDU_SIZE], tpda_length;
   SM_RL_REPORT_IND_T               SM_RL_REPORT_IND;
   boolean                          success = TRUE;


   /* In case of SMSoIMSoLTE, there is no CP-Layer and hence no buffereing. So check if
       * there is already a MO SMS Transaction ongoing. If yes, then return error to UASMS.
       */

   MSG_HIGH_DS_2(MN_SUB,"=MNCNM= transport type recd = %d, current transport type is set at %d",message_ptr->sms_transport_type,mo_sms_transport_type);

   /* Compare, validate and Set NAS mo_sms_transport_type 
    * If it is not already set, set it to the value conveyed by 
    * WMS in MO SMS request.
    * If it is already set, compare the set value against the value
    * given by WMS.
    * If it matches, process the SMS.
    * If it doesn't match, send fail report to WMS */

   if(mo_sms_transport_type == NAS_MSG_TRANSPORT_TYPE_MAX)
   {
     if(message_ptr->sms_transport_type != NAS_MSG_TRANSPORT_TYPE_MAX)
     {
       mo_sms_transport_type = message_ptr->sms_transport_type;
     }
     else
     {
       mo_sms_transport_type = NAS_MSG_TRANSPORT_TYPE_GW;
     }
   }
   else if(mo_sms_transport_type != message_ptr->sms_transport_type)
   {
     success = FALSE;  /* received transport type doesn't match with already set value */
   }

   MSG_HIGH_DS_3(MN_SUB,"=MNCNM= transport type recd = %d, transport type is now set at %d, success = %d",message_ptr->sms_transport_type,mo_sms_transport_type,success);

   if(success == TRUE)
   {
      if(mo_sms_transport_type == NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS)
      {
        connection_id_T  conn_id;

        // Check if already an MO transaction is active on IMS Transport
        
        for (conn_id = 0;conn_id <= MAX_MO_CONNECTION_ID;conn_id++)
        {
          if(sm_rl_transaction_information[conn_id].rl_state != IDLE)
          {
            break;
          }
        }
        
        if( conn_id <= MAX_MO_CONNECTION_ID)
        {
          /* Set up the SM_RL_REPORT_IND message & send to RL */
   
          SM_RL_REPORT_IND.short_message_identifier = 
                        message_ptr->short_message_identifier;
          SM_RL_REPORT_IND.error_status = SMR_ERROR_NO_SM_RESOURCES;
          SM_RL_REPORT_IND.cause_value = 0;
          SM_RL_REPORT_IND.diagnostics_present = FALSE;
          SM_RL_REPORT_IND.diagnostics = 0;
   #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
          SM_RL_REPORT_IND.permanent_error = 0;
          SM_RL_REPORT_IND.current_domain = MMCC_SMS_CS_PS;
   #endif
          SM_RL_REPORT_IND.ims_cause_value = (word)0; 
          /*
           * This function is called during MO SMS. Here, message set doesnt correspond to 
           * domain. So, set the domain as some default one
           */
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN MO SMS transaction is progress for SMR-%d",conn_id);
   
          MN_put_uasms_sm_report_ind(&SM_RL_REPORT_IND);
          return;
   
        }     
   
        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN NO MO SMS in Progress");
        
      }
   
      /* Try and get a free connection id */
      connection_id = MN_get_new_sms_connection_id();
   
      if (connection_id <=  MAX_MO_CONNECTION_ID) /* error - no conn_ids available */
      {
         /* create and send RL message to network */
         sm_data_ptr = &(sm_rl_transaction_information[connection_id]);
         sm_data_ptr->short_message_identifier = message_ptr->short_message_identifier;
         sm_data_ptr->as_id                    = message_ptr->as_id;

         /* Create the RPDU */
   
         index = 0;
         rpdu[index++] = RP_DATA_MS;
         rpdu[index++] = 0x00;        /* message reference */
         rpdu[index++] = 0x00;        /* RP_OA length - OA not present */
   
         rpda_length = message_ptr->RP_DA.length;
   
         if(rpda_length > MIN(CM_CALLED_PARTY_BCD_NO_LENGTH, (RPDU_SIZE - index - 1)) )
         {
           MSG_ERROR_DS_1(MN_SUB,"=MNCNM= rpda_length exceeds ?%d",rpda_length);
           rpda_length = MIN(CM_CALLED_PARTY_BCD_NO_LENGTH, (RPDU_SIZE - index - 1));
         }
   
   
         rpdu[index++] = rpda_length;
         rpdu[index++] = message_ptr->RP_DA.data[0];
   
         for (rpda_index = 1;
             rpda_index < (rpda_length);
             rpda_index++, index++)
         {
            rpdu[index] = message_ptr->RP_DA.data[rpda_index];
         }
         
         if(message_ptr->tpdu_length > MIN(TPDU_SIZE, RPDU_SIZE - index - 1))
         {
           MSG_ERROR_DS_1(MN_SUB,"=MNCNM= tpdu_length exceeds ?%d",message_ptr->tpdu_length);
           tpda_length = MIN(TPDU_SIZE, RPDU_SIZE - index - 1);
         }
         else
         {
           tpda_length = message_ptr->tpdu_length;
         }
   
         if(index >= (RPDU_SIZE -1))
         {
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and bufferover flow");
         }
         else
         {
            rpdu[index++] = tpda_length;
         }
   
         for (tpdu_index = 0;
              (tpdu_index < tpda_length && index < RPDU_SIZE);
               tpdu_index++, index++)
         {
            rpdu[index] = message_ptr->tpdu[tpdu_index];
         }
   
         /* Load the output data in the store */
         /* Klocwork Fix */
         if(index >= 0 && index <= RPDU_SIZE)
         {
            memscpy(mo_rpdu_store.rpdu,
                    sizeof(mo_rpdu_store.rpdu),
                    rpdu, 
                    (size_t)index);
            mo_rpdu_store.rpdu_length = (byte)index;
         }
         else
         {
            ERR("ERROR: Buffer Overflow",0,0,0); 
         }
   
         mo_rpdu_store.connection_id = connection_id;
   
         /* Store the Destination Address of the MO SMS here */
         mo_sms_destination_addr = message_ptr->RP_DA;
         return_data = MN_control_SMS_RL( connection_id, s_mo_sms_req );
   
         if (return_data.success == FALSE)
         {
            ERR("ERROR: SMS_RL in wrong state",0,0,0);
   
         }
   #if defined(FEATURE_DUAL_SIM )
         else if(MNCNM_IS_SINGLE_STACK)
         {
         /* store only when FEATURE_3GPP_SMS_OVER_IMS is not defined in Mainline */
            mn_as_id = message_ptr->as_id;
            MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
         }
   #endif
   
      }
      else
      {
         success = FALSE;
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Can't allocate valid connection ID, success = FALSE");
      }
   
   }

   if(success == FALSE)
   {
      /* Set up the SM_RL_REPORT_IND message & send to RL */

      SM_RL_REPORT_IND.short_message_identifier = 
          message_ptr->short_message_identifier;
      SM_RL_REPORT_IND.error_status             = SMR_ERROR_NO_SM_RESOURCES;
      SM_RL_REPORT_IND.cause_value              = 0;
      SM_RL_REPORT_IND.diagnostics_present      = FALSE;
      SM_RL_REPORT_IND.diagnostics              = 0;
#if defined(FEATURE_DUAL_SIM )
      if(MNCNM_IS_SINGLE_STACK)
      {
      SM_RL_REPORT_IND.as_id                    = message_ptr->as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE

      else

      {

        SM_RL_REPORT_IND.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;

      }

#endif
#endif
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
      SM_RL_REPORT_IND.permanent_error = 0;
      SM_RL_REPORT_IND.current_domain = MMCC_SMS_CS_PS;
#endif
      SM_RL_REPORT_IND.ims_cause_value = (word)0;
      /*
       * This function is called during MO SMS. Here, message set doesnt correspond to 
       * domain. So, set the domain as some default one
       */

      MN_put_uasms_sm_report_ind(&SM_RL_REPORT_IND);

   }
      
}



/*
 * Function name :  MN_process_SM_RL_MEM_AVAIL_REQ
 * -------------
 *
 * Description :  Creates an RPDU for transmission to the network
 * -----------    OR sends an error message to TL
 *
 *
 *
 *
 *
 *
 * Parameters: none
 * ----------  
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_process_SM_RL_MEM_AVAIL_REQ(
   word short_message_identifier
#if defined(FEATURE_DUAL_SIM )
  ,sys_modem_as_id_e_type  as_id
#endif
  ,nas_msg_transport_type_e_type    sms_transport_type
)
{
   byte                             rpdu[3];
   connection_id_T                  connection_id;
   sm_rl_transaction_information_T *sm_data_ptr = NULL;
   control_sm_rl_return_data_T      return_data;
   int                              index;
   SM_RL_REPORT_IND_T               SM_RL_REPORT_IND;
   boolean                          success = TRUE;


   MSG_HIGH_DS_2(MN_SUB,"=MNCNM= transport type recd = %d, current transport type is set at %d", sms_transport_type, mo_sms_transport_type);
 
   if(mo_sms_transport_type == NAS_MSG_TRANSPORT_TYPE_MAX)
   {
      if(sms_transport_type != NAS_MSG_TRANSPORT_TYPE_MAX)
      {
         mo_sms_transport_type = sms_transport_type;
      }
      else
      {
         mo_sms_transport_type = NAS_MSG_TRANSPORT_TYPE_GW;
      }
   }
   else if(mo_sms_transport_type != sms_transport_type)
   {
      success = FALSE;  /* received transport type doesn't match with already set value */
   }

   if(TRUE == success && NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == mo_sms_transport_type)
   {
     connection_id_T  conn_id;

     // Check if already an MO transaction is active on IMS Transport
     for (conn_id = 0;conn_id <= MAX_MO_CONNECTION_ID;conn_id++)
     {
       if(sm_rl_transaction_information[conn_id].rl_state != IDLE)
       {
         break;
       }
     }
      
     if( conn_id <= MAX_MO_CONNECTION_ID)
     {
       success = FALSE;
     }
   }

   connection_id = MN_get_new_sms_connection_id();
   
   if (TRUE == success && connection_id <=  MAX_MO_CONNECTION_ID)  /* error condition */
   {
      /* Send RL message to the network */
      sm_data_ptr = &(sm_rl_transaction_information[connection_id]);

      /* Added "word short_message_identifier" to the
      SM_RL_MEMORY_AVAILABILITY_REQ_T in order to
      correlate the SM_RL_REPORT_IND's recevied 
      with the memory availablity requests. Filling
      in the sm_rl_transaction_information with the
      short_message_identifier received.
      */
      sm_data_ptr->short_message_identifier = short_message_identifier;
#if defined(FEATURE_DUAL_SIM )
      sm_data_ptr->as_id = as_id;
#endif
      return_data = MN_control_SMS_RL( connection_id, s_mo_smma_req );

      if (return_data.success == TRUE)
      {
         /* Create the rpdu */

         index = 0;
         rpdu[index++] = RP_SMMA_MS;
         rpdu[index++] = 0x00;      /* tp message ref */

         /* Load data in to the store */

         memscpy(mo_rpdu_store.rpdu,
             sizeof(mo_rpdu_store.rpdu),
             rpdu,
             (unsigned int)index);
         mo_rpdu_store.rpdu_length = (byte)index;
         mo_rpdu_store.connection_id = connection_id;

         /* create a copy of memory availabe notification
            required in case of retry on expiry of TRAM timer */
         mo_memory_available_rpdu_store.connection_id = connection_id;
         mo_memory_available_rpdu_store.rpdu_length = (byte)index;
         memscpy(mo_memory_available_rpdu_store.rpdu, sizeof(mo_memory_available_rpdu_store.rpdu) ,rpdu, (unsigned int)index);      
#if defined(FEATURE_DUAL_SIM )
         if(MNCNM_IS_SINGLE_STACK)
         {
           mn_as_id = as_id;
           MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
         }
#endif

         if(NAS_MSG_TRANSPORT_TYPE_SMS_OVER_IMS == mo_sms_transport_type)
         {
           mo_sms_destination_addr.present = FALSE;
           MN_put_MNIMS_MO_RPDU();
           /* Start TR1M Timer here only in case of WMS-IMS interface */
           /* Start the MN timer just before sending the RP-DATA */
           MN_start_sms_timer(connection_id,TR1M); /* SMS timer id */
         }
         else
         {
           MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MN WMS-IMS has not registered the CallBack Func");
           MN_put_MNSMS_EST_REQ();
         }
      }
      else
      {
         ERR("ERROR: SMS RL in wrong state for SMMA",0,0,0);
      }
   }

   else
   {
      /* Set up the SM_RL_REPORT_IND message & send to RL */

      SM_RL_REPORT_IND.short_message_identifier =
          short_message_identifier;
      SM_RL_REPORT_IND.error_status = SMR_ERROR_NO_SM_RESOURCES;
      SM_RL_REPORT_IND.cause_value = 0;
      SM_RL_REPORT_IND.diagnostics_present = FALSE;
      SM_RL_REPORT_IND.diagnostics = 0;
#if defined(FEATURE_DUAL_SIM )
      if(MNCNM_IS_SINGLE_STACK)
      {
      SM_RL_REPORT_IND.as_id = as_id;
      }
#ifdef FEATURE_DUAL_ACTIVE

      else

      {

        SM_RL_REPORT_IND.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;

      }

#endif
#endif
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
      SM_RL_REPORT_IND.permanent_error = 0;
      SM_RL_REPORT_IND.current_domain = MMCC_SMS_CS_PS;
#endif
      SM_RL_REPORT_IND.ims_cause_value = (word)0;
      /*
       * This function is called when MEM_AVAIL_REQ arrives. Here, message set doesnt correspond to 
       * domain. So, set the domain as some default one
       */
      MN_put_uasms_sm_report_ind(&SM_RL_REPORT_IND);

   }

}
