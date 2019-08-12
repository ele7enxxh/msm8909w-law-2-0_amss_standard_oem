/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_process_cm_ss_msgs.c_v   1.20   29 May 2002 14:02:24   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_process_cm_ss_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/7/01      CD     Fixed compile warning C4701 - local variable may bef used 
           without being initialized.  In MN_handle_mmi_unstructured_ss_res()
           initialized variable 'components' to NULL

5/7/01      CD     Fixed compile warning C4701 - local variable may be used 
           without being initialized.  In MN_handle_mmi_process_unstructured_ss_data()
           initialized variable 'components' to NULL

5/10/01     CD     Removed unnecessary header file includes

5/24/01     CD     Removed #include for cause.h and ms.h since they are not used

                   Removed redundant declaration of extern ss_transaction_information. It is
                   already declared in mn_ss.h

                   Corrected several lint errors 732 -- loss of sign (int to unsigned int)

                   Corrected call to memcpy in MN_handle_mmi_get_password_res to avoid a data
                   overrun lint error

7/18/01     CD     Removed duplicate external declaration of ss_invoke_data

8/10/01     CD     Removed unnecessary queue.h

08/16/01    AB     Renamed MMI references to CM.

08/30/01    AB     Added debug information for SS Abort command received from CM.

09/10/01    AB     Removed unnessary local/automatic variables. 

09/30/01    AB     Removed nested comments.

10/08/01    AB     Fixed Lint errors.

10/22/01    AB     Changed access to SS state via CNM_get_ss_state() instead of 
                     referencing SS global ss_transaction_information[] variable.

11/02/01    AB     Removed the logic that store ie_ss_password_T element in
                     MN_handle_cm_activate_ss_req(), and MN_handle_cm_eactivate_ss_req()
                     as UI/CM does not support this interface.

12/01/01    AB     Added Guard timer for following SS Call independent operation:
                     registerSS
                     eraseSS
                     activateSS
                     deactivateSS
                     interrogateSS
                     registerPassword
                     processUnstructuredSS_Data
                     processUnstructuredSS_Request.

12/19/01    AB     Moved MN_process_MNCC_FACILITY_IND() from mn_process_cm_cc_msgs.c

01/23/02    AB     Added mn_exp.h includes becuase SS components ERROR tag had been
                      moved to mn_exp.h so that these tag can be used by CM/UI.

01/25/02    CD     Updated copyright
                   Replaced calls to ERR() by MSG_ERR()
                   Replaced IEI by boolean flag
                   Added customer.h  and removed unnecessary mn_exp.h

02/10/02    AB     Filled in default value for CLIR information if none are
                    provided by the network.
                   
02/18/02    AB     Corected to context specific value of the interrogate SS 
                    response value to 0x5, instead of 0x1. 
                    
02/18/02    AB     Fixed MN_handle_interrogate_ss_res() to handle up to 
             MAXNO_FORWARD_TO_NUMBER, 0x9, possible forward to number.

02/20/02    AB     Temporay backed out the previous changes as CM is not ready 
                     to make this interface changes.  

03/06/02    AB     Enabled the previous MN_handle_interrogate_ss_res() fixes.
 
05/06/02    AB     Added handler, MN_handle_cm_release_ss_req, for User to 
                     release the USS transaction. 

05/14/02    AB     In MN_handle_cm_process_unstructured_ss_req(), set the 
                     SS operation based on value received from CM instead 
                     fix value, processUnstructuredSS_REQ.

05/15/02    AB     Undo previous changes in 
                     MN_handle_cm_process_unstructured_ss_req() as
                     SS operation, processUnstructedSS_Data is already
                     handled by a seperate function, 
                     MN_handle_cm_process_unstructured_ss_data().

08/16/02    AB     In MN_handle_cm_unstructured_ss_notify_res(), added
                     more details in the F3 error log messages, and 
                     re-ordered the cheking of the error condition.  Also, 
                     Reduced the local arguments buffer to 16 from 30 stack
                     utilization. 

01/10/03    AB     In MN_handle_cm_mt_call_ss_res(), clear out the invoke
                     entry, ss_invoke_data[] after RETURN_RESULT had been
                     sent to the network.

04/15/03    sbs    Added support for Location Services related SS messages

11/18/03    AB     In MN_handle_cm_process_unstructured_ss_res, release
                     invoke ID so that next network initated USSD would
                     have same invoke ID as previous. 

09/28/04    AB     In MN_handle_cm_lcs_molr_req(), increase the molr_ARG
                     buffer via gs_alloc(100) to the max size of a SS 
                     component.

04/18/05    HS     Added support for FEATURE_CCBS

06/23/05    HS     Fixed wrong size of memory allocation in
                   MN_handle_cm_get_password_res()

03/27/06    NR     Clening the invoke id in MN_handle_cm_unstructured_ss_notify_res()

05/09/06    NR     Clearing the invoke ids properly in MN_handle_cm_release_ss_req()

05/24/06    NR     While sending MT USSD response not clearing the invoke id

07/20/06    NR     Clearing invoke id properly in MN_handle_cm_mt_call_ss_res()

10/03/06    NR     Clearing invoke id while sending ussd_res and notify_ussd_res

02/14/07    RD     Fixed lint errors

04/20/07    NR     Handling the MO USSD release properly

06/26/07    NR     Correcting the scenario where only SS Status in genericServiceInfo 
                   is recieved in response to interrogate request
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
#include "sys_plmn_selection_v.h"
#include "nasutils.h"


#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

#include <stringl/stringl.h>

extern ue_nas_rel_compliance_type mm_nv_nas_rel_compliance;

extern ss_state_T       CNM_get_ss_state(transaction_id_T id);

extern VOID_FUNC MN_put_result(byte invoke_id, int size, byte *argument, byte ss_operation, byte *components);


/*
 * Function name :  MN_handle_cm_abort_ss_req
 * -------------
 *
 * Description :  Processes message from CM requesting that SS services
 * -----------    are to be aborted
 *
 *
 *
 *
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

VOID_FUNC MN_handle_cm_abort_ss_req(cm_abort_ss_req_T *abort_req)
{
   connection_id_T connection_id;
   byte invoke_id, stored_operation;
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(abort_req->as_id, 0, 0, FALSE)))
#endif
   {
     /* The user wishes to close all call dependant SS */
     for (connection_id = 0; connection_id < MAXNO_SS_TRANSACTIONS; connection_id ++)
     {
       if (CNM_get_ss_state(connection_id) != SS_IDLE)  /* Invoke ID in SS transaction table */
       {
         for (invoke_id = 0; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
         {
            if(ss_invoke_data[invoke_id].connection_id == connection_id ||
               ss_invoke_data[invoke_id].mt_connection_id == connection_id)
            {
                /* Retrieive the stored operation, and RELEASE invoke entry */
                stored_operation = MN_retrieve_ss_operation((invoke_id), TRUE);
            
                /* special processing for USSD Data and Requests */
                if ((stored_operation == unstructuredSS_Request)||
                    (stored_operation == processUnstructuredSS_Request)||
                    (stored_operation == processUnstructuredSS_Data))
                {
                    MN_put_cm_release_unstructured_ss_ind(
#if defined(FEATURE_DUAL_SIM )
                    ss_transaction_information[connection_id].as_id
#endif
                    );
                }

            } /* invoke_id in ss_transaction table */
      
         } /* invoke_id */

         MN_stop_ss_timer(connection_id); /* stop related timer */

         /* active operation */
         MN_put_MNSS_END_REQ(connection_id, NULL);

		 /* reset MT related parameters to make sure that no MT session exist */
         for (invoke_id = 0; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
         {
            if(ss_invoke_data[invoke_id].mt_invoke_id_valid == TRUE)
            {
               if(ss_invoke_data[invoke_id].mt_connection_id == connection_id)
               {
                  ss_invoke_data[invoke_id].mt_invoke_id_valid = FALSE;
                  ss_invoke_data[invoke_id].mt_ss_operation = NULL_SS_OPERATION;
                  ss_invoke_data[invoke_id].mt_connection_id=UNKNOWN_CONNECTION_ID;   
                  break;
               }
           } 
         } 

         /* Inhibit the RELEASE_COMPLETE in POWER OFF or OFFLINE */
         if(abort_req != NULL)
         {
            if( (abort_req->cause == (byte)SYS_STOP_MODE_REASON_POWER_OFF) ||
                (abort_req->cause == (byte)SYS_STOP_MODE_REASON_OFFLINE) )
            {
                ss_transaction_information[connection_id].offline = TRUE;
            }
         }

      } /* SS_IDLE */

     } /* connection_id */
 
     /* If CM_RELEASE_UNSTRUCTURED_SS_IND is not already sent, then send it now */
     if((abort_req != NULL) && (waiting_for_next_network_ussd_trigger == TRUE))
     {
        MN_put_cm_release_unstructured_ss_ind(
#if defined(FEATURE_DUAL_SIM )
                         abort_req->as_id
#endif
                         );

     }

  }
}


/*
 * Function name :  MN_handle_cm_release_ss_req
 * -------------
 *
 * Description :  Processes release request from CM/UI requesting that SS services
 * -----------    are to be release.  Currently used by UE to release the USS 
 *                transaction.
 *
 *
 * Parameters: cm_release_ss_req_T
 * ----------  
 *
 *
 *
 * Returns:    none
 * --------
 *
 */

VOID_FUNC MN_handle_cm_release_ss_req(cm_release_ss_req_T *message)
{
   connection_id_T connection_id;
   byte invoke_id;
   byte flag=FALSE;
   /* The user wishes to close a specific SS cm_ss_ref & operation */

   if(message != NULL)
   {
#if defined(FEATURE_DUAL_SIM )
      if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message->as_id, message->cm_ss_ref, message->ss_operation, FALSE)))
#endif
      {
         /* MT SS operation */
         if (MN_release_ci_invoke_id(message->cm_ss_ref, message->ss_operation, &connection_id, TRUE) == TRUE)
         {
           flag = TRUE;
         } 
         else if(MN_release_mo_invoke_id(message->cm_ss_ref, message->ss_operation, &connection_id, TRUE) == TRUE) 
         {
           flag = TRUE;
         }    
         else
         {
            MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Release inactive operation: %d, ref = %d", message->ss_operation, 
                                                                  message->cm_ss_ref);
         }

         if(flag==TRUE)
         {
           /* stop related connection_id timer */
           MN_stop_ss_timer(connection_id); 

           MN_put_MNSS_END_REQ(connection_id, NULL);   
#if defined(FEATURE_DUAL_SIM )
             if(MNCNM_IS_SINGLE_STACK)
             {
               mn_as_id = message->as_id;
                MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
             }
#endif
           for (invoke_id = 0; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
           {
             if(ss_invoke_data[invoke_id].mt_invoke_id_valid == TRUE)
             {
                 if(ss_invoke_data[invoke_id].mt_connection_id == connection_id)
                 {
                       ss_invoke_data[invoke_id].mt_invoke_id_valid = FALSE;
                       ss_invoke_data[invoke_id].mt_ss_operation = NULL_SS_OPERATION;
                       ss_invoke_data[invoke_id].mt_connection_id=UNKNOWN_CONNECTION_ID;   
                 }
             } 
          }    
        } 

           /* special processing for USSD Data and Requests */
         if ((message->ss_operation == unstructuredSS_Request)||
             (message->ss_operation == processUnstructuredSS_Request)||
             (message->ss_operation == processUnstructuredSS_Data))
         {
             MN_put_cm_release_unstructured_ss_ind(
#if defined(FEATURE_DUAL_SIM )
               message->as_id
#endif
               );
         }
      }
   }
}


/*
 * Function name :  MN_handle_cm_activate_ss_req 
 * -------------
 *
 * Description :
 * -----------  
 *
 *
 *
 *
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
MN_handle_cm_activate_ss_req(cm_activate_ss_req_T *message_ptr)
{
   ie_cm_ss_error_T       cm_ss_error;
   connection_id_T        new_connection_id;
   connection_id_T        connection_id;
   boolean                call_path_available,success;
   byte                   invoke_id;
   byte                  *components;
   byte                  *argument;
   ie_facility_T          facility;
   ss_ForBS_T             ss_ForBS;
   int                    size;
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->cm_ss_ref, activateSS, TRUE)))
#endif
   {
     ss_ForBS.ss_code = message_ptr->ss_code;

     ss_ForBS.basic_service = message_ptr->basic_service;

     new_connection_id = MN_request_ci_invoke_id(&invoke_id,
                                                 &connection_id,
                                                 &call_path_available,
                                                 activateSS,
                                                 message_ptr->cm_ss_ref);

     if (invoke_id == 0xff)    /* failure */
     {
       /* Invalid invoke id - build error */
       cm_ss_error.present = TRUE;
       cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
       cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

        MN_select_ss_error(message_ptr->cm_ss_ref,
                           activateSS,
#if defined(FEATURE_DUAL_SIM )
                           message_ptr->as_id,
#endif
                           cm_ss_error);
    }
    else
    {
#if defined(FEATURE_DUAL_SIM )
      if(MNCNM_IS_SINGLE_STACK)
      {
        ss_transaction_information[connection_id].as_id = message_ptr->as_id;
      }
#endif


      success = FALSE;

      /* Start the SS Guard timer */
      MN_start_ss_timer(connection_id, invoke_id, 30000);  /* 30 sec */

      /* Allocate memory for argument */
#ifndef FEATURE_MODEM_HEAP
      if ((argument = (byte *)gs_alloc(10)) !=NULL)
#else
      if ((argument = (byte *)modem_mem_calloc(1, 10, MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
      {
         size = MN_put_ss_ForBS(ss_ForBS, argument);

         if((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL7 && mm_nv_nas_rel_compliance < NV_NAS_COMPLIANCE_REL_MAX) &&
             long_ftn_support)
         {
           if( message_ptr->ss_code == cfu || message_ptr->ss_code == cfb ||
               message_ptr->ss_code == cfnry || message_ptr->ss_code == cfnrc )
           {
              /* longFTN-Supported */ 
              argument[size++] = 0x84; /* Tag */ ;
              argument[size++] = 0x00; /* Length */ ;
              argument[1] = argument[1] + 2; /* update the length in the argument list */
           }
         }

         /* Allocate memory for component */

         
#ifndef FEATURE_MODEM_HEAP
          if ((components = (byte *)gs_alloc((unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE))) != NULL)
#else
          if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
          {
             MN_put_invoke(argument,
                          size,
                          activateSS,    /* ss_operation */
                          invoke_id,
                          components);

            /* Build the facility component */
            MN_put_facility(components, &facility, (components + (size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE) - 1));

#ifndef FEATURE_MODEM_HEAP
            gs_free( components );
#else
            modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif

            success = TRUE;
          }
          else
          {
             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
          } /* components = gs_alloc() */

#ifndef FEATURE_MODEM_HEAP
          gs_free( argument );
#else
          modem_mem_free(argument, MODEM_MEM_CLIENT_NAS);
#endif
        }
        else
        {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
        } /* argument = gs_alloc */
      
        if (success == TRUE)  /* Send relevant message to SS */
        {
           if (new_connection_id == TRUE)
           {
             MN_put_MNSS_BEGIN_REQ(connection_id, &facility);
           }
           else
           {
            MN_put_MNSS_FACILITY_REQ(connection_id, &facility);
           }
#if defined(FEATURE_DUAL_SIM )
           if(MNCNM_IS_SINGLE_STACK)
           {
             mn_as_id = message_ptr->as_id;
             MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
           }
#endif
        } /* success */
     } /* invoke_id */
   }
}


  
/*
 * Function name :  MN_handle_cm_deactivate_ss_req 
 * -------------
 *
 * Description :
 * -----------  
 *
 *
 *
 *
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
MN_handle_cm_deactivate_ss_req(cm_deactivate_ss_req_T *message_ptr)
{

   ie_cm_ss_error_T        cm_ss_error;
   boolean                  new_connection_id;
   connection_id_T          connection_id;
   boolean                  call_path_available,success;
   byte                     invoke_id;
   byte                    *components;
   byte                    *argument;
   ie_facility_T            facility;
   ss_ForBS_T               ss_ForBS;
   int                      size;

#if defined(FEATURE_DUAL_SIM ) 
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->cm_ss_ref, deactivateSS, TRUE)))
#endif
   {
     ss_ForBS.ss_code = message_ptr->ss_code;

     ss_ForBS.basic_service = message_ptr->basic_service;

     new_connection_id = MN_request_ci_invoke_id(&invoke_id,
                                                 &connection_id,
                                                 &call_path_available,
                                                 deactivateSS,
                                                 message_ptr->cm_ss_ref);

     if (invoke_id == 0xff)    /* failure */
     {
       /* Invalid invoke id - build error */
       cm_ss_error.present = TRUE;
       cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
       cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

       MN_select_ss_error(message_ptr->cm_ss_ref, 
                          deactivateSS, 
#if defined(FEATURE_DUAL_SIM )
                          message_ptr->as_id,
#endif
                          cm_ss_error);
    }
    else
    {
#if defined(FEATURE_DUAL_SIM )
      ss_transaction_information[connection_id].as_id = message_ptr->as_id;
#endif
      success = FALSE;

      /* start SS Guard Timer */
      MN_start_ss_timer(connection_id, invoke_id, 30000);  /* 30 sec */

      /* Allocate memory for argument */
#ifndef FEATURE_MODEM_HEAP
      if((argument = (byte *)gs_alloc(8)) !=NULL)
#else
      if((argument = (byte *)modem_mem_calloc(1, 8, MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
      {
         size = MN_put_ss_ForBS(ss_ForBS, argument);

         /* Allocate memory for component */
#ifndef FEATURE_MODEM_HEAP
         if ((components = (byte *)gs_alloc((unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE))) != NULL)
#else
         if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
         {

            MN_put_invoke(argument,
                         size,
                         deactivateSS,    /* ss_operation */
                         invoke_id,
                         components);

            /* Build the facility component */
            MN_put_facility(components, &facility, (components + (size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE) - 1));
            success = TRUE;
#ifndef FEATURE_MODEM_HEAP
            gs_free( components );
#else
            modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif
         }
         else
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
         }  /* components = gs_alloc() */

#ifndef FEATURE_MODEM_HEAP
         gs_free( argument );
#else
         modem_mem_free(argument, MODEM_MEM_CLIENT_NAS);
#endif
         }
         else
         {

           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
         } /* argument = gs_alloc() ; facility iei buffer */

         if( success == TRUE)  /* Send relevant message to SS */
         {
           if (new_connection_id == TRUE)
           {
             MN_put_MNSS_BEGIN_REQ(connection_id, &facility);
           }
           else
           {
             MN_put_MNSS_FACILITY_REQ(connection_id, &facility);
           }
#if defined(FEATURE_DUAL_SIM )
           if(MNCNM_IS_SINGLE_STACK)
           {
             mn_as_id = message_ptr->as_id;
             MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
           }
#endif
        }/* success */
     } /* invoke_id != 0xff */
  }
}



/*
 * Function name :  MN_handle_cm_unstructured_ss_res
 * -------------
 *
 * Description :  Process the contents of an MN_handle_cm_unstructured_ss_res
 * -----------   
 *
 *
 *
 *
 *
 *
 * Parameters: 
 * ----------  
 *
 *
 *
 * Returns:    
 * --------
 *
 */
VOID_FUNC
MN_handle_cm_unstructured_ss_res(cm_unstructured_ss_res_T *message_ptr)
{
   byte                       ss_operation;
   byte                       invoke_id;
   connection_id_T            connection_id;
   uss_data_T                *ussd_Res;
   int                        size = 0;
   byte                      *argument;
   byte                      *components;
   byte                      *last_octet_component_pointer = NULL; /* wherever success is made true, we must have a valid value for this var*/
   ss_reject_T                ss_reject;
   ie_facility_T              facility;
   boolean                    success;

#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->invoke_id, unstructuredSS_Request, FALSE)))
#endif
   {

     ss_operation = unstructuredSS_Request;

     invoke_id = message_ptr->invoke_id;

     success = FALSE;

     components = NULL;

     if (MN_release_ci_invoke_id(invoke_id, ss_operation, &connection_id, TRUE) == TRUE)
     {
 
       waiting_for_next_network_ussd_trigger = TRUE;

       if ((message_ptr->uss_res_data.data_type == USS_DATA_INFO) &&
          (message_ptr->uss_res_data.data.uss_data.present == TRUE))
       {
         ussd_Res = &(message_ptr->uss_res_data.data.uss_data);

         /* Allocate memory to argument */

#ifndef FEATURE_MODEM_HEAP
         if( (argument = (byte *)gs_alloc(ussd_Res->size + USSD_RESPONSE_OCTET_SIZE)) != NULL)
#else
         if ((argument = (byte *)modem_mem_calloc(1, (ussd_Res->size + USSD_RESPONSE_OCTET_SIZE), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif 
         {
           size = MN_put_ussd_Res(ussd_Res, argument, ussd_Res->size);

           /* Create the component with dynamic memory allocation */

#ifndef FEATURE_MODEM_HEAP
           if ((components = (byte *)gs_alloc((unsigned int)(size + RETURN_RESULT_OCTETS_MAX_SIZE_USED))) != NULL)
#else
           if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + RETURN_RESULT_OCTETS_MAX_SIZE_USED), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
           {
               last_octet_component_pointer = components +(size + RETURN_RESULT_OCTETS_MAX_SIZE_USED) -1;
               MN_put_result(invoke_id, size, argument, ss_operation, components);

               success = TRUE;
           }
           else
           {
                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
           }

#ifndef FEATURE_MODEM_HEAP
           gs_free( argument );
#else
           modem_mem_free(argument, MODEM_MEM_CLIENT_NAS);
#endif
         }
         else
         {
             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
         }
      } /* CM_USS_DATA */
      else if ((message_ptr->uss_res_data.data_type == SS_ERROR_INFO) &&
                (message_ptr->uss_res_data.data.cm_ss_error.present == TRUE))
      {
         if (message_ptr->uss_res_data.data.cm_ss_error.error_code_tag == ERROR_CODE_TAG) 
         {
#ifndef FEATURE_MODEM_HEAP
            if ((argument = (byte *)gs_alloc(30)) !=NULL)
#else
            if ((argument = (byte *)modem_mem_calloc(1, 30, MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
            {
               /* put in the argument */

               size = MN_encode_error_parameters(&(message_ptr->uss_res_data.data.cm_ss_error), argument);
               /* Build the whole RETURN_ERROR component */

#ifndef FEATURE_MODEM_HEAP
               if ((components = (byte *)gs_alloc((unsigned int)(size + RETURN_ERROR_MAX_OCTETS))) != NULL)
#else
               if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + RETURN_ERROR_MAX_OCTETS), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
               {
                  last_octet_component_pointer = components +(size + RETURN_ERROR_MAX_OCTETS) -1;
                  MN_put_error(message_ptr->uss_res_data.data.cm_ss_error,
                               invoke_id,
                               argument,    /* Null argument string */
                               size,
                               components);

                  success = TRUE;
               }
               else
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
         
               }
#ifndef FEATURE_MODEM_HEAP
               gs_free( argument );
#else
               modem_mem_free(argument, MODEM_MEM_CLIENT_NAS);
#endif
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
 
            }
         } /* ERROR_CODE_TAG */
         else if (message_ptr->uss_res_data.data.cm_ss_error.error_code_tag == INVOKE_PROBLEM)
         {
             ss_reject.problem_code_tag = message_ptr->uss_res_data.data.cm_ss_error.error_code_tag;

             ss_reject.problem_code = message_ptr->uss_res_data.data.cm_ss_error.error_code;

#ifndef FEATURE_MODEM_HEAP
            if ((components = (byte *)gs_alloc(REJECT_OCTETS_SIZE)) !=NULL)
#else
            if ((components = (byte *)modem_mem_calloc(1, REJECT_OCTETS_SIZE, MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
             {
                last_octet_component_pointer = components + REJECT_OCTETS_SIZE - 1;
                MN_put_reject(ss_reject, invoke_id, components);
 
                success = TRUE;
             }
             else
             {
                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
             }
         } /* INVOKE_PROBLEM */
         else
         {
             MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Unrecognize SS error_code_tag = %d",
                 message_ptr->uss_res_data.data.cm_ss_error.error_code_tag);
         } /* cm_ss_error */

      } /* CM_SS_ERROR */
      else 
      {

          MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Unrecognize SS data type = %d",
                    message_ptr->uss_res_data.data_type);

          success = FALSE;
      }

      if (success == TRUE)
      {
         MN_put_facility(components, &facility, last_octet_component_pointer);

         MN_put_MNSS_FACILITY_REQ(connection_id, &facility);
#if defined(FEATURE_DUAL_SIM )
           if(MNCNM_IS_SINGLE_STACK)
           {
             mn_as_id = message_ptr->as_id;
             MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
           }
#endif

         /* Free memory for components */
#ifndef FEATURE_MODEM_HEAP
         gs_free( components );
#else
         modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif
      }
      else 
      {
          MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Unrecognize Error Code Tag = %d",
                     message_ptr->uss_res_data.data.cm_ss_error.error_code_tag);
      }

    } /* release_ci_invoke_id */
    else
    {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Incorrect Invoke ID: %d",invoke_id);
    }
  }
}


/*
 * Function name :  MN_handle_cm_unstructured_ss_notify_res
 * -------------
 *
 * Description :  Process the contents of an MN_handle_cm_unstructured_ss_notify_res
 * -----------   
 *
 *
 *
 * Parameters: 
 * ----------  
 *
 *
 *
 * Returns:    
 * --------
 *
 */
VOID_FUNC
MN_handle_cm_unstructured_ss_notify_res(cm_unstructured_ss_notify_res_T *message_ptr)
{
   byte                              invoke_id;
   int                               size;
   ie_cm_ss_error_T                  cm_ss_error;
   byte                              ss_operation;
   connection_id_T                   connection_id;
   byte                              *components,component[MAX_COMPONENTS_LENGTH],argument[8];
   byte                              *last_octet_component_pointer;
   ie_facility_T                     facility;
   ss_reject_T                       ss_reject;

#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->invoke_id, unstructuredSS_Notify, FALSE)))
#endif
   {
     invoke_id = message_ptr->invoke_id;
     cm_ss_error = message_ptr->cm_ss_error;
     ss_operation = unstructuredSS_Notify;
     size = 0;
     components = &(component[0]);
     last_octet_component_pointer = components + sizeof(component) - 1;

     if (MN_release_ci_invoke_id(invoke_id,
       ss_operation,
       &connection_id,
       TRUE) == TRUE)
     {

       /* clean up buffer */
       memset(component, 0, 16);
       memset(argument, 0, 8);  /* parameters buffer */

       if (cm_ss_error.present == TRUE)
       {
         /* Build the whole RETURN_ERROR component */

         if (cm_ss_error.error_code_tag == ERROR_CODE_TAG)
         {
            /* put in the argument */

            size = MN_encode_error_parameters(&cm_ss_error, argument);

            MN_put_error(cm_ss_error, invoke_id, 
                         &(argument[0]),    /* Null argument string */
                         size,
                         components);
         }
         else if (cm_ss_error.error_code_tag == INVOKE_PROBLEM)
         {
            ss_reject.problem_code_tag = INVOKE_PROBLEM;
            ss_reject.problem_code = message_ptr->cm_ss_error.error_code;

            MN_put_reject(ss_reject, invoke_id, components);
         }
         else
         {
              MSG_ERROR_DS_2(MN_SUB,"=MNCNM= UNKNOWN ERROR CODE TAG = %d, %d",connection_id, cm_ss_error.error_code_tag);
         }

         MN_put_facility(components, &facility, last_octet_component_pointer);

         MN_put_MNSS_FACILITY_REQ(connection_id, &facility);

      }
      else /* Send a Return Result with no options */
      {
 
          MN_put_result(invoke_id, 0, argument, ss_operation, components);

          MN_put_facility(components, &facility, last_octet_component_pointer);

          MN_put_MNSS_FACILITY_REQ(connection_id, &facility);
  
         }
#if defined(FEATURE_DUAL_SIM )
         if(MNCNM_IS_SINGLE_STACK)
         {
           mn_as_id = message_ptr->as_id;
           MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
         }
#endif
    } /* MN_release_ci_invoke_id() */
    else
    {
       MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Mismatched invoke = %d, operation = %d",connection_id, ss_operation);
    }
  }
}

/*
 * Function name :  MN_handle_cm_erase_ss_req
 * -------------
 *
 * Description :  Process the contents of an MN_handle_cm_erase_ss_req
 * -----------   
 *
 *
 *
 * Parameters:                                
 * ----------  
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_handle_cm_erase_ss_req(cm_erase_ss_req_T *message_ptr)
{
   ss_ForBS_T          ss_ForBS;
   byte               *argument;
   byte               *components;
   ie_cm_ss_error_T    cm_ss_error;
   boolean             new_connection_id;
   boolean             call_path_available;
   boolean             success;
   byte                invoke_id;
   connection_id_T     connection_id;
   ie_facility_T      *facility;
   int                 size;
   byte              *last_octet_component_pointer;

#if defined(FEATURE_DUAL_SIM )
  if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->cm_ss_ref, eraseSS, TRUE)))
#endif
  {
#ifndef FEATURE_MODEM_HEAP
    facility = (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));
#else
    facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T), MODEM_MEM_CLIENT_NAS);
#endif

    if ( facility != NULL )
    {
      ss_ForBS.ss_code = message_ptr->ss_code;
      ss_ForBS.basic_service = message_ptr->basic_service;

      new_connection_id = MN_request_ci_invoke_id( &invoke_id,
                                                    &connection_id,
                                                    &call_path_available,
                                                    eraseSS,
                                                    message_ptr->cm_ss_ref);

      if (invoke_id == 0xff)
      {
         cm_ss_error.present = TRUE;
         cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
         cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

         MN_select_ss_error(message_ptr->cm_ss_ref,
                            eraseSS,    /* ss_operation */
#if defined(FEATURE_DUAL_SIM )
                            message_ptr->as_id,
#endif
                             cm_ss_error);
      }
      else
      {
#if defined(FEATURE_DUAL_SIM )
         if(MNCNM_IS_SINGLE_STACK)
         {
           ss_transaction_information[connection_id].as_id = message_ptr->as_id;
         }
#endif

         success = FALSE;

         MN_start_ss_timer(connection_id, invoke_id, 30000);  /* 30 sec */

#ifndef FEATURE_MODEM_HEAP
         if((argument = (byte *)gs_alloc(8)) !=NULL)
#else
         if((argument = (byte *)modem_mem_calloc(1, 8, MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
         {
            size = MN_put_ss_ForBS(ss_ForBS, argument);

            /* 
             * dynamically allocate memory for component 
             */

#ifndef FEATURE_MODEM_HEAP
            if ((components = (byte *)gs_alloc((unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE))) != NULL)
#else
            if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
            {
               last_octet_component_pointer = components + (size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE) -1;
               /* 
                * size of arg + 8 fixed component bytes 
                */
               MN_put_invoke(argument,
                               size,
                               eraseSS,    /* ss_operation*/
                               invoke_id,
                               components);

               MN_put_facility(components, facility, last_octet_component_pointer);

               success = TRUE;

#ifndef FEATURE_MODEM_HEAP
               gs_free( components );
#else
               modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
            } /* components = gs_alloc() */

#ifndef FEATURE_MODEM_HEAP
            gs_free( argument );
#else
            modem_mem_free(argument, MODEM_MEM_CLIENT_NAS);
#endif
         }
         else
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
         }

         if (success == TRUE)
         {
            if (new_connection_id == TRUE)

            {
               MN_put_MNSS_BEGIN_REQ(connection_id, facility);
            }

            else
            {
               MN_put_MNSS_FACILITY_REQ(connection_id, facility);
            }
#if defined(FEATURE_DUAL_SIM )
              if(MNCNM_IS_SINGLE_STACK)
              {
                mn_as_id = message_ptr->as_id;
                MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
              }
#endif
         } /* success */
      } 

#ifndef FEATURE_MODEM_HEAP
      gs_free( facility );
#else
      modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
    }
    else
    {   
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
    }
  }
}

/*
 * Function name :  MN_handle_cm_interrogate_ss_req
 * -------------
 *
 * Description :  Process the contents of an MN_handle_cm_interrogate_ss_req
 * -----------   
 *
 *
 *
 *
 *
 *
 * Parameters:                                
 * ----------  
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_handle_cm_interrogate_ss_req(cm_interrogate_ss_req_T *message_ptr)
{
   ss_ForBS_T                ss_ForBS;
   byte                     *components;
   ie_cm_ss_error_T          cm_ss_error;
   boolean                   new_connection_id;
   boolean                   call_path_available;
   boolean                   success;
   byte                      invoke_id;
   connection_id_T           connection_id;
   ie_facility_T            *facility;
   int                       size;
   byte                     *argument;
   byte                     *last_octet_component_pointer;

#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->cm_ss_ref, interrogateSS, TRUE)))
#endif
   {
#ifndef FEATURE_MODEM_HEAP
     facility = (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));
#else
     facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T), MODEM_MEM_CLIENT_NAS);
#endif

     if ( facility != NULL )
     {
       ss_ForBS.ss_code = message_ptr->ss_code;

       ss_ForBS.basic_service = message_ptr->basic_service;

       new_connection_id = MN_request_ci_invoke_id(&invoke_id,
                                                    &connection_id,
                                                    &call_path_available,
                                                    interrogateSS,
                                                    message_ptr->cm_ss_ref);

       if (invoke_id == 0xff)
       {
         cm_ss_error.present = TRUE;
         cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
         cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

         MN_select_ss_error(message_ptr->cm_ss_ref,
                            interrogateSS,    /* ss_operation */
#if defined(FEATURE_DUAL_SIM )
                            message_ptr->as_id,
#endif
                            cm_ss_error);
       }
       else
       {
#if defined(FEATURE_DUAL_SIM )
         if(MNCNM_IS_SINGLE_STACK)
         {
           ss_transaction_information[connection_id].as_id = message_ptr->as_id;
         }
#endif
         success = FALSE;

         MN_start_ss_timer(connection_id, invoke_id, 30000);  /* Guard Timer = 30 sec */

#ifndef FEATURE_MODEM_HEAP
         if ((argument = (byte *)gs_alloc(10)) !=NULL)
#else
         if ((argument = (byte *)modem_mem_calloc(1, 10, MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
         {
            size = MN_put_ss_ForBS(ss_ForBS, argument);

            if((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL7 && mm_nv_nas_rel_compliance < NV_NAS_COMPLIANCE_REL_MAX) &&
                long_ftn_support)
            {
              if( message_ptr->ss_code == cfu || message_ptr->ss_code == cfb ||
                  message_ptr->ss_code == cfnry || message_ptr->ss_code == cfnrc )
              {
                /* longFTN-Supported */ 
                argument[size++] = 0x84; /* Tag */ ;
                argument[size++] = 0x00; /* Length */ ;
                argument[1] = argument[1] + 2; /* update the length in the argument list */
              }
            }

#ifndef FEATURE_MODEM_HEAP
            if ((components = (byte *)gs_alloc((unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE))) != NULL)
#else
            if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
            {
               last_octet_component_pointer = components + (size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE) -1;
               MN_put_invoke(argument,
                             size,
                             interrogateSS,    /* ss_operation*/
                             invoke_id,
                             components);

               MN_put_facility(components, facility, last_octet_component_pointer);

               success = TRUE;

#ifndef FEATURE_MODEM_HEAP
               gs_free( components );
#else
               modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
            } /* components = gs_alloc() */

#ifndef FEATURE_MODEM_HEAP
            gs_free( argument );
#else
            modem_mem_free(argument, MODEM_MEM_CLIENT_NAS);
#endif
         }
         else
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
         } /* argument = gs_alloc() */

         if (success == TRUE)
         {
            if (new_connection_id == TRUE)
            {
               MN_put_MNSS_BEGIN_REQ(connection_id, facility);
            }
            else
            {
               MN_put_MNSS_FACILITY_REQ(connection_id, facility);
            }
#if defined(FEATURE_DUAL_SIM )
              if(MNCNM_IS_SINGLE_STACK)
              {
                mn_as_id = message_ptr->as_id;
                MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
              }
#endif
         }/* success */
      } /* invoke_id */

#ifndef FEATURE_MODEM_HEAP
      gs_free( facility );
#else
      modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
    }
    else
    {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
    } /* facility = gs_alloc() */
  }
}


/*
 * Function name :  MN_handle_cm_register_password_req
 * -------------
 *
 * Description :  Process the contents of an MN_handle_cm_interrogate_ss_req
 * -----------   
 *
 *
 *
 *
 *
 *
 * Parameters:                                
 * ----------  
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_handle_cm_register_password_req(cm_register_password_req_T *message_ptr)
{

   byte                        *argument;
   byte                        *components;  
   ie_facility_T               *facility;
   ie_cm_ss_error_T             cm_ss_error;
   boolean                      new_connection_id;
   boolean                      call_path_available;
   boolean                      success;
   byte                         invoke_id;
   connection_id_T              connection_id;
   ie_ss_code_T                 ss_code;
   int                          size;
   byte                       *last_octet_component_pointer;

#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->cm_ss_ref, registerPassword, TRUE)))
#endif
   {
#ifndef FEATURE_MODEM_HEAP
     facility = (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));
#else
     facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T), MODEM_MEM_CLIENT_NAS);
#endif

     if ( facility != NULL )
     {
       ss_code = message_ptr->ss_code;

       new_connection_id = MN_request_ci_invoke_id(&invoke_id,
                                                    &connection_id,
                                                    &call_path_available,
                                                    registerPassword,
                                                    message_ptr->cm_ss_ref);

       if (invoke_id == 0xff)
       {
         cm_ss_error.present = TRUE;
         cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
         cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

         MN_select_ss_error(message_ptr->cm_ss_ref,
                            registerPassword,    /* ss_operation */
#if defined(FEATURE_DUAL_SIM )
                            message_ptr->as_id,
#endif
                            cm_ss_error);
       }
       else
       {
#if defined(FEATURE_DUAL_SIM )
         if(MNCNM_IS_SINGLE_STACK)
         {
           ss_transaction_information[connection_id].as_id = message_ptr->as_id;
         }
#endif  
           MN_start_ss_timer(connection_id, invoke_id, 30000);  /* Guard Timer = 30 sec */

         /* Store any passwords for ss get password*/
         if (message_ptr->ss_password.present == TRUE)
         {
            ss_invoke_data[invoke_id].ss_password = message_ptr->ss_password;
            ss_invoke_data[invoke_id].ss_password_present = TRUE;
         }

         if (message_ptr->ss_new_password.present == TRUE)
         {
            ss_invoke_data[invoke_id].ss_new_password = 
                                                message_ptr->ss_new_password;

            ss_invoke_data[invoke_id].ss_new_password_present = TRUE;
         }

         if (message_ptr->ss_new_password_again.present == TRUE)
         {
            ss_invoke_data[invoke_id].ss_new_password_again = 
                message_ptr->ss_new_password_again;
            ss_invoke_data[invoke_id].ss_new_password_again_present = TRUE;
         }

         success = FALSE;

#ifndef FEATURE_MODEM_HEAP
         if ((argument = (byte *)gs_alloc(8)) !=NULL)
#else
         if ((argument = (byte *)modem_mem_calloc(1, 8, MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
         {
            size = MN_put_ss_code(ss_code.ss_code,
                                  argument);

#ifndef FEATURE_MODEM_HEAP
            if ((components = (byte *)gs_alloc((unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE))) != NULL)
#else
            if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
            {
               last_octet_component_pointer = components + (size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE) -1;
               MN_put_invoke(argument,
                             size,
                             registerPassword,    /* ss_operation*/
                             invoke_id,
                             components);

               MN_put_facility(components, facility, last_octet_component_pointer);

               success = TRUE;

#ifndef FEATURE_MODEM_HEAP
               gs_free( components );
#else
               modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
         
            } /* components = gs_alloc() */

#ifndef FEATURE_MODEM_HEAP
            gs_free( argument );
#else
            modem_mem_free(argument, MODEM_MEM_CLIENT_NAS);
#endif
         }
         else
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");

         } /* argument = gs_alloc() */

         if (success == TRUE)
         {
            if (new_connection_id == TRUE)
            {
               MN_put_MNSS_BEGIN_REQ(connection_id, facility);
            }
            else
            {
               MN_put_MNSS_FACILITY_REQ(connection_id, facility);
            }
#if defined(FEATURE_DUAL_SIM )
              if(MNCNM_IS_SINGLE_STACK)
              {
                mn_as_id = message_ptr->as_id;
                MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
              }
#endif
         } /* success */
      } /* invoke id */

#ifndef FEATURE_MODEM_HEAP
      gs_free( facility );
#else
      modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
     }
     else
     {  
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
     } /* facility = gs_alloc() */
  }
}


/*
 * Function name :  MN_handle_cm_register_ss_req
 * -------------
 *
 * Description :  Process the contents of an MN_handle_cm_register_ss_req
 * -----------   
 *
 *
 *
 *
 *
 *
 * Parameters:                                
 * ----------  
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_handle_cm_register_ss_req(cm_register_ss_req_T *message_ptr)
{
   registerSS_ARG_T         *registerSS_ARG;
   byte                     *argument;
   byte                     *components;
   ie_cm_ss_error_T          cm_ss_error;
   boolean                   new_connection_id;
   boolean                   call_path_available;
   boolean                   success;
   byte                      invoke_id;
   connection_id_T           connection_id;
   ie_facility_T            *facility;
   int                       size;
   byte                     *last_octet_component_pointer;
   boolean                   success1 = TRUE;
#if defined(FEATURE_DUAL_SIM )
  if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->cm_ss_ref, registerSS, TRUE)))
#endif
  {
    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= no_reply_condition_timer_value = %d",
                            message_ptr->cm_no_reply_condition_timer.timer_value);
    /* validating the  no_reply_condition_timer */
    /* send an error to upperlayers if validation fails*/
    if ((message_ptr->cm_no_reply_condition_timer.present == TRUE) && 
        ((message_ptr->cm_no_reply_condition_timer.timer_value < 5) || 
         (message_ptr->cm_no_reply_condition_timer.timer_value > 30 ) ||  
         (message_ptr->cm_no_reply_condition_timer.timer_value % 5 != 0)))
    {
       MSG_HIGH_DS_1(MN_SUB,"=MNCNM= NAS received invalid timer value = %d",
                                         message_ptr->cm_no_reply_condition_timer.timer_value);
      cm_ss_error.present = TRUE;
      cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
      cm_ss_error.error_code = MN_NO_REPLY_CONDITION_TIMER_VALUE_INVALID;

      MN_select_ss_error(message_ptr->cm_ss_ref,
                         registerSS,    /* ss_operation */
#if defined(FEATURE_DUAL_SIM )
                          message_ptr->as_id,
#endif
                          cm_ss_error);
      success1 = FALSE;
    }
    if (success1 == TRUE)
    {
#ifndef FEATURE_MODEM_HEAP
      facility = (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));
#else
      facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T), MODEM_MEM_CLIENT_NAS);
  #endif
  
      if ( facility != NULL )
      {
#ifndef FEATURE_MODEM_HEAP
        registerSS_ARG = (registerSS_ARG_T *) gs_alloc(sizeof(registerSS_ARG_T));
#else
        registerSS_ARG = (registerSS_ARG_T *) modem_mem_calloc(1, sizeof(registerSS_ARG_T), MODEM_MEM_CLIENT_NAS);
#endif
  
        if ( registerSS_ARG != NULL )
        {
           registerSS_ARG->ss_code = message_ptr->ss_code;
  
           registerSS_ARG->basic_service = 
                                      message_ptr->basic_service;
  
           registerSS_ARG->cm_forwarded_to_number = 
                                      message_ptr->forwarded_to_number;
  
           registerSS_ARG->cm_forwarded_to_subaddress = 
                                      message_ptr->forwarded_to_subaddress;
  
           registerSS_ARG->no_reply_condition_timer = 
                                      message_ptr->cm_no_reply_condition_timer;
  
           new_connection_id = MN_request_ci_invoke_id(&invoke_id,
                                                         &connection_id,
                                                         &call_path_available,
                                                         registerSS,
                                                         message_ptr->cm_ss_ref);
  
           if (invoke_id == 0xff)
           {
              cm_ss_error.present = TRUE;
              cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
              cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;
  
              MN_select_ss_error(message_ptr->cm_ss_ref,
                                 registerSS,    /* ss_operation */
#if defined(FEATURE_DUAL_SIM )
                                 message_ptr->as_id,
#endif
                                 cm_ss_error);
           }
           else
           {
           
#if defined(FEATURE_DUAL_SIM )
              if(MNCNM_IS_SINGLE_STACK)
              {
                ss_transaction_information[connection_id].as_id = message_ptr->as_id;
              }
#endif         
              success = FALSE;
              MN_start_ss_timer(connection_id, invoke_id, 30000);  /* Guard Timer = 30 sec */
  
#ifndef FEATURE_MODEM_HEAP
              if ((argument = (byte *)gs_alloc(sizeof(registerSS_ARG_T) + 5)) != NULL)
#else
              if ((argument = (byte *)modem_mem_calloc(1, (sizeof(registerSS_ARG_T) + 5), MODEM_MEM_CLIENT_NAS)) != NULL)
#endif
              {
                 size = MN_put_registerSS_ARG(argument, *registerSS_ARG);
  
#ifndef FEATURE_MODEM_HEAP
                 if ((components = (byte *)gs_alloc((unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE))) != NULL)
#else
                 if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
                 {
                    last_octet_component_pointer = components + (size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE) -1;
                    MN_put_invoke( argument,
                                   size,
                                   registerSS,    /* ss_operation*/
                                   invoke_id,
                                   components);
  
                    MN_put_facility(components, facility, last_octet_component_pointer);
  
                    success = TRUE;
  
#ifndef FEATURE_MODEM_HEAP
                    gs_free( components );
#else
                    modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif
                 } /* gs_alloc() */
                 else
                 {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");   
                 }
  
#ifndef FEATURE_MODEM_HEAP
                 gs_free( argument );
#else
                 modem_mem_free(argument, MODEM_MEM_CLIENT_NAS);
#endif
              } /* gs_alloc() */
              else
              {
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
        
              }
  
              if (success == TRUE )  /* MN_put_invoke */
              {
  
                 if (new_connection_id == TRUE)
                 {
                    MN_put_MNSS_BEGIN_REQ(connection_id, facility);
                 }
                 else
                 {
                    MN_put_MNSS_FACILITY_REQ(connection_id, facility);
                 }
#if defined(FEATURE_DUAL_SIM )
                   if(MNCNM_IS_SINGLE_STACK)
                   {
                     mn_as_id = message_ptr->as_id;
                     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
                   }
#endif
                }
             }
    
#ifndef FEATURE_MODEM_HEAP
           gs_free( registerSS_ARG );
#else
           modem_mem_free(registerSS_ARG, MODEM_MEM_CLIENT_NAS);
#endif
        } /* gs_alloc() registerSS_ARG */
        else
        {   
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
        }
  
#ifndef FEATURE_MODEM_HEAP
        gs_free( facility );
#else
        modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
      }
      else
      {   
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
      }
    }
  }
}

/*
 * Function name :  MN_handle_cm_get_password_res
 * -------------
 *
 * Description :  Process the contents of an MN_handle_cm_get_password_res
 * -----------   
 *
 *
 *
 *
 *
 *
 * Parameters:  cm_get_password_res
 * ----------  
 *
 *
 *
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_handle_cm_get_password_res(cm_get_password_res_T *message_ptr)
{
   byte                    ss_operation;
   byte                    invoke_id;
   connection_id_T         connection_id;
   byte                    argument[6];
   int                     size;
   byte                   *components;
   ie_facility_T          *facility;
   byte                    cm_ss_ref;
   byte                    *last_octet_component_pointer;
#if defined(FEATURE_DUAL_SIM ) 
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->invoke_id, getPassword, FALSE)))
#endif
   {
#ifndef FEATURE_MODEM_HEAP
     facility = (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));
#else
     facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T), MODEM_MEM_CLIENT_NAS);
#endif

     if ( facility != NULL )
     {

       /* Start decoding the message */

       ss_operation = getPassword;

       cm_ss_ref = message_ptr->invoke_id;

       if ( MN_find_associated_invoke_id( ss_operation,
                                         cm_ss_ref,
                                         &invoke_id ) == FALSE)
       {
          invoke_id = message_ptr->invoke_id;
       }
      

       if ((message_ptr->ss_password.present == TRUE)&&
             (MN_release_ci_invoke_id(  invoke_id,
                                        ss_operation,
                                        &connection_id,
                                        TRUE) == TRUE))
       {
         argument[0] = NUMERIC_STRING;
         argument[1] = 4;                       /* length */

         memscpy(argument + 2, sizeof(argument) -2 ,message_ptr->ss_password.ss_password, MAX_PWD_CHAR);
         size = 6;

#ifndef FEATURE_MODEM_HEAP
         if ((components = (byte *)gs_alloc((unsigned int)(size + RETURN_RESULT_OCTET_SIZE))) != NULL)
#else
         if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + RETURN_RESULT_OCTET_SIZE), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
         {
            last_octet_component_pointer = components + (size + RETURN_RESULT_OCTET_SIZE) - 1;
            MN_put_result(invoke_id,
                          size,
                          argument,
                          ss_operation,
                          components);

            MN_put_facility(components, facility, last_octet_component_pointer);

#ifndef FEATURE_MODEM_HEAP
            gs_free( components );
#else
            modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif

            MN_put_MNSS_FACILITY_REQ(connection_id, facility);
#if defined(FEATURE_DUAL_SIM )
              if(MNCNM_IS_SINGLE_STACK)
              {
                mn_as_id = message_ptr->as_id;
                MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
              }
#endif
         }
         else
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
         }
      }
      else
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect component in cm_get_password_res");
      }

#ifndef FEATURE_MODEM_HEAP
      gs_free( facility );
#else
      modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
    }
    else
    {   
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
    }
  }
}

/*
 * Function name :  MN_handle_cm_process_unstructured_ss_data
 * -------------
 *
 * Description :  Process the contents of an 
 * -----------    MN_handle_cm_process_unstructured_ss_data
 *                This is a Phase 1 MO SS operation
 *
 *
 *
 *
 *
 * Parameters:  cm_process_unstructured_ss_data
 * ----------  
 *
 *
 *
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_handle_cm_process_unstructured_ss_data(cm_process_unstructured_ss_data_T *message_ptr)
{
   byte                                ss_operation;
   byte                                invoke_id;
   byte                                cm_ss_ref;
   byte                               *components;
   byte                               *argument;
   connection_id_T                     connection_id;
   boolean                             call_path_available,success;
   boolean                             new_connection_id;
   ie_cm_ss_error_T                   cm_ss_error;
   int                                 size,index;
   ie_facility_T                       *facility;
   byte                               *last_octet_component_pointer;

#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->cm_ss_ref, processUnstructuredSS_Data, TRUE)))
#endif
   {
     components = NULL;
#ifndef FEATURE_MODEM_HEAP
     facility = (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));
#else
     facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T), MODEM_MEM_CLIENT_NAS);
#endif

     if (facility != NULL )
     {
       /* Process message data */

       ss_operation = processUnstructuredSS_Data;

       cm_ss_ref = message_ptr->cm_ss_ref;

       size = (int)message_ptr->ss_User_Data.size;

       /* check size for overrun */

       if (size > 200)
       {
         size = 200;

             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid ss_User_Data length - truncating");

       }


       new_connection_id = MN_request_ci_invoke_id(&invoke_id,
                                                    &connection_id,
                                                    &call_path_available,
                                                    ss_operation,
                                                    cm_ss_ref);

       if (invoke_id == 0xff)  /* failure */
       {
         cm_ss_error.present = TRUE;
         cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
         cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

         MN_select_ss_error(cm_ss_ref,
                            ss_operation,
#if defined(FEATURE_DUAL_SIM )
                            message_ptr->as_id,
#endif
                            cm_ss_error);
      }

      else
      {
#if defined(FEATURE_DUAL_SIM )
         if(MNCNM_IS_SINGLE_STACK)
         {
           ss_transaction_information[connection_id].as_id = message_ptr->as_id;
         }
#endif  
         success = FALSE;

         MN_start_ss_timer(connection_id, invoke_id, 30000);  /* Guard Timer = 30 sec */

         /* Build the argument */
#ifndef FEATURE_MODEM_HEAP
         if ((argument = (byte *)gs_alloc((unsigned int)(size + MAX_ELEMENT_LENGTH))) != NULL)
#else
         if ((argument = (byte *)modem_mem_calloc(1, (unsigned int)(size + MAX_ELEMENT_LENGTH), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif

         {

            index = 0;

            argument[index++] = IA5_STRING;

            MN_put_element_length(argument,
                                  &index,
                                  size);

            memscpy(&(argument[index]),
                   (size + MAX_ELEMENT_LENGTH - index),
                   message_ptr->ss_User_Data.ss_UserData,
                   (unsigned int)size);

            /* build up a component */

#ifndef FEATURE_MODEM_HEAP
            if ((components = (byte *)gs_alloc((unsigned int)(size + index + INVOKE_COMPONENT_OCTETS_MAX_SIZE_USED))) != NULL)
#else
            if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + index + INVOKE_COMPONENT_OCTETS_MAX_SIZE_USED), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
            {
               last_octet_component_pointer = components + (size + index + INVOKE_COMPONENT_OCTETS_MAX_SIZE_USED) -1;
               MN_put_invoke( argument,
                              (size + index),
                              ss_operation,
                              invoke_id,
                              components);

                
               /* build a facility */

               MN_put_facility(components,
                                facility,
                                last_octet_component_pointer);

               success = TRUE;

#ifndef FEATURE_MODEM_HEAP
               gs_free( components );
#else
               modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif
            }
            else
            {
                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
            }

#ifndef FEATURE_MODEM_HEAP
            gs_free( argument );
#else
            modem_mem_free(argument, MODEM_MEM_CLIENT_NAS);
#endif
         }
         else
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
         }

         if (success == TRUE)
         {
            if (call_path_available == TRUE)
            {
               MN_put_MNCC_FACILITY_REQ(connection_id,
                                        facility->data,
                                       (int)facility->Length,
                                       NULL,
                                       0);
            }
            else
            {
               if (new_connection_id == TRUE)
               {
                  MN_put_MNSS_BEGIN_REQ(connection_id, facility);
               }
               else
               {
                  MN_put_MNSS_FACILITY_REQ(connection_id, facility);
               }
#if defined(FEATURE_DUAL_SIM )
                 if(MNCNM_IS_SINGLE_STACK)
                 {
                   mn_as_id = message_ptr->as_id;
                   MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
                 }
#endif
              }
           }
        }
  
#ifndef FEATURE_MODEM_HEAP
      gs_free( facility );
#else
      modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif

    }
    else
    {  
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
    }
  }
}


/*
 * Function name :  MN_handle_cm_process_unstructured_ss_req
 * -------------
 *
 * Description :  Process the contents of an 
 * -----------    MN_handle_cm_process_unstructured_ss_req
 *
 *
 *
 *
 *
 *
 * Parameters:  cm_process_unstructured_ss_req
 * ----------  
 *
 *
 *
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_handle_cm_process_unstructured_ss_req(cm_process_unstructured_ss_req_T *message_ptr)
{
   byte                               ss_operation;
   byte                               cm_ss_ref;
   ie_cm_ss_error_T                   cm_ss_error;
   byte                               invoke_id;
   connection_id_T                    connection_id;
   boolean                            call_path_available;
   boolean                            new_connection_id;
   boolean                            success;
   byte                              *argument;
   byte                              *component;
   int                                size;
   ie_facility_T                      *facility;
   byte                              *last_octet_component_pointer;
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->cm_ss_ref, processUnstructuredSS_Request, TRUE)))
#endif
   {
#ifndef FEATURE_MODEM_HEAP
     facility = (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));
#else
     facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T), MODEM_MEM_CLIENT_NAS);
#endif

     if ((facility != NULL ) && (message_ptr != NULL))
     {
       /* Process the incoming message */

       /* ss_operation = processUnstructuredSS_Request; */

       ss_operation = processUnstructuredSS_Request;

       size = (int)message_ptr->uss_data.size;

       if (size > MAX_COMPONENTS_LENGTH)
       {
         size = MAX_COMPONENTS_LENGTH;

         MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Invalid uss_data length = %d invoke = %d", 
                                             size, message_ptr->cm_ss_ref);
       }

       cm_ss_ref = message_ptr->cm_ss_ref;

       new_connection_id = MN_request_ci_invoke_id(&invoke_id,
                                                    &connection_id,
                                                    &call_path_available,
                                                    ss_operation,
                                                    cm_ss_ref);

       if (invoke_id == 0xff)
       {
         cm_ss_error.present = TRUE;
         cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
         cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

         MN_select_ss_error(cm_ss_ref,
                            ss_operation,
#if defined(FEATURE_DUAL_SIM )
                            message_ptr->as_id,
#endif
                            cm_ss_error);
       }
       else
       {
#if defined(FEATURE_DUAL_SIM ) 
         if(MNCNM_IS_SINGLE_STACK)
         {
           ss_transaction_information[connection_id].as_id = message_ptr->as_id;
         }
#endif   
         success = FALSE;

             MN_start_ss_timer(connection_id, invoke_id, 30000);  /* Guard Timer = 30 sec */

         /* allocate memory for argument */

#ifndef FEATURE_MODEM_HEAP
         if ((argument = (byte *)gs_alloc((unsigned int)(size + MAX_USSD_ARGS_SIZE))) != NULL)
#else
         if ((argument = (byte *)modem_mem_calloc(1, (unsigned int)(size + MAX_USSD_ARGS_SIZE), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
         {
            size = MN_put_ussd_Arg(message_ptr->uss_data, argument, (byte)size);

         /* allocate memory for component */

#ifndef FEATURE_MODEM_HEAP
            if ((component = (byte *)gs_alloc((unsigned int)(size + INVOKE_COMPONENT_OCTETS_MAX_SIZE_USED))) != NULL)
#else
            if ((component = (byte *)modem_mem_calloc(1, (unsigned int)(size + INVOKE_COMPONENT_OCTETS_MAX_SIZE_USED), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
            {
               last_octet_component_pointer = component + (size + INVOKE_COMPONENT_OCTETS_MAX_SIZE_USED) - 1;
               MN_put_invoke(argument,
                             size,
                             ss_operation,
                             invoke_id,
                             component);

               MN_put_facility(component, facility, last_octet_component_pointer);

               success = TRUE;

#ifndef FEATURE_MODEM_HEAP
               gs_free( component );
#else
               modem_mem_free(component, MODEM_MEM_CLIENT_NAS);
#endif
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
            }

#ifndef FEATURE_MODEM_HEAP
            gs_free( argument );
#else
            modem_mem_free(argument, MODEM_MEM_CLIENT_NAS);
#endif
         }
         else
         {

            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
         }

         if (success == TRUE)
         {
            if (new_connection_id == TRUE)
            {
               MN_put_MNSS_BEGIN_REQ(connection_id, facility);
            }
            else
            {
               MN_put_MNSS_FACILITY_REQ(connection_id, facility);
            }
#if defined(FEATURE_DUAL_SIM )
              if(MNCNM_IS_SINGLE_STACK)
              {
                mn_as_id = message_ptr->as_id;
                MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
              }
#endif
           }
        }
  
#ifndef FEATURE_MODEM_HEAP
      gs_free( facility );
#else
      modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
    }
    else
    {   
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
    }
  }
}

/*
 * Function name :  MN_handle_interrogate_ss_res
 * -------------
 *
 * Description :  Process the contents of an 
 * -----------    MN_handle_interrogate_ss_res
 *
 *
 *
 *
 *
 *
 * Parameters:  parameters
 * ----------   InterrogateSS_Res
 *              
 *
 *
 * Returns:    success
 * --------    ss_conf_type
 *
 */

boolean MN_handle_interrogate_ss_res(byte *parameters, cm_ss_conf_e_type *ss_conf_type,
                                     InterrogateSS_Res_T *intSS_Res, byte *last_octet_parameters_buf)
{
   int              index;
   int              param_size;
   boolean          success;

   /* Decode parameters containing InterrogateSS_Res */

   index = 0;
   success = TRUE;

   switch (parameters[index])
   {
      case 0x80:   /* ss_Status */
      {
         index++;
         if (parameters[index] == 1)
         {
            index++;
            MN_get_ss_status(&(intSS_Res->cm_ss_status), &(parameters[index]));
            *ss_conf_type = SS_STATUS_INFO;
            index++;
         }
         else
         {
             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid ss_status length");

             success = FALSE;
         }

         break;
      }

      case 0xa2: /* context specific Construct [2] = basicServiceGroupList */
      {
         index++;
         success = MN_get_bsg_list(parameters, 
                                    &index,
                                    &(intSS_Res->basic_service_group_list),
                                    last_octet_parameters_buf);
         *ss_conf_type = BSG_LIST_INFO;
         break;
      }

      case 0xa3: /* Context specific Construct [3] = forwardingFeatureList */
      {
         index++;
         success = MN_get_ff_list(parameters, 
                                  &index,
                                  &(intSS_Res->forwarding_feature_list),
                                  last_octet_parameters_buf);
         *ss_conf_type = FWD_FEATURE_LIST_INFO;
         break;
      }

#ifdef FEATURE_CCBS
      case 0xa4: /* Context specific Construct [4] = genericServiceInfo */
      {
         ie_ss_status_T  ss_status={0,0,0,0,0};
           /*can't decide whose ss-Staus it is yet*/
         return_size_T   size;

         cli_restriction_info_T  *p_clir = &intSS_Res->cli_restriction_info;
         ccbs_info_T             *p_ccbs = &intSS_Res->ccbs_info;
           
         index++;

         size = MN_get_element_length(&parameters[index], SS_MAX_CCBS_FEATURE_LIST_LENGTH, last_octet_parameters_buf - &parameters[index] + 1);

         param_size  = size.length;
         index      += size.increment;

         while (index<param_size && success)
         {
            switch (parameters[index])
            {
               case NULL:  /* Possible null IE - end of sequence */
                  index++;

                  if (parameters[index] != NULL)
                  {
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid null IE length value in parameter list");
                     success = FALSE;
                  }

                  index = param_size;
                  break;

               case OCTET_STRING:  /* ss-Status */
               {
                  index++;
                  if (parameters[index++] != 1)
                  {
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid octet string length"); 
                     success= FALSE;
                  }

                  MN_get_ss_status(&ss_status, &parameters[index]);

                  index++;
                  intSS_Res->cm_ss_status = ss_status;
                  *ss_conf_type = SS_STATUS_INFO;                  
                  break;
               }

               case ENUMERATED:   /* cliRestrictionOption */
               {
                  index++;
                  if (parameters[index++] != 1)
                  {
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid enumeration length");
                     success = FALSE;
                  }

                  p_clir->present                           = TRUE;
                  p_clir->cm_cli_restriction_option.present = TRUE;
                  p_clir->cm_cli_restriction_option.option  = parameters[index];

                  *ss_conf_type = CLI_RESTRICTION_INFO;

                  index++;
                  break;
               }

               case (CONTEXT_SPECIFY_CONSTRUCT|2):   /* ccbs-FeatureList */
               {
                  index++;

                  *ss_conf_type = CCBS_INFO;

                  if ( !MN_get_ccbs_list(parameters, &index, p_ccbs, last_octet_parameters_buf) )
                  {
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid argument in parameters");
                     success = FALSE;
                  }
                  break;
               }

               default:
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid argument in parameters");
                  param_size = index;
                  success    = FALSE;
               }
            }
         }

         if ( success )
         {
            if ( ss_status.present )  /* mandatory */
            {
              if (p_clir->present)
              {
                 p_clir->cm_ss_status = ss_status;
              }
              else if (p_ccbs->present)
              {
                 p_ccbs->cm_ss_status = ss_status;
              }
              else
              {
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Assuming ss-Staus for CLIR info");
                 p_clir->cm_ss_status = ss_status;
              }
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: ss-Status missing");
               success = FALSE;
            }
         }
         break;
      }
#else
      case 0xa4: /* Context specific Construct [4] = cli-RestrictionInfo */
      {
         index++;
         intSS_Res->cli_restriction_info.present = TRUE;
         intSS_Res->cli_restriction_info.cm_cli_restriction_option.option = CM_CLIR_DEFAULT;
         param_size = (int)parameters[index];
         *ss_conf_type = CLI_RESTRICTION_INFO;
         index++;

         while ((index < param_size) && (success == TRUE))
         {
            switch (parameters[index])
            {
               case 0x00:  /* end code */
               {
                  index++;
                  if (parameters[index++] != 0)
                  {
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid endcode element in parameter list");
                     success = FALSE;
                  }

                  /* Make execution leave the 'while' loop */
                  param_size = index;
                  break;
               }

               case OCTET_STRING:
               {
                  index++;
                  if (parameters[index++] != 1)
                  {
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid octet string length"); 
      

                     success= FALSE;
                  }


                  MN_get_ss_status(&(intSS_Res->cli_restriction_info.
                                                            cm_ss_status),
                                   &(parameters[index]));

                  index++;

                  break;
               }

               case ENUMERATED:
               {
                  index++;
                  if (parameters[index++] != 1)
                  {

                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid enumeration length");
           
                     success = FALSE;
                  }

                  intSS_Res->cli_restriction_info.
                                    cm_cli_restriction_option.present = TRUE;

                  intSS_Res->cli_restriction_info.cm_cli_restriction_option.
                                     option = parameters[index];
                  index++;
                  break;
               }

               default:
               {

                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid argument in parameters");
      
                  param_size = index;

                  success = FALSE;
               }
            }

          }
         break;
      }
#endif /* FEATURE_CCBS */

      default:
      {

         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid Tag in interrogateSS-res");
    
         success = FALSE;
         break;
      }

   }
   return(success);
}

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
/*
 * Function name :  MN_handle_lcs_molr_res
 * -------------
 *
 * Description :  Process the contents of an 
 * -----------    MN_handle_lcs_molr_res
 *
 *
 *
 *
 *
 *
 * Parameters:  param_ptr
 * ----------   lcs_molr_Res
 *              
 *
 *
 * Returns:    success
 *
 */

boolean MN_handle_lcs_molr_res(byte *param_ptr, lcs_molr_res_T *lcs_molr_res, byte *last_octet_parameters_buf)
{
   int              index;
   return_size_T    size;
   byte             cnt;
   byte             *pointer;
   boolean          success;
   lcs_molr_res_data_T *lcs_molr_res_data = &lcs_molr_res->data.molr_res_data;

   /* Decode parameters containing lcs_molr_Res */

   index = 0;
   success = FALSE;
   lcs_molr_res->data_type = LCS_MOLR_INFO;

   if(param_ptr[index] == SEQUENCE_TAG)
   {
      index++;
      success = TRUE;

     /* Get the sequence size */

      pointer = &(param_ptr[index]);
      size = MN_get_element_length(pointer,SS_MAX_MOLR_RES_LENGTH, last_octet_parameters_buf - pointer + 1);

      /* Adjust the pointers */ 

      pointer += size.increment;
      index += size.increment;

      /* Decode the Location Estimate */

      if( param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 0 ) )
      {
         index++;
     pointer = &(param_ptr[index]);
     size = MN_get_element_length(pointer,SS_MAX_LOC_ESTIMATE_LENGTH, last_octet_parameters_buf - pointer + 1);

         /* Adjust the pointers */
         pointer += size.increment;
         index += size.increment;

         lcs_molr_res_data->location_estimate.present = TRUE;
         lcs_molr_res_data->location_estimate.size = (byte)size.length;

         for ( cnt = 0; cnt < size.length; cnt++ )
         {
            lcs_molr_res_data->location_estimate.data[ cnt ] = param_ptr[ index ];
            index++;
         }
      }

      /* Decode Deciphering Keys */
      if ( param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 1 ) )
      {
         index++;
     pointer = &(param_ptr[index]);
     size = MN_get_element_length(pointer,SS_MAX_DECIPHERING_KEYS_LENGTH, last_octet_parameters_buf - pointer + 1);

         /* Adjust the pointers */
         pointer += size.increment;
         index += size.increment;

         /* Perform Size Check */
     if ( size.length != 15 )
     {
            /* Incorrect Size, Discard Message */
        index += size.length;
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= LCS Molr Error: Deciphering Keys Size Too Large!");
         }
     else
     {
        lcs_molr_res_data->deciphering_keys.present = TRUE;

            for ( cnt = 0; cnt < 15; cnt++ )
            {
            lcs_molr_res_data->deciphering_keys.data[ cnt ] = param_ptr[index];
            index++;
        }
         }
      }

      /* Decode add-LocationEstimate */
      if ( param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 2 ) )
      {
         index++;
     pointer = &(param_ptr[index]);
     size = MN_get_element_length(pointer,SS_MAX_ADD_LOC_EST_LENGTH, last_octet_parameters_buf - pointer + 1);

     /* Adjust the pointers */
         pointer += size.increment;
         index += size.increment;

         lcs_molr_res_data->add_location_estimate.present = TRUE;
         lcs_molr_res_data->add_location_estimate.size = (byte)size.length;

         for ( cnt = 0; cnt < size.length; cnt++ )
         {
            lcs_molr_res_data->add_location_estimate.data[ cnt ] = param_ptr[index];
            index++;
         }

      } /* context | 2 */

    }  /* SEQUENCE */
    else
    {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Invalid LCS MOLR Response Received");
    }

   return(success);
}

#endif

/*
 * Function name :  MN_handle_cm_mt_call_ss_res
 * -------------
 *
 * Description :    Handles the incoming message from CM holding
 * -----------      info of the AOC state
 *
 *
 *
 *
 *
 *
 * Parameters:      IMH_T *
 * ----------   
 *
 *
 *
 * Returns:         None
 * --------
 *
 */

VOID_FUNC MN_handle_cm_mt_call_ss_res(cm_mt_call_ss_res_T *message_ptr)
{
   word    index;
   byte    components[5];

#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, 0, 0, FALSE)))
#endif
   {
     /* 
      ** Find an AOC ss_operation in the ss_invoke_data
      */

     for (index = 0;
       index < MAXNO_INVOKE_IDS;
       index++)
     {
       if ( (ss_invoke_data[index].mt_ss_operation == (byte)aoci) ||
          (ss_invoke_data[index].mt_ss_operation == (byte)aocc) )
       {
         ss_invoke_data[index].mt_invoke_id_valid = FALSE;

         if (message_ptr->aoc_supported == TRUE)
         {

             /* set up data arrays */
             /* Send Return Result to SS */

             components[0] = RETURN_RESULT;
             components[1] = 0x03; /* length */
             components[2] = 0x02; /* invoke_id_tag */
             components[3] = 0x01; /* invoke_id length */
             components[4] = ss_invoke_data[index].mt_invoke_id;


             MN_put_MNCC_FACILITY_REQ(ss_invoke_data[index].mt_connection_id,
                                     components,
                                     5,
                                     0,
                                     0);
 
#if defined(FEATURE_DUAL_SIM )
             if(MNCNM_IS_SINGLE_STACK)
             {
               mn_as_id = message_ptr->as_id;
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
             }
#endif
             /* clean up the invoke entry */
            
             ss_invoke_data[index].mt_invoke_id_valid=FALSE;
             ss_invoke_data[index].mt_connection_id= UNKNOWN_CONNECTION_ID;
             ss_invoke_data[index].mt_invoke_id= NULL;
             ss_invoke_data[index].mt_ss_operation=NULL_SS_OPERATION;

             break;
           }
        }
    }
  }
}
#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
/*
 * Function name :  MN_handle_cm_lcs_molr_req
 * -------------
 *
 * Description :  Process the contents of an MN_handle_cm_lcs_molr_req
 * -----------   
 *
 *
 *
 *
 *
 *
 * Parameters:                                
 * ----------  
 *
 *
 *
 * Returns:    
 * --------
 *
 */

VOID_FUNC MN_handle_cm_lcs_molr_req(cm_lcs_molr_req_T *message_ptr)
{

   byte                        *argument;
   byte                        *components;  
   ie_facility_T               *facility;
   ie_cm_ss_error_T             cm_ss_error;
   boolean                      new_connection_id;
   boolean                      call_path_available;
   boolean                      success = FALSE;
   byte                         invoke_id;
   connection_id_T              connection_id;
   int                          size;
   lcs_molr_ARG_T               *lcs_molr_ARG;
   byte                       *last_octet_component_pointer;

#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->cm_ss_ref, lcs_MOLR, TRUE)))
#endif
   {
#ifndef FEATURE_MODEM_HEAP
     facility = (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));
#else
     facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T), MODEM_MEM_CLIENT_NAS);
#endif

     if ( facility != NULL )
     {
#ifndef FEATURE_MODEM_HEAP
       lcs_molr_ARG = (lcs_molr_ARG_T *) gs_alloc(sizeof(lcs_molr_ARG_T));
#else
       lcs_molr_ARG = (lcs_molr_ARG_T *) modem_mem_calloc(1, sizeof(lcs_molr_ARG_T), MODEM_MEM_CLIENT_NAS);
#endif

       if ( lcs_molr_ARG != NULL )
       {
         lcs_molr_ARG->molr_type = message_ptr->molr_type;
         lcs_molr_ARG->location_method = message_ptr->location_method;
         lcs_molr_ARG->lcs_qos = message_ptr->lcs_qos;
         lcs_molr_ARG->mlc_number = message_ptr->mlc_number;
         lcs_molr_ARG->gps_assistance_data = message_ptr->gps_assistance_data;
         lcs_molr_ARG->supported_gad_shapes = message_ptr->supported_gad_shapes;
         lcs_molr_ARG->external_id = message_ptr->external_id;
 
         new_connection_id = MN_request_ci_invoke_id(&invoke_id,
                                                      &connection_id,
                                                      &call_path_available,
                                                      lcs_MOLR,
                                                      message_ptr->cm_ss_ref);

         if (invoke_id == 0xff)
         {
           cm_ss_error.present = TRUE;
           cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
           cm_ss_error.error_code = MN_INSUFFICIENT_RESOURCES;

           MN_select_ss_error(message_ptr->cm_ss_ref,
                              lcs_MOLR,    /* ss_operation */
#if defined(FEATURE_DUAL_SIM )
                              message_ptr->as_id,
#endif
                              cm_ss_error);
        }
        else
        {
#if defined(FEATURE_DUAL_SIM )
          if(MNCNM_IS_SINGLE_STACK)
          {
            ss_transaction_information[connection_id].as_id = message_ptr->as_id;
          }
#endif
           /* Per spec, Guard Timer = 30 sec. However, this value is unnecessarily
              too short and does not match the maximum response times that can
              be indicated for position location by RRLP and RRC. Changed to 300 seconds
              to make sure the timer encompasses the largest response times that
              can be specified by the network in GSM and WCDMA. */
            MN_start_ss_timer(connection_id, invoke_id, 300000);  
  
           /* Allocate enough space for the argument */
#ifndef FEATURE_MODEM_HEAP
          if ((argument = (byte *)gs_alloc(sizeof(ie_facility_T))) != NULL)
#else
          if ((argument = (byte *)modem_mem_calloc(1, sizeof(ie_facility_T), MODEM_MEM_CLIENT_NAS)) != NULL)
#endif
          {
              size = MN_put_lcs_molr_ARG(argument,
                                      lcs_molr_ARG);

#ifndef FEATURE_MODEM_HEAP
              if ((components = (byte *)gs_alloc((unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE))) != NULL)
#else
              if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
              {
                 last_octet_component_pointer = components + (size + INVOKE_COMPONENT_MANDATORY_OCTETS_MIN_SIZE) - 1;
                 MN_put_invoke(argument,
                               size,
                               lcs_MOLR,    /* ss_operation*/
                               invoke_id,
                               components);

                 MN_put_facility(components, facility, last_octet_component_pointer);
  
                 success = TRUE;
  
#ifndef FEATURE_MODEM_HEAP
                 gs_free( components );
#else
                 modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif
              }
              else
              {
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
              } /* components = gs_alloc() */
  
#ifndef FEATURE_MODEM_HEAP
              gs_free( argument );
#else
              modem_mem_free(argument, MODEM_MEM_CLIENT_NAS);
#endif
           }
           else
           {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
  
           } /* argument = gs_alloc() */
  
           if (success == TRUE)
           {
              if (new_connection_id == TRUE)
              {
                 MN_put_MNSS_BEGIN_REQ(connection_id, facility);
              }
              else
              {
                 MN_put_MNSS_FACILITY_REQ(connection_id, facility);
              }
#if defined(FEATURE_DUAL_SIM )
                if(MNCNM_IS_SINGLE_STACK)
                {
                  mn_as_id = message_ptr->as_id;
                  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
                }
#endif
           } /* success */

        } /* invoke id */
#ifndef FEATURE_MODEM_HEAP
        gs_free( lcs_molr_ARG );
#else
        modem_mem_free(lcs_molr_ARG, MODEM_MEM_CLIENT_NAS);
#endif
     }
     else
     {   
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
     }

#ifndef FEATURE_MODEM_HEAP
     gs_free( facility );
#else
     modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {  
     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
    } /* facility = gs_alloc() */
  }
}

/*
 * Function name :  MN_handle_cm_lcs_location_notification_res
 * -------------
 *
 * Description :  Process the contents of an MN_handle_cm_lcs_location_notification_res
 * -----------   
 *
 *
 *
 *
 *
 *
 * Parameters:  cm_lcs_location_notification_res
 * ----------  
 *
 *
 *
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_handle_cm_lcs_location_notification_res(cm_lcs_location_notification_res_T *message_ptr)
{
   byte                    ss_operation;
   byte                    invoke_id;
   connection_id_T         connection_id;
   byte                    argument[255];
   int                     size;
   byte                   *components;
   ie_facility_T          *facility;
   byte                    cm_ss_ref;
   byte                    *last_octet_component_pointer;
#if defined(FEATURE_DUAL_SIM )
   if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->invoke_id, lcs_LocationNotification, FALSE)))
#endif
   {
#ifndef FEATURE_MODEM_HEAP
     facility = (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));
#else
     facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T), MODEM_MEM_CLIENT_NAS);
#endif

     if ( facility != NULL )
     {
       /* Start decoding the message */

       ss_operation = lcs_LocationNotification;

       cm_ss_ref = message_ptr->invoke_id;

       if ( MN_find_associated_invoke_id( ss_operation,
                                          cm_ss_ref,
                                          &invoke_id ) == FALSE)
       {
          invoke_id = message_ptr->invoke_id;
       }
      

      if ( MN_release_ci_invoke_id(  invoke_id,
                                     ss_operation,
                                     &connection_id,
                                     TRUE) == TRUE )
      {
        argument[0] = SEQUENCE_TAG; /* Sequence Tag is Mandatory */

        if ( message_ptr->location_notification_res.present == TRUE )
        {
          argument[1] = 3;
          argument[2] = CONTEXT_SPECIFY_PRIMITIVE | 0;
          argument[3] = 1;                       /* length */

          memscpy(argument + 4, sizeof(argument)-4 ,&message_ptr->location_notification_res.location_notification_res, 1);
          size = 5;
        }
        else
        {
          argument[1] = 0; /* Sequence is Empty */
          size = 2;
        }
        
#ifndef FEATURE_MODEM_HEAP
        if ((components = (byte *)gs_alloc((unsigned int)(size + RETURN_RESULT_OCTETS_MAX_SIZE))) != NULL)
#else
        if ((components = (byte *)modem_mem_calloc(1, (unsigned int)(size + RETURN_RESULT_OCTETS_MAX_SIZE), MODEM_MEM_CLIENT_NAS)) !=NULL)
#endif
        {
           last_octet_component_pointer = components + (size + RETURN_RESULT_OCTETS_MAX_SIZE) - 1;
           MN_put_result(invoke_id,
                         size,
                         argument,
                         ss_operation,
                         components);

           MN_put_facility(components, facility, last_octet_component_pointer);

#ifndef FEATURE_MODEM_HEAP
           gs_free( components );
#else
           modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif

           MN_put_MNSS_END_REQ(connection_id, facility);
#if defined(FEATURE_DUAL_SIM )
             if(MNCNM_IS_SINGLE_STACK)
             {
               mn_as_id = message_ptr->as_id;
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
             }
#endif
        }
        else
        {
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
        }
      }
      else
      {
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Incorrect Invoke ID: %d",invoke_id);
      }

#ifndef FEATURE_MODEM_HEAP
      gs_free( facility );
#else
      modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
    }
    else
    {   
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
    }
  }
}
/*
 * Function name :  MN_handle_cm_lcs_location_notification_res
 * -------------
 *
 * Description :  Process the contents of an MN_handle_cm_lcs_location_notification_res
 * -----------   
 *
 *
 *
 *
 *
 *
 * Parameters:  cm_lcs_location_notification_res
 * ----------  
 *
 *
 *
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_handle_cm_lcs_molr_completed(cm_lcs_molr_compl_T *message_ptr)
{
    byte  invoke_id;
    byte  facility[2] = {NULL, NULL};

    if(message_ptr != NULL)
    {
#if defined(FEATURE_DUAL_SIM )
      if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->cm_ss_ref, lcs_MOLR, FALSE)))
#endif
      {
        if(MN_find_associated_invoke_id(lcs_MOLR, message_ptr->cm_ss_ref, &invoke_id))
        {
            if(invoke_id < MAXNO_INVOKE_IDS)
            {
                MN_put_MNSS_END_REQ(MN_retrieve_ss_connection_id(invoke_id, TRUE), (ie_facility_T *)facility);
#if defined(FEATURE_DUAL_SIM )
                if(MNCNM_IS_SINGLE_STACK)
                {
                  mn_as_id = message_ptr->as_id;
                  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
                }
#endif
            }
            else
            {   
                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid invoke=%d", invoke_id);
            }
        }
        else
        {
            MSG_ERROR_DS_2(MN_SUB,"=MNCNM= No matching reference=%d, operation=%d", message_ptr->cm_ss_ref, lcs_MOLR);
        }
      }
    }
    else
    {
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Invalid, NULL message");
    }
}

#endif

#ifdef FEATURE_CCBS
/*
 * Function name :  MN_handle_cm_erase_cc_entry_req
 * -------------
 *
 * Description :
 * -----------  
 *  Process CM_ERASE_CC_ENTRY_REQ from CM to SS
 *
 * Parameters:
 * ----------  
 *
 * Returns:    none
 * --------
 */
VOID_FUNC
MN_handle_cm_erase_cc_entry_req (cm_erase_cc_entry_req_T *message_ptr)
{
#define MAX_ERASECCENTRY_ARG   8
  boolean                  new_connection_id, error=FALSE;
  boolean                  call_path_available;
  connection_id_T          connection_id;
  byte                     invoke_id;
  byte                     components[255]; /*It was MAX_ERASECCENTRY_ARG+8. Now changed to remove lint error*/
  byte                     argument[MAX_ERASECCENTRY_ARG];
  ie_facility_T           *facility;
  int                      size;
  ie_cm_ss_error_T         cm_ss_error;
  byte                   *last_octet_component_pointer;

  last_octet_component_pointer = components + sizeof(components) - 1;

#if defined(FEATURE_DUAL_SIM )
  if((MNCNM_IS_SINGLE_STACK == FALSE) || (MN_validate_ss_as_id(message_ptr->as_id, message_ptr->cm_ss_ref, eraseCCEntry, TRUE)))
#endif
  {
    new_connection_id = MN_request_ci_invoke_id( &invoke_id,
                                                 &connection_id,
                                                 &call_path_available,
                                                  eraseCCEntry,
                                                  message_ptr->cm_ss_ref);

    /* *************************
     * check errors first
     * ************************* */
    if ( invoke_id == 0xff )                            // invalid invoke id
    {
      error = TRUE;
    }

    if ( message_ptr->ccbs_index.present )
    {
      if ( message_ptr->ccbs_index.ccbs_Index < 1 ||    // invalid ccbs-Index
           message_ptr->ccbs_index.ccbs_Index > 5    )
        error = TRUE;
    }

#ifndef FEATURE_MODEM_HEAP
  facility = (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));
#else
  facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T), MODEM_MEM_CLIENT_NAS);
#endif
    if ( !facility )
    {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
      error = TRUE;
    }

    /* *************************
     * process error cases
     * ************************* */
    if ( error )        /* failure */
    {
      memset (&cm_ss_error, 0x00, sizeof(ie_cm_ss_error_T));
      cm_ss_error.present        = TRUE;
      cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
      cm_ss_error.error_code     = MN_INSUFFICIENT_RESOURCES;

      MN_select_ss_error(message_ptr->cm_ss_ref, 
                         eraseCCEntry, 
#if defined(FEATURE_DUAL_SIM )
                         message_ptr->as_id,
#endif
                         cm_ss_error);

      return;
    }

    /* *************************
     * now process the message
     * ************************* */
#if defined(FEATURE_DUAL_SIM )
    if(MNCNM_IS_SINGLE_STACK)
    {
      ss_transaction_information[connection_id].as_id = message_ptr->as_id;
    }
#endif

    /* start SS Guard Timer */
    MN_start_ss_timer(connection_id, invoke_id, 30000);  /* 30 sec */

    size = MN_put_eraseCCEntry_ARG(argument, message_ptr);

    MN_put_invoke(argument, size, eraseCCEntry, invoke_id, components);

    /* Build the facility component */
    MN_put_facility(components, facility, last_octet_component_pointer);

    if ( new_connection_id )
      MN_put_MNSS_BEGIN_REQ(connection_id, facility);
    else
      MN_put_MNSS_FACILITY_REQ(connection_id, facility);
#if defined(FEATURE_DUAL_SIM )
    if(MNCNM_IS_SINGLE_STACK)
    {
      mn_as_id = message_ptr->as_id;
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= set mn_as_id=%d\n",mn_as_id); 
    }
#endif
  
#ifndef FEATURE_MODEM_HEAP
  gs_free( facility );
#else
  modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
  }
}
#endif /* FEATURE_CCBS */

