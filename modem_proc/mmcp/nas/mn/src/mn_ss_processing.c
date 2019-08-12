/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001- 2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_ss_processing.c_v   1.19   13 May 2002 15:20:56   abanh  $   
           
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_ss_processing.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $                      
           
when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

4/25/01     TS     Corrected ARM compile error.  Missing ; at end of line.

5/4/01      CD     Fixed compile warning C4701: local variable 'index' may be 
                   used without having been initialized in MN_execute_ss_notify

5/4/01      CD     Fixed comiple warning C4701: local variable 'invoke_success' 
                   may be used without having been initialized in 
                   MN_request_ci_invoke_id.  Set to FALSE.

5/4/01      CD     Fixed compile warnings C4244 conversion from int to byte

5/7/01      CD     Fixed compile warnings C4244 conversion from int to byte in
                   MN_put_facility() and MN_put_registerSS_ARG()

5/7/01      CD     Fixed compile warning C4701 - local variable may be used
                   without being initialized.  Set ff_index=0 in MN_handle_ss_info()

5/10/01     CD     Removed unnecessary header file includes

6/1/01      CD     Removed redundant declaration of ss_transaction_information, 
                   cc_transaction_information and MN_get_ss_status(), since they are
                   declared in header files

                   Corrected use of memcpy in MN_get_ff_list

                   Changed logic involved in calling memcpy in MN_get_ff_list 
                   avoid lint error (possible data overrun)

                   Removed redundant declaration of ss_invoke_data

                   Removed local variable 'ptr' in MN_select_ss_error since it was 
                   not accessed

                   Added missing default case to switch statement in MN_handle_error

                   In MN_handle_invoke, corrected check of param_length before memcpy to avoid possible data
                   overrun

                   In MN_handle_invoke, removed local variables sequence_length and return_ptr
                   since they were not being accessed

                   In MN_decode_error_parameters, corrected use of 'parameter' to avoid
                   many out-of-bounds lint errors

                   Corrected call to memcpy in MN_put_invoke, MN_handle_result and MN_put_result


                   Fixed indentation throughout the file

                   Removed ms.h and cause.h since they were not being used

6/4/01      CD     Corrected use of 'buffer' union in MN_select_ss_error to avoid lint error

7/18/01     CD     Removed duplicate external declaration of ss_invoke_data

8/10/01     CD     Removed unnecessary queue.h

08/16/01    AB     Renamed MMI references to CM.

09/10/01    AB     Fixed eraseSS mishandled of ss_info processing.

10/12/01    AB     Fixed numberous Lint errors (even case indentation was flag error?).

10/25/01    AB     Replaced reference to ss_transaction_information[] variable, with
                     SS state access function, CNM_get_ss_state().

01/18/02    AB     Initialized the CM notify SS buffer in mn_execute_ss_notify() 
                     with 0x00 instead of ASCII '0'. 

01/20/02    AB     Set the connection_id to NEW_CONNECTION_ID indicator instead of 
                     the actual connection ID that derived from transaction TI, when network
                     sent SS notification via new SETUP message. 

01/25/02    CD     Replaced IEI by boolean flag
                   Updated copyright
                   Replaced calls to ERR() by MSG_ERR()

02/07/02    AB     Fill in NO_INFO in cm_ss_conf_e_type even for SS RETURN result without
                     parameters. 

02/18/02    AB     Set a new flag, success, in SS call independent confirmation
                     function to prevent UI for locking up when MN unable to 
                     process the parameters that received from the network.

03/04/02    AB     Fixed MN_get_ff_list() to process the optional 
                     longForwardToNumber, 0x89, and up to 
                     MAXNO_FOREWARD_TO_NUMBER, 9, instead of just 1.

03/12/02    AB     Update the notify_ss to forward the all optional ie 
                     to CM including ect_indicator, cnap_indicator, 
                     alert_pattern, ccbs_feature, multicall_indicator. 

04/08/02    AB     Splited the long ECT indication notification into 
                     seperate function MN_get_ect_indicator() from 
                     the main notifySS function, MN_execute_ss_notify().

04/30/02    AB     Merged changes in 1.16.1.0 branch to correct the 
                     the SS call independent response messages to CM with
                     the addition of success element added.

05/06/02    AB     Replaced the '0x00' constant used in memset() with a NULL
                     liternal.

08/14/02    AB     In MN_execute_ss_notify(), advance a pointer 
                      over callingName tag before processing the name CHOICE
                      element.

09/16/02    AB     In MN_execute_ss_notify(), defer notification only for 
                      SETUP indication transistion, CC_STATE_U6, state.

10/02/02    AB     In MN_request_ci_invoke_id(), start connection ID search
                      for USSD operation at 1 instead of 0.

11/6/02     AB     In MN_handle_result(), process required parameter
                   UnstructuredUSSData return result component only
                   if parameter length is not zero.  

                   Although ANS.1 required a mandatory return result 
                   to contains IA5String Tag (with empty string), if
                   parameters is not included in the return component, 
                   the required paramter tags would not need to be 
                   process. 

02/02/03    AB     In MN_get_ect_indicator, MN_get_name_indicator, 
                   MN_get_ccbs_indicator() ended the parsing of the
                   sequence in a while loop when detect an error.

04/04/03    AB     In MN_request_ci_invoke_id(), force a new MM
                   connection to be established on retry of 
                   USSD cross phase compatible procedure. 

06/17/03    AB     Added processing of CNAP NULL length to prevent
                   flagging of F3 error message.

04/15/03    sbs    Added support for Location Services related SS messages

04/11/05    HS     Fixed indexing error for forward_to_number[] in MN_get_ff_list()

04/22/05    HS     Added support for FEATURE_CCBS

07/08/05    HS     MN_get_ccbs_indicator() not used anymore. Use MN_get_ccbs_feature() instead.

09/23/05    HS     Fixed length checking routine on processUnstructuredSS_Request

10/21/05    HS     When clearing operation for certain invoke id, also clear any MT operation
                   with the same invoke id

12/20/05    NR     Retuning proper transaction ID for  processUnstructuredSS_Data when MT call  is present

12/20/05    NR     Finding proper invoke Id in MN_find_associated_invoke_id() for MO SS transactions

03/27/06    NR     Adding lcs_MOLR in MN_check_for_parallel_SS()

11/04/06    NR     Adding support for LSM CR88759

08/28/06    NR     Adding MN_release_invoke_ids() for clearing the invoke ids.

11/02/06    NR     Fixed lint errors

12/22/06    RD     Fixed lint errors

01/11/07    NR     Adding fix for rejecting multiple USSD with retaining transactin id
                   inspite of invoke id being cleared
                   
02/14/07    RD     Fixed lint errors

04/20/07    NR     Adding new function for releasing MO invoke ids

09/05/11    AM     Adding range check to avoid buffer overflow

02/09/12    PM     USSD- NAS sending RELEASE_COMPLETE to NW on getting wrong 
                   opcode in INVOKE by NW (on top of MO USSD). In this scenario
                   NAS sends CM_PROCESS_UNSTRUCTURED_SS_CONF.
===========================================================================*/

#include "mmcp_variation.h"
#include "rex.h"
#include "environ.h"
#include <string.h>
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include <stringl/stringl.h>
#include "nasutils.h"



extern ue_nas_rel_compliance_type mm_nv_nas_rel_compliance;


extern ss_state_T       CNM_get_ss_state(transaction_id_T id);

#ifndef FEATURE_MODEM_HEAP
/*
 * Allocate some space for message construction used in this module
 */

static union
{
   InterrogateSS_Res_T  a;
   ss_info_T            b;
   ss_password_info_T   c;
   ss_UserData_info_T   d;
   ss_data_info_T       e;
#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
   lcs_molr_res_T       f;
#endif
} buffer; 

#endif

static byte MN_find_mt_invoke_id( byte mt_invoke_id );

typedef enum
{
  MEM_ALLOC,
  MEM_FREE,
  MEM_ALLOC_FAILURE
} mn_print_mem_type_e_type;

/*
 * Function name :  MN_print_ss_status
 * -------------
 *
 * Description :  This function is used to print memory allocation or failures.
 *                      Main purpose of this function is to reduce F3s.
 * -----------     
 *
 *
 *
 *
 * Parameters: 
 * ----------  
 *
 *
 *
 * Returns: None
 * --------
 *
 */

void MN_print_ss_status(mn_print_mem_type_e_type mem_type, byte ss_operation)
{
  switch(mem_type)
  {
    case MEM_ALLOC:
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MEM ALLOC - %d", ss_operation);
         break;
    case MEM_FREE:
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MEM FREE - %d", ss_operation );
         break;
    case MEM_ALLOC_FAILURE:
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Memory allocation failure for %d", ss_operation );
         break;
  }
}


/*
 * Function name :  MN_request_invoke_id
 * -------------
 *
 * Description :  Checks out a new invoke_id for a MO SS operation
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
 * Returns: invoke_id
 * --------
 *
 */

byte
MN_request_invoke_id(byte ss_operation)
{
   boolean success;
   byte invoke_id;

   success = FALSE;

   for(invoke_id = 0x00; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
   {
      if(ss_invoke_data[invoke_id].ss_stored_operation == NULL_SS_OPERATION)
      {
         success = TRUE;
         ss_invoke_data[invoke_id].ss_stored_operation = ss_operation;
         break;
      }
   }

   if(success == TRUE)
   {
      return invoke_id;
   }

   else
   {
      return INVOKE_FAIL;
   }

}



/*
 * Function name :  MN_retrieve_ss_operation
 * -------------
 *
 * Description :  given the original invoke_id
 * -----------    the routine returns the SS operation code in
 *                the original SS request
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  call_progress
 *
 *
 *
 * Returns:
 * --------
 *
 */

byte MN_retrieve_ss_operation(byte invoke_id, boolean delete_entry)

{

   byte    ss_code_returned = NULL, i;

   /*  FIND THE SS OPERATION CODE CORRESPONDING TO THE INVOKE ID
             GIVEN DELETE IS REQUIRED  */
   if(invoke_id < MAXNO_INVOKE_IDS)
   {
       ss_code_returned = ss_invoke_data[invoke_id].ss_stored_operation;

       if(delete_entry == TRUE)
       {
          ss_invoke_data[invoke_id].ss_stored_operation = NULL_SS_OPERATION;
          ss_invoke_data[invoke_id].connection_id = UNKNOWN_CONNECTION_ID;

          if(ss_code_returned == NULL_SS_OPERATION)
          {
            ss_code_returned = ss_invoke_data[invoke_id].mt_ss_operation;
            ss_invoke_data[invoke_id].mt_invoke_id_valid = FALSE;
            ss_invoke_data[invoke_id].mt_ss_operation = NULL_SS_OPERATION;
            ss_invoke_data[invoke_id].mt_connection_id = UNKNOWN_CONNECTION_ID;
          }

          /* clear any MT operation with same invoke id (most likely for */
          /* getPassword operation linked with other operations)         */
          for (i=0; i<MAXNO_INVOKE_IDS; i++)
          {
            if (ss_invoke_data[i].mt_invoke_id_valid &&
                ss_invoke_data[i].mt_invoke_id == invoke_id)
            {
              ss_invoke_data[i].mt_invoke_id_valid = FALSE;
              break;
            }
          }
       }
   }

   return(ss_code_returned);
}

/*
 * Function name :  MN_retrieve_ss_connection
 * -------------
 *
 * Description :  given the original invoke_id
 * -----------    the routine returns the SS operation code in
 *                the original SS request
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  call_progress
 *
 *
 *
 * Returns:
 * --------
 *
 */

byte MN_retrieve_ss_connection_id(byte invoke_id, boolean delete_entry)

{
   byte    ss_code_returned = NULL;

   /*  FIND THE SS OPERATION CODE CORRESPONDING TO THE INVOKE ID
             GIVEN DELETE IS REQUIRED  */

   if(invoke_id < MAXNO_INVOKE_IDS)
   {
       ss_code_returned = ss_invoke_data[invoke_id].connection_id;

       if(delete_entry == TRUE)
       {
           ss_invoke_data[invoke_id].ss_stored_operation = NULL_SS_OPERATION;
           ss_invoke_data[invoke_id].connection_id = UNKNOWN_CONNECTION_ID;
       }
   }

   return(ss_code_returned);
}


/*
 * Function name :  MN_execute_ss_notify
 * -------------
 *
 * Description :  Acknowledge reciept of message and forward
 * -----------     charge advice to user
 *
 *
 *
 *
 *
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  component_number
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_execute_ss_notify(MN_FACILITY_DATA_T *facility_data_ptr,  
                               int component_number)
{
   boolean                            success, notify_deferred = FALSE;
   int                                index,seq_end;
   ie_cm_notify_ss_T                 *notify_data;
   byte                               element_length,*parameter_pointer;
   byte                               *last_octet_data_pointer;
   return_size_T                      size;
   
   /* tmp_idx - This will be used to store index value before it is passed on to another function call.
        This will help in calculating the increments done to actual index by the function call.
        It should be initialized to index before use whenever used.
    */
   int                                  tmp_idx; 

   /*  DECODE THE PARAMETERS AND SEND DATA ON TO CM  */

/* Obtain memory for the message */

   index = 0;
#ifndef FEATURE_MODEM_HEAP
   notify_data = (ie_cm_notify_ss_T*) gs_alloc(sizeof(ie_cm_notify_ss_T));
#else
   notify_data = (ie_cm_notify_ss_T*) modem_mem_calloc(1, sizeof(ie_cm_notify_ss_T), MODEM_MEM_CLIENT_NAS);
#endif

   if (notify_data == NULL)
   {
      success = FALSE;
   }
   else
   {
      success = TRUE;

      /* Initialise memory before use */
      memset(notify_data, NULL, sizeof(ie_cm_notify_ss_T));
   }

   parameter_pointer = &(facility_data_ptr->
      facility_component_data[component_number].parameters[index]);
   last_octet_data_pointer = parameter_pointer + MAX_SS_PARAMETER_LENGTH -1; /* Note that this is true if index is zero*/

   if((*parameter_pointer != SEQUENCE_TAG) &&
       (*parameter_pointer != SET_TAG)) /* sequence identifier */
   {
      success = FALSE;
   }

   index++;
   parameter_pointer++;

   /* find the size of the sequence */
   size = MN_get_element_length(parameter_pointer,SS_MAX_NOTIFY_LENGTH, (last_octet_data_pointer - parameter_pointer + 1));

   index += size.increment;
   parameter_pointer+=size.increment;

   /* find the end of the sequence */
   seq_end = index + size.length;

   if(&(parameter_pointer[seq_end - 1]) > last_octet_data_pointer)
   {
     success = FALSE;
   }
 
   /* parse the sequence */ 
   while((index < seq_end) &&(success == TRUE))
   {
     if(notify_data != NULL) 
     {
       switch(*parameter_pointer)
       {
       case 0x81:  /* context-specific primitive - ss-code */

        if( (parameter_pointer + 2) <= last_octet_data_pointer) /* reading three bytes */
        {
            /* Load data in to CM structures */

            index++;
            parameter_pointer++;
            element_length = *parameter_pointer;
            notify_data->cm_ss_code.present = TRUE;

            parameter_pointer++;
            index++;
            element_length--;
            notify_data->cm_ss_code.ss_code = *parameter_pointer;

            /* Check element length was correct */
            if (element_length != 0)
            {
               success = FALSE;
            }

            index++;
            parameter_pointer++;

         }
         else
         {
            parameter_pointer += 3;
            index += 3;
            success = FALSE;
         }
         break;

        case 0x84:  /* context-specific primitive ss-status*/
         if( (parameter_pointer + 2) <= last_octet_data_pointer) /* reading three bytes */
         {
            /* Load data in to CM structures */

            index++;
            parameter_pointer++;
            element_length = *parameter_pointer;

            parameter_pointer++;
            index++;
            element_length--;

            MN_get_ss_status(&(notify_data->cm_ss_status), parameter_pointer); /* reads one byte */

            /* Check element length was correct */
            if (element_length != 0)
            {
               success = FALSE;
            }

            index++;
            parameter_pointer++;

         }
         else
         {
            parameter_pointer += 3;
            index += 3;
            success = FALSE;
         }
         break;

        case 0x85:  /* context-specific primitive - ss-notification */
         if( (parameter_pointer + 2) <= last_octet_data_pointer) /* reading three bytes */
         {
            /* Load data in to CM structures */

            index++;
            parameter_pointer++;
            element_length = *parameter_pointer;
            notify_data->cm_ss_notification.present = TRUE;
            

            parameter_pointer++;
            index++;
            element_length--;
            notify_data->cm_ss_notification.notification = *parameter_pointer;

            /* Check element length was correct */
            if (element_length != 0)
            {
               success = FALSE;
            }

            index++;
            parameter_pointer++;

         }
         else
         {
            parameter_pointer += 3;
            index += 3;
            success = FALSE;
         }
         break;

        case 0x8e:  /* context-specific primitive - callsWaitingIndicator */
         if( (parameter_pointer + 1) <= last_octet_data_pointer) /* reading two bytes */
         {
            /* Load data in to CM structures */

            index++;
            parameter_pointer++;
            element_length = *parameter_pointer;
            notify_data->cm_call_is_waiting_indicator.present = TRUE;

            /* Check element length was correct */
            if (element_length != 0)
            {
               success = FALSE;
            }

            index++;
            parameter_pointer++;

         }
         else
         {
            parameter_pointer += 2;
            index += 2;
            success = FALSE;
         }
         break;

        case 0x8f:  /* context-specific primitive - callOnHold-Indicator */
         if( (parameter_pointer + 2) <= last_octet_data_pointer) /* reading three bytes */
         {
            /* Load data in to CM structures */

            index++;
            parameter_pointer++;
            element_length = *parameter_pointer;
            notify_data->cm_call_on_hold_indicator.present = TRUE;
            parameter_pointer++;
            index++;
            element_length--;
            notify_data->cm_call_on_hold_indicator.call_on_hold = 
               *parameter_pointer;

            /* Check element length was correct */
            if (element_length != 0)
            {
               success = FALSE;
            }

            index++;
            parameter_pointer++;
         }
         else
         {
            parameter_pointer += 3;
            index += 3;
            success = FALSE;
         }
         break;

        case 0x90:  /* context-specific primitive - mpty-Indicator */
         if( (parameter_pointer + 1) <= last_octet_data_pointer) /* reading two bytes */
         {
            /* Load data in to CM structures */

            index++;
            parameter_pointer++;
            element_length = *parameter_pointer;
            notify_data->cm_mpty_indicator.present = TRUE;

            /* Check element length was correct */
            if (element_length != 0)
            {
               success = FALSE;
            }

            index++;
            parameter_pointer++;
         }
         else
         {
            parameter_pointer += 2;
            index += 2;
            success = FALSE;
         }
         break;

        case 0x91:  /* context-specific primitive - cug-Index */
         if( (parameter_pointer + 3) <= last_octet_data_pointer) /* reading four bytes */
         {
            /* Load data in to CM structures */

            index++;
            parameter_pointer++;
            element_length = *parameter_pointer;
            notify_data->cm_cug_index.present = TRUE;

            if (element_length == 1)

            {
               notify_data->cm_cug_index.msb = 0;
            }
            else
            {
               parameter_pointer++;
               index++;
               element_length--;
               notify_data->cm_cug_index.msb = *parameter_pointer;
            }

            parameter_pointer++;
            index++;
            element_length--;
            notify_data->cm_cug_index.lsb = *parameter_pointer;

            /* Check element length was correct */
            if (element_length != 0)
            {
               success = FALSE;
            }

            index++;
            parameter_pointer++;

         }
         else
         {
            parameter_pointer += 4;
            index += 4;
            success = FALSE;
         }
         break;


        case 0x92:  /* context-specific primitive - clirSuppressionRejected */
         if( (parameter_pointer + 1) <= last_octet_data_pointer) /* reading two bytes */
         {
            /* Load data in to CM structures */

            index++;
            parameter_pointer++;
            element_length = *parameter_pointer;
            notify_data->cm_clir_suppression_rejected.clir_suppresion_rej = 
                                                        CM_CLIR_SUPPRESSION_REJECTED;
            notify_data->cm_clir_suppression_rejected.present =  TRUE;

            /* Check element length was correct */
            if (element_length != 0)
            {
               success = FALSE;
            }

            index++;
            parameter_pointer++;

         }
         else
         {
            parameter_pointer += 2;
            index += 2;
            success = FALSE;
         }
         break;       

        case (0x96): /* alertingPattern */
         if( (parameter_pointer + 2) <= last_octet_data_pointer) /* reading three bytes */
         {

             index++;
             parameter_pointer++;
             element_length = *parameter_pointer;

             index++;
             parameter_pointer++;
             notify_data->cm_alert_pattern.pattern = *parameter_pointer;

             notify_data->cm_alert_pattern.present =  TRUE;

#ifdef FEATURE_CCBS
             index++;
             parameter_pointer++;
#endif /* FEATURE_CCBS */

         }
         else
         {
            parameter_pointer += 3;
            index += 3;
            success = FALSE;
         }
         break; 

        case (0x97): /* multicall-Indicator */
         if( (parameter_pointer + 1) <= last_octet_data_pointer) /* reading two bytes */
         {
             index++;
             parameter_pointer++;
             element_length = *parameter_pointer;

             index++;
             parameter_pointer++;
             notify_data->cm_multicall_indicator.indicator = (ie_indicator_T)(*parameter_pointer);

             notify_data->cm_multicall_indicator.present = TRUE;;
         }
         else
         {
            parameter_pointer += 2;
            index += 2;
            success = FALSE;
         }
         break;

        case (0xB3):  /* ect-Indicator */
         if(parameter_pointer <= last_octet_data_pointer) 
         {
             tmp_idx = index; /* store index value before passing it in function call*/

             success = MN_get_ect_indicator(notify_data, parameter_pointer, &index, last_octet_data_pointer); /* reads *(parameter_pointer + 1) octets*/

             parameter_pointer += (index - tmp_idx); 

         } /* ect indicator */
         else
         {
           success = FALSE;
         }
         break;

        case (0xB4): /* name-Indicator */
         {
             tmp_idx = index; /* store index value before passing it in function call*/

             success = MN_get_name_indicator(notify_data, parameter_pointer, &index, last_octet_data_pointer); 

             parameter_pointer += (index - tmp_idx); 

             break;
         }

        case (0xB5): /* ccbs-Feature */
         {
#ifdef FEATURE_CCBS


             parameter_pointer ++;
             index++;
             tmp_idx=index;

             success = MN_get_ccbs_feature(parameter_pointer, &index, &notify_data->cm_ccbs_indicator, last_octet_data_pointer); 

             parameter_pointer += (index-tmp_idx); 
#else
             tmp_idx = index;

             success = MN_get_ccbs_indicator(notify_data, parameter_pointer, &index, last_octet_data_pointer); 

             parameter_pointer += (index-tmp_idx); 
#endif
             break;
         }

        case (NULL_TAG):
         {
           /* check the length */
           index++;
           parameter_pointer++;
           if (*parameter_pointer != NULL)
           {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Null component length incorrect");

           }
            
           seq_end = index;

           break;
          }

        default:
        {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected component in SS notify data-skipping");

            /* Skip the unknown IE */
            index++;
            parameter_pointer++;
            size = MN_get_element_length(parameter_pointer,SS_MAX_NOTIFY_LENGTH, (last_octet_data_pointer - parameter_pointer + 1));

            index += (size.increment + size.length);
            parameter_pointer += (size.increment + size.length);

            break;
        }

       } /* switch */

     } /* notify_data */

   }

   if(success == TRUE)
   {
       /*  SEND THE MESSAGE  */
       if((cc_transaction_information_p[facility_data_ptr->connection_id] != NULL) &&
          (cc_transaction_information_p[facility_data_ptr->connection_id]->cc_state == CC_STATE_U6))
       {
           /* Give CM/UI a chance to create a call object for given connection id */

           notify_deferred = TRUE;

           MN_put_notify_deferred_ind(facility_data_ptr->connection_id, notify_data);
       }
       else
       {
           MN_put_cm_mt_call_ss_ind(facility_data_ptr->connection_id,
                                    (byte)CM_NOTIFY_SS_IND,
                                    notify_data,
                                    NULL);
       }
   }
   else
   {
      
      if (facility_data_ptr->facility_component_data[component_number].
          component_type == RETURN_ERROR)
      {
         MN_put_crss_reject(RETURN_ERROR_PROBLEM,
             UNEXPECTED_ERROR,
             facility_data_ptr->connection_id,
             facility_data_ptr->facility_component_data[component_number].
             invoke_id);
      }

      MSG_ERROR_DS_3(MN_SUB,"=MNCNM= ERROR: Unexpected component element", facility_data_ptr->connection_id,
                                                 component_number,
                                                 facility_data_ptr->facility_component_data[component_number].invoke_id);

   }

   if ((notify_data != NULL) && (notify_deferred == FALSE))
   {
#ifndef FEATURE_MODEM_HEAP
        gs_free(notify_data);
#else
        modem_mem_free(notify_data, MODEM_MEM_CLIENT_NAS);
#endif
   }
}


/*
 * Function name :  MN_get_ss_status
 * -------------
 *
 * Description :  Get ss_status from ss_notify from facility message
 * -----------   
 *
 *
 *
 *
 *
 *
 * Parameters: ie_ss_status_T *
 * ----------  data_byte
 *
 *
 *
 * Returns:    none
 * --------
 *
 */
VOID_FUNC
MN_get_ss_status(ie_ss_status_T *cm_ss_status, byte *data_byte)
{

   cm_ss_status->present = TRUE;

   if ((*data_byte & 0x01) > 0)
   {
      cm_ss_status->cm_ss_active = TRUE;
   }
   else
   {
      cm_ss_status->cm_ss_active = FALSE;
   }
   if ((*data_byte & 0x02) > 0)
   {
      cm_ss_status->cm_ss_registered = TRUE;
   }
   else
   {
      cm_ss_status->cm_ss_registered = FALSE;
   }
   if ((*data_byte & 0x04) > 0)
   {
      cm_ss_status->cm_ss_provisioned = TRUE;
   }
   else
   {
      cm_ss_status->cm_ss_provisioned = FALSE;
   }
   if ((*data_byte & 0x08) > 0)
   {
      cm_ss_status->cm_ss_operative = TRUE;
   }
   else
   {
      cm_ss_status->cm_ss_operative = FALSE;
   }

}



/*
 * Function name :  MN_get_bsg_list
 * -------------
 *
 * Description :  Get the data from basic services group list.
 * -----------    Enter routine with [index] pointing at the 
 *                length element of the sequence containing
 *                the bsgl in the input message parameters.
 *                Data is transferred from octet string in
 *                parameters to bs_group_list
 *
 *
 * Parameters: parameters
 * ----------  msg_index
 *             
 *
 *
 * Returns:    basic_service_group_list,success
 * --------
 *
 */
boolean
MN_get_bsg_list(byte *parameters, int *index,basic_service_group_list_T *bs_group_list, byte *last_octet_parameters_buf)
{
   int bsgl_index = 0;
   int msg_index,last_octet;
   boolean    success;
   return_size_T    size;

   msg_index = *index;

   success = TRUE;

   size = MN_get_element_length(&(parameters[msg_index]),SS_MAX_BSGL_LENGTH, (last_octet_parameters_buf - &(parameters[msg_index]) + 1)) ;

   msg_index+= size.increment;


   bs_group_list->present = TRUE;


   last_octet = msg_index + size.length;

   if(last_octet >= MAX_SS_PARAMETER_LENGTH)
   {
     last_octet = MAX_SS_PARAMETER_LENGTH;
   }
   
   if(&(parameters[last_octet - 1]) > last_octet_parameters_buf)
   {
     success = FALSE;
     last_octet = msg_index; /* This is done to skip processing inside while loop and also not alter msg_index value */
   }
   
   while (msg_index < last_octet)
   {

      switch (parameters[msg_index])
      {
      case 0x82:  /* bearer service */
         {
            bs_group_list->list[bsgl_index].present = TRUE;
            msg_index++;
            if (parameters[msg_index++] != 1)
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length element in parameter list");

               success = FALSE;
            }
            bs_group_list->list[bsgl_index].code_type =   BS_CODE;
            bs_group_list->list[bsgl_index].bs_code =   parameters[msg_index];
            break;

         }

      case 0x83:  /* tele service */
         {
            bs_group_list->list[bsgl_index].present = TRUE;
            msg_index++;
            if (parameters[msg_index++] != 1)
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length element in parameter list");

               success = FALSE;
            }

            bs_group_list->list[bsgl_index].code_type = TS_CODE;
            bs_group_list->list[bsgl_index].bs_code= parameters[msg_index];
            break;
         }

       default:
         {
            success = FALSE;
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid identifier element in parameter list");

            /* Make execution leave the 'while' loop */
            msg_index = last_octet;
               break;
         }
      }
         /* End of switch statement */
      bsgl_index++;

      if(bsgl_index >= BASIC_SERV_GROUP_LIST_SIZE)
      {
        msg_index = last_octet;
        break;
      }

      msg_index++;
   }

      /* End of while statement */

   bs_group_list->list_length = (byte)bsgl_index;
   *index = msg_index;

   return(success);

}



/*
 * Function name :  MN_get_ff_list
 * -------------
 *
 * Description :  Get the data from forwarding feature list
 * -----------    Enter routine with [index] pointing a first
 *                octet of length element
 *
 *
 *
 *
 *
 * Parameters: parameters
 * ----------  msg_index
 *             
 *
 *
 * Returns:    forwarding feature list,success
 * --------
 *
 */
boolean
MN_get_ff_list(byte *parameters, int *index,ie_forwarding_feature_list_T *ff_list, byte *last_octet_parameters_buf)
{
   int                       ff_index = 0, forward_to_number = 0;
   ie_ss_status_T            cm_ss_status;
   int                       msg_index,sequence_last_octet,list_last_octet;
   ie_forwarding_options_T    *fwd_options;
   cm_called_party_subaddress_T  *subaddress;
   byte                      subaddress_size,*pointer;
   boolean                   success;
   return_size_T             size;

   msg_index = *index;
   success = TRUE;

   if(msg_index >= MAX_SS_PARAMETER_LENGTH)
   {
     return FALSE;
     /*
      * parameters is MAX_SS_PARAMETER_LENGTH byte array
      * Its invalid to access its 260th or greater index
      */
   }

   pointer=&(parameters[msg_index]);

   size = MN_get_element_length(pointer,SS_MAX_FF_LIST_LENGTH,(last_octet_parameters_buf - pointer + 1));

   msg_index+= size.increment;

   ff_list->present = TRUE;

   list_last_octet = size.length + msg_index;
   
   if(list_last_octet >= MAX_SS_PARAMETER_LENGTH)
   {
     list_last_octet = MAX_SS_PARAMETER_LENGTH;
   }
   
   if(&(parameters[list_last_octet - 1]) > last_octet_parameters_buf)
   {
     success = FALSE;
     list_last_octet = msg_index; /* This is done to skip processing inside while loop and also not alter msg_index value */
   }

   while ((&(parameters[msg_index]) <= last_octet_parameters_buf) && (msg_index < list_last_octet))
   {

      switch (parameters[msg_index++])
      {

          case SEQUENCE_TAG:
          {

            /* Get the sequence size and calculate the position of the last octet */
            size = MN_get_element_length(&(parameters[msg_index]),SS_MAX_FF_LENGTH,(last_octet_parameters_buf - pointer + 1));
            msg_index += size.increment;

            sequence_last_octet = (msg_index + size.length);

            if(sequence_last_octet >= list_last_octet)
            {
              sequence_last_octet = list_last_octet;
            }

            if(&(parameters[sequence_last_octet]) > last_octet_parameters_buf)
            {
              sequence_last_octet = sequence_last_octet - (&(parameters[sequence_last_octet]) - last_octet_parameters_buf); /* sequence_last_octet cannot point beyond last octet of the buffer */
            }

            while ((&(parameters[msg_index]) <= last_octet_parameters_buf) && (msg_index < sequence_last_octet))

            {

               switch (parameters[msg_index])
               {

               case NULL:  /* Possible null IE - end of sequence */

                  {

                     msg_index++;

                     if(msg_index >= sequence_last_octet)
                     {
                       /*
                        * In this case, one more octet from parmeters array needs to be read.
                        * To prevent read overflow, this check is needed.
                        */
                       break;
                     }

                     if (parameters[msg_index] !=0)
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid null IE length value in parameter list");

                        success = FALSE;
                     }
                     else
                     {
                        success = TRUE;
                        msg_index++;
                     }
                  /* Exit the while loop */

                     msg_index = sequence_last_octet;
                     break;
                  }
   

               case 0x82:  /* bearer_service */
                  {

                     if(msg_index >= sequence_last_octet - 2)
                     {
                       /*
                        * This case is valid if reading at-least three octets from parameter
                        * array won't cause read-overflow. Otherwise, we need not proceed with
                        * any more octets and break.
                        */
                       msg_index = sequence_last_octet;
                       break;
                     }

                     ff_list->forwarding_feature[ff_index].basic_service.present = TRUE;
                     msg_index++;
                     if (parameters[msg_index++] != 1)
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length value in parameter list");

                        success = FALSE;
                     }

                     ff_list->forwarding_feature[ff_index].basic_service.code_type = BS_CODE;
                     ff_list->forwarding_feature[ff_index].basic_service.bs_code =
                       parameters[msg_index];
                     msg_index++;
                     break;
                  }

               case 0x83:  /* tele service */
                  {

                     if(msg_index >= sequence_last_octet - 2)
                     {
                       /*
                        * This case is valid if reading at-least three octets from parameter
                        * array won't cause read-overflow. Otherwise, we need not proceed with
                        * any more octets and break.
                        */
                       msg_index = sequence_last_octet;
                       break;
                     }

                     ff_list->forwarding_feature[ff_index].basic_service.present = TRUE;
                     msg_index++;
                     if (parameters[msg_index++] != 1)
                     {
                         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length value in parameter list");

                        success = FALSE;
                     }

                     ff_list->forwarding_feature[ff_index].basic_service.code_type = TS_CODE;
                     ff_list->forwarding_feature[ff_index].basic_service.bs_code =
                         parameters[msg_index];
                     msg_index++;
                     break;
                  }

               case 0x84:  /* ss status */
                  {

                     if(msg_index >= sequence_last_octet - 2)
                     {
                       /*
                        * This case is valid if reading at-least three octets from parameter
                        * array won't cause read-overflow. Otherwise, we need not proceed with
                        * any more octets and break.
                        */
                       msg_index = sequence_last_octet;
                       break;
                     }

                     msg_index ++;
                     if (parameters[msg_index++] != 1)
                     {
                         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length value in forwarding feature list");

                        success = FALSE;
                     }

                     MN_get_ss_status(&cm_ss_status, &(parameters[msg_index]));
                     ff_list->forwarding_feature[ff_index].ss_status = cm_ss_status;
                     msg_index++;
                     break;
                  }

               case 0x85:  /* forwardedToNumber */
               case 0x89:  /* longfowardedToNumber */
                  {
                     msg_index++;

                     if(msg_index >= sequence_last_octet)
                     {
                       /*
                        * This case is valid if reading at-least one more octet from parameter
                        * array won't cause read-overflow. Otherwise, we need not proceed with
                        * any more octets and break.
                        */
                       break;
                     }


                     if(forward_to_number < (2*MAXNO_FORWARD_TO_NUMBER))
                     {
                         if(MN_get_ft_number(parameters, 
                                          &msg_index,
                                          &ff_list->forwarding_feature[ff_index].forwarded_to_number[forward_to_number],
                                          sequence_last_octet))
                         {
                          
                             forward_to_number++;
                         }
                     }
                     else 
                     {
                         msg_index += parameters[msg_index] + 1;  /* skip this number */ 
                     }

                     break;
                  }
 
               case 0x86:  /* forwarding options */
                  {

                     if(msg_index >= sequence_last_octet - 2)
                     {
                       msg_index = sequence_last_octet;
                       break;  // to prevent read overflow
                     }

                     msg_index++;

                     /* set up a pointer to reduce code */

                     fwd_options = &(ff_list->forwarding_feature[ff_index].forwarding_options);

                     fwd_options->present = TRUE;

                     if (parameters[msg_index++] != 1)
                     {
                         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length element in parameter list");

                        success = FALSE;
                     }

                     /* fixed compile warning C4244 */
                     fwd_options->forwarding_reason =
                      (byte)((parameters[msg_index] & 0x0c) / 4);

                     if ((parameters[msg_index] & 0xc0) > 0)
                     {
                        fwd_options->notification_to_fp = TRUE;
                     }
                     else
                     {
                        fwd_options->notification_to_fp = FALSE;
                     }

                     if ((parameters[msg_index] & 0x30) > 0)
                     {
                        fwd_options->notification_to_cp = TRUE;
                     }
                     else
                     {
                        fwd_options->notification_to_cp = FALSE;
                     }

                     msg_index++;

                     break;
                  }

               case 0x87:  /* noReplyConditionTime */
                  {

                     if(msg_index >= sequence_last_octet - 2)
                     {
                       msg_index = sequence_last_octet;
                       break;  // to prevent read overflow
                     }

                     ff_list->forwarding_feature[ff_index].no_reply_condition_timer.present =
                         TRUE;
                     msg_index++;

                     if (parameters[msg_index++] != 1)
                     {
                         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length element in parameter list");

                        success = FALSE;
                     }

                     ff_list->forwarding_feature[ff_index].no_reply_condition_timer.timer_value
                      =
                         parameters[msg_index];
                     msg_index++;

                     break;
                  }

               case 0x88:  /* forwardedToSubaddress */
                  {

                     if(msg_index >= sequence_last_octet - 2)
                     {
                       msg_index = sequence_last_octet;
                       break;  // to prevent read overflow
                     }

                  /* Set up a pointer to save code */

                     subaddress = &(ff_list->forwarding_feature[ff_index].forwarded_to_subaddress);

                     subaddress->present = TRUE;

                     msg_index++;

                     subaddress_size = parameters[msg_index++];

                     if ((subaddress_size > CM_CALLED_PARTY_SUBADDRESS_LENGTH) ||
                          ((msg_index + subaddress_size) >= sequence_last_octet)) /* Max size all
                                            owed in 04.80 */
                     {
                         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Subaddress length element in parameter list too large"); 

                        success = FALSE;
                     }
                     else
                             /* Copy subaddress data in to ff list */
                     {

                        memscpy(subaddress->data, sizeof(subaddress->data) ,&parameters[msg_index],subaddress_size);
                        subaddress->length = subaddress_size;

                     }


                  /* point the index at the next element */

                     msg_index += subaddress_size;

                     break;
                  }

               default:
                  {
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid argument identifier in parameter lis");
 
                      success = FALSE;

                      /* Make execution leave the 'while' loop */
                      msg_index = sequence_last_octet;

                      break;
                  }
               }             /* End of switch statement */
            }   /* End of while statement */

            ff_index ++;   /* increment for next loop */

            if(ff_index >= FWD_FEATURE_LIST_SIZE)
            {
              msg_index = list_last_octet;
            }

            forward_to_number = 0;  /* reset index of ff_list->forwarding_feature[ff_index].forwarded_to_number[forward_to_number] */

            break;

         } /* End of case SEQUENCE_TAG  */
         
         case NULL:
         {
            msg_index++;

            /* Null IE - end of sequence */
            if (parameters[msg_index] != 0x00)
            {
                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid null parameter length");

                success = FALSE;
            }
            else
            {
                success = TRUE;
                msg_index++;
            }

            /* break the sequence */
            msg_index = list_last_octet;
            break;
         }  /* end of case null */

         default:
         {

             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid octet");

            success = FALSE;
            msg_index = list_last_octet;
            break;

         }  /* end of case default */

      } /* end of switch statement */

   } /* End of while statement */

   ff_list->forwarding_feature_list_length = (byte)ff_index;

   *index = msg_index;
   return(success);
}

#ifdef FEATURE_CCBS
/* ****************************************************
 * PARAMETERS
 *  param  : points to the begining of length octet of SEQUENCE
 *  msg_idx: reference to index to be moved. it will be moved to the first octet
 *           after this SEQUENCE (including 0x00 0x00)
 *  ccbs_ptr   : CCBS feature container
 *
 * RETURN
 *  TRUE if success, FALSE if failed
 * **************************************************** */
boolean MN_get_ccbs_feature (byte *param, int *msg_idx, ie_ccbs_indicator_T *ccbs_ptr, byte *last_octet_data_pointer)
{
   boolean                        success=TRUE;
   int                            idx=0, addr_idx=0;
   cm_called_party_subaddress_T  *subaddr;
   return_size_T                  size;
   int                            sequence_last_octet;
   byte                           subaddr_size;

   ccbs_ptr->present = TRUE;

   /* Get the sequence size and calculate the position of the last octet */
   size = MN_get_element_length(param,SS_MAX_CCBS_FEATURE_LENGTH,(last_octet_data_pointer - param + 1));
   idx += size.increment;

   sequence_last_octet = idx + size.length;

   if (*param == 0x80)
   {
      sequence_last_octet += 2;  /* for the last 0x00 0x00 */
   }

   if(&(param[sequence_last_octet - 1]) > last_octet_data_pointer)
   {
     success = FALSE;
     sequence_last_octet = idx; 
   }
   
   while (idx<sequence_last_octet && success)
   {
      switch (param[idx])
      {
         case NULL:  /* Possible null IE - end of sequence */
            idx++;
            if (param[idx++] !=0)
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid null IE length value in parameter list");
               success = FALSE;
            }
            break;

         case (CONTEXT_SPECIFY_PRIMITIVE|0):  /* ccbs-Index */
            idx++;
            if (param[idx++] != 1)
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length element in parameter list");
               success = FALSE;
            }

            ccbs_ptr->ccbsIndex.present    = TRUE;
            ccbs_ptr->ccbsIndex.ccbs_Index = param[idx++];
            break;

         case (CONTEXT_SPECIFY_PRIMITIVE|1):  /* b-subscriberNumber */
            idx++;

            if (addr_idx < 2)
            {
               if ( MN_get_ft_number(param, &idx, &ccbs_ptr->subscriberNumber[addr_idx], sequence_last_octet) )
                  addr_idx++;
            }
            else 
            {
               idx += param[idx] + 1;  /* skip this number */ 
            }
            break;

         case (CONTEXT_SPECIFY_PRIMITIVE|2):  /* b-subscriberSubaddress */
            subaddr = &ccbs_ptr->subscriberAddress;
            subaddr->present = TRUE;

            idx++;

            subaddr_size = param[idx++];

            if (subaddr_size <= CM_CALLED_PARTY_SUBADDRESS_LENGTH) /* Max size allowed in 04.80 */
            {
               memscpy(subaddr->data, sizeof(subaddr->data) ,&param[idx], subaddr_size);
               subaddr->length = subaddr_size;
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Subaddress length element in parameter list too large"); 
               success = FALSE;
            }

            /* point the index at the next element */
            idx += subaddr_size;
            break;

         case (CONTEXT_SPECIFY_CONSTRUCT|3):  /* basicServiceGroup */
            ccbs_ptr->basicService.present = TRUE;
            idx++;

            if (param[idx++] != 3)
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length value in parameter list");
               success = FALSE;
            }
            else
            {
               switch (param[idx++])
               {
                  case CONTEXT_SPECIFY_PRIMITIVE|2:  /* bearerService */
                     if (param[idx++] != 1)
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length value in parameter list");
                        success = FALSE;
                     }
                     ccbs_ptr->basicService.code_type = BS_CODE;
                     ccbs_ptr->basicService.bs_code   = param[idx++];
                     break;

                  case CONTEXT_SPECIFY_PRIMITIVE|3:  /* teleservice */
                     if (param[idx++] != 1)
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length value in parameter list");
                        success = FALSE;
                     }

                     ccbs_ptr->basicService.code_type = TS_CODE;
                     ccbs_ptr->basicService.bs_code   = param[idx++];
                     break;

                  default:
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid basic svc grp");
                     success = FALSE;
                     break;
               }
            }
            break;

         default:
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid argument identifier in parameter list");
            success = FALSE;
            break;
      }
   }
   
   *msg_idx += idx;

   return success;
}



/*
 * Function name :  MN_get_ccbs_list
 * -------------
 *
 * Description :  Get the data from forwarding feature list
 * -----------    Enter routine with [index] pointing a first
 *                octet of length element
 *
 *
 *
 *
 *
 * Parameters: parameters
 * ----------  msg_index
 *             
 *
 *
 * Returns:    forwarding feature list,success
 * --------
 *
 */
boolean MN_get_ccbs_list(byte *parameters, int *index, ccbs_info_T *ccbs_ptr, byte *last_octet_parameters_buf)
{
  int             ccbs_index=0, list_last_octet, msg_index=*index;
  byte           *pointer;
  boolean         success=TRUE;
  return_size_T   size;

  pointer = &parameters[msg_index];

  size = MN_get_element_length(pointer,SS_MAX_CCBS_FEATURE_LIST_LENGTH, (last_octet_parameters_buf - pointer + 1));

  msg_index       += size.increment;
  list_last_octet  = size.length + msg_index;

  if (*pointer == 0x80)
  {
    list_last_octet += 2;
  }

  if(&(pointer[list_last_octet-1]) > last_octet_parameters_buf)
  {
    success = FALSE;
    list_last_octet = msg_index;
  }
  
  while (msg_index<list_last_octet && success)
  {
    switch (parameters[msg_index++])
    {
      case SEQUENCE_TAG:
        success = MN_get_ccbs_feature ( &parameters[msg_index],
                                        &msg_index,
                                        &ccbs_ptr->ccbs_feature_list[ccbs_index],
                                        last_octet_parameters_buf);

        ccbs_index++;   /* increment for next loop */
        break;

      case 0x00:
        if (parameters[++msg_index] != 0x00)
        {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid null parameter length");
          success = FALSE;
        }
        break;

      default:
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid octet");
        success = FALSE;
        break;
    }
  }

  if (success)
  {
    ccbs_ptr->present = TRUE;
    ccbs_ptr->ccbs_feature_list_length = (byte)ccbs_index;
  }

  *index = msg_index;
  return success;
}
#endif /* FEATURE_CCBS */


/*
 * Function name :  MN_get_ft_number
 * -------------
 *
 * Description :  Get the data from forward to number list
 * -----------
 *
 *
 *
 *
 *
 *
 * Parameters: parameters
 * ----------  msg_index
 *             
 *
 *
 * Returns:    ft_number,success
 * --------
 *
 */
boolean MN_get_ft_number(byte *parameters, int *index, cm_called_party_bcd_no_T *ft_number, int sequence_end)
{
   int msg_index;
   boolean  success;

   msg_index = *index;
   success = TRUE;

   ft_number->present = TRUE;

   if ((parameters[msg_index] > CM_CALLED_PARTY_BCD_NO_LENGTH) ||
        ((msg_index + parameters[msg_index]) >= sequence_end))
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid forwarded to number length element");

      success = FALSE;
   }
   else
   {
      ft_number->length = parameters[msg_index++];

      memscpy(&(ft_number->data[0]), sizeof(ft_number->data) ,&(parameters[msg_index]), ft_number->length);

      msg_index += ft_number->length;    /* increment index by length of number */

      *index = msg_index;
   }

   return(success);
}


/*
 * Function name :  MN_release_ci_invoke_id
 * -------------
 *
 * Description :  Releases invoke data for a MT operation
 * -----------
 *
 * Parameters: connection id
 * ----------  invoke id
 *
 *
 * Returns:    success
 * --------
 *
 */

/*   When a NW triggers a MT USSD in response to a MO USSD,
       For Tearing down both the USSD sessions, from MO side, 
       UI/CM client should always teardown the MO USSD and MT USSD will be automatically torn down. 
       BUT Vice Versa is not Supported.
*/


boolean MN_release_ci_invoke_id( byte            invoke_id,
                                 byte            ss_operation,
                                 connection_id_T *connection_id_ptr,
                                 boolean         release)
{
   boolean success;
   int      i;

   /* These transaactions already have a transaction ID from the network */

   /* Find if the invoke id has been stored, if so, release and return the 
            corresponding connection id and set success true */


   for( success = FALSE,i = 0;
        ((i<MAXNO_INVOKE_IDS)&&(success==FALSE));
        i++)

   {

      if ((ss_invoke_data[i].mt_invoke_id_valid == TRUE)&&
          (ss_invoke_data[i].mt_invoke_id == invoke_id)&&
          (ss_invoke_data[i].mt_ss_operation == ss_operation))
      {

         success = TRUE;
         *connection_id_ptr = ss_invoke_data[i].mt_connection_id;

         if ( release == TRUE)
         {
            ss_invoke_data[i].mt_invoke_id_valid = FALSE;
/*            ss_invoke_data[i].mt_ss_operation = NULL_SS_OPERATION; */
/* The above mt_ss_operation field is used to reject a new MT USSD session while the old MT USSD session
    has not yet been torn down  ( PTCRB TC ) */

         }
      }
   }

   return(success);
}


/*
 * Function name :  MN_release_ci_invoke_id_end
 * -------------
 *
 * Description :  Releases invoke data for a MT operation
 * -----------
 *
 * Parameters: connection id
 * ----------  invoke id
 *
 *
 * Returns:    success
 * --------
 *
 */

boolean MN_release_ci_invoke_id_end(connection_id_T connection_id)
{
   boolean success;
   int      i;

   /* These transaactions already have a transaction ID from the network */

   /* Find if the invoke id has been stored, if so, release and return the 
            corresponding connection id and set success true */


   for( success = FALSE,i = 0;
        (i<MAXNO_INVOKE_IDS);
        i++)
   {

      if ((ss_invoke_data[i].mt_invoke_id_valid == TRUE)&&
          (ss_invoke_data[i].mt_connection_id == connection_id))
      {

         success = TRUE;
         ss_invoke_data[i].mt_invoke_id_valid = FALSE;
      }
      else if(ss_invoke_data[i].mt_invoke_id_valid == FALSE)
      {
          ss_invoke_data[i].mt_connection_id = UNKNOWN_CONNECTION_ID;   
          ss_invoke_data[i].mt_ss_operation = NULL_SS_OPERATION;
      }
   }

   return(success);
}


/*
 * Function name :  MN_request_ci_invoke_id
 * -------------
 *
 * Description :  Requests invoke id for a MO SS operation
 * -----------
 *
 * Parameters: connection id
 * ----------  invoke id
 *
 * Returns:    boolean (last_invoke_id)
 * --------
 *
 */
boolean MN_request_ci_invoke_id( byte *invoke_id,               /*These 3 are*/
                                 connection_id_T *connection_id,/*values to be*/
                                 boolean *call_path_available,    /* returned */
                                 byte ss_operation,
                                 byte cm_ss_ref)
{

   /* Local declarations  */

   boolean success = FALSE;
   boolean invoke_success = FALSE;
   uint8 index;

 if((connection_id != NULL)&&
       (invoke_id != NULL)&&
       (call_path_available!= NULL))
 {
   *call_path_available = FALSE;

   /* 
    * MO SS transactions require a new invoke id and a new connection id
    * unless a connection id already being used for CI SS is available  
    */
   for (index = 0; (index < UNKNOWN_CONNECTION_ID)&&(success == FALSE); index++)
   {
      if (CNM_get_ss_state(index) == SS_IDLE)
      {
         *connection_id = (connection_id_T)index;

         *call_path_available = FALSE;

         success = TRUE;
      }
   }

   if (success == TRUE)
   {
      /* Get an invoke id */

      invoke_success = FALSE;
      for (index = 0;
          (index < MAXNO_INVOKE_IDS)&&(invoke_success == FALSE);
          index++)
      {

         if(ss_invoke_data[index].ss_stored_operation == NULL_SS_OPERATION)
         {
            *invoke_id = (byte)index;
            ss_invoke_data[index].ss_password_present = FALSE;
            ss_invoke_data[index].ss_password.present = FALSE;
            ss_invoke_data[index].ss_new_password_present = FALSE;
            ss_invoke_data[index].ss_new_password.present = FALSE;
            ss_invoke_data[index].ss_new_password_again_present = FALSE;
            ss_invoke_data[index].ss_new_password_again.present = FALSE;

            ss_invoke_data[index].ss_stored_operation = ss_operation;
            ss_invoke_data[index].cm_ss_ref = cm_ss_ref;
            ss_invoke_data[index].connection_id = *connection_id;
            invoke_success = TRUE;
         }
      }
   }

   if (invoke_success != TRUE)
   {
      *invoke_id = 0xff;
   }
   else
   {
      /* Special processing of USSD Cross Phase Compatible procedures */
       if(ss_operation == processUnstructuredSS_Data)
       {
           for(index =0; index <  MAXNO_INVOKE_IDS; index++)
           {
               if(ss_invoke_data[index].ss_stored_operation == processUnstructuredSS_Request)
               {
                   *call_path_available = TRUE;  /* don't force new MM connection if the
                                                    processUnstructureSS_Req operation 
                                                    had not been closed */
               } 
               else
               {
                   *call_path_available = FALSE;  /* force a new MM connection to be establish,
                                                     even though one may already be existed */
               }
           } 
       }
   }
 }
   else
   {
       success = FALSE;

       MSG_ERROR_DS_3(MN_SUB,"=MNCNM= Invalid ptr: connection_id=%d, invoke_id=%d, call_path_available=%d", 
                             connection_id,
                             invoke_id,
                             call_path_available);
   }

   return (success);
}



/*
 * Function name :  MN_select_ss_error
 * -------------
 *
 * Description :  Requests invoke data
 * -----------
 *
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  ss_operation
 *             cm_ss_error
 *
 *
 * Returns:    none
 * --------
 *
 */

VOID_FUNC MN_select_ss_error( byte              cm_ss_ref,
                              byte              ss_operation,
#if defined(FEATURE_DUAL_SIM )
                              sys_modem_as_id_e_type as_id,
#endif
                              ie_cm_ss_error_T cm_ss_error)
{

  InterrogateSS_Res_T  *InterrogateSS_Res;
  ss_info_T            *ss_info;
  ss_password_info_T   *ss_password_info;
  ss_UserData_info_T   *ss_UserData_info;
  ss_data_info_T       *ss_data_info;
#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
  lcs_molr_res_T       *lcs_molr_res;
#endif

  switch (ss_operation)
  {
  
    case interrogateSS:
    {
#ifndef FEATURE_MODEM_HEAP
      InterrogateSS_Res = (InterrogateSS_Res_T *) &buffer.a;
#else
      InterrogateSS_Res = (InterrogateSS_Res_T *) modem_mem_calloc(1, sizeof(InterrogateSS_Res_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);
      if (InterrogateSS_Res != NULL)
      {
#endif
      
        memset(InterrogateSS_Res, NULL, sizeof(InterrogateSS_Res_T));

        InterrogateSS_Res->cm_ss_error = cm_ss_error;

        MN_put_cm_interrogate_ss_conf(TRUE, 
                                       cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                       as_id,
#endif
                                       SS_ERROR_INFO,
                                       InterrogateSS_Res);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(InterrogateSS_Res, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_interrogate_ss_conf(FALSE, 
                                       cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                       as_id,
#endif
                                       NO_INFO,
                                       InterrogateSS_Res);
      }
#endif
      break;
    }

    case registerSS:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer.b;
#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      /* Wipe the data space */
      if (ss_info != NULL)
      {
#endif
        memset(&(ss_info->info), NULL, sizeof(ie_cm_ss_error_T));

        /* Copy the ss error info */
        ss_info->info_type = SS_ERROR_INFO;

        memscpy(&(ss_info->info), sizeof(ss_info->info) ,&cm_ss_error, sizeof(ie_cm_ss_error_T));

        MN_put_cm_register_ss_conf(TRUE, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_register_ss_conf(FALSE, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);
      }
#endif
      break;
    }

    case eraseSS:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer.b;
#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);
      if (ss_info != NULL)
      {
#endif

        /* Wipe the data space */
        memset(&(ss_info->info), NULL, sizeof(ie_cm_ss_error_T));

        ss_info->info_type = SS_ERROR_INFO;
        memscpy(&(ss_info->info), sizeof(ss_info->info) ,&cm_ss_error, sizeof(ie_cm_ss_error_T));

        MN_put_cm_erase_ss_conf(TRUE, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_erase_ss_conf(FALSE, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 ss_info);
      }  
#endif
      break;
    }

    case activateSS:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer.b;
#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);
      if (ss_info != NULL)
      {
#endif
        /* Wipe the data space */

        memset(&(ss_info->info), NULL, sizeof(ie_cm_ss_error_T));

        ss_info->info_type = SS_ERROR_INFO;
        memscpy(&(ss_info->info), sizeof(ss_info->info) ,&cm_ss_error, sizeof(ie_cm_ss_error_T));

        MN_put_cm_activate_ss_conf(TRUE, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_activate_ss_conf(FALSE, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);
      } 
#endif
      break;
    }

    case deactivateSS:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer.b;
#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_info != NULL)
      {
#endif

        /* Wipe the data space */

        memset(&(ss_info->info), NULL, sizeof(ie_cm_ss_error_T));

        ss_info->info_type = SS_ERROR_INFO;
        memscpy(&(ss_info->info), sizeof(ss_info->info) ,&cm_ss_error, sizeof(ie_cm_ss_error_T));

        MN_put_cm_deactivate_ss_conf(TRUE, 
                                      cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                      as_id,
#endif
                                      ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);

        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_deactivate_ss_conf(FALSE, 
                                      cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                      as_id,
#endif
                                      ss_info);
      }  

#endif
      
      break;

    }

    case registerPassword:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_password_info = (ss_password_info_T *) &buffer.c;
#else
      ss_password_info = (ss_password_info_T *) modem_mem_calloc(1, sizeof(ss_password_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_password_info != NULL)
      {
#endif
        /* Wipe the data space */

        memset(&(ss_password_info->data), NULL, sizeof(ie_cm_ss_error_T));

        ss_password_info->data_type = SS_ERROR_INFO;
        memscpy(&(ss_password_info->data), sizeof(ss_password_info->data) ,&cm_ss_error, sizeof(ie_cm_ss_error_T));

        MN_put_cm_register_password_conf(TRUE, 
                                          cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                           as_id,
#endif
                                          ss_password_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_password_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_register_password_conf(FALSE, 
                                          cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                           as_id,
#endif
                                          ss_password_info);
      }  
#endif
      break;
    }

    case processUnstructuredSS_Data:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_UserData_info = (ss_UserData_info_T *) &buffer.d;

#else
      ss_UserData_info = (ss_UserData_info_T *) modem_mem_calloc(1, sizeof(ss_UserData_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_UserData_info != NULL)
      {

#endif
        /* Wipe the data space */

        memset(&(ss_UserData_info->data), NULL, sizeof(ie_cm_ss_error_T));

        ss_UserData_info->data_type = SS_ERROR_INFO;
        memscpy(&(ss_UserData_info->data), sizeof(ss_UserData_info->data) ,&cm_ss_error, sizeof(ie_cm_ss_error_T));

        MN_put_cm_process_unstructured_ss_data_conf(TRUE, 
                                                     cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                     as_id,
#endif
                                                     ss_UserData_info);


#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_UserData_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_process_unstructured_ss_data_conf(FALSE, 
                                                     cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                     as_id,
#endif
                                                     ss_UserData_info);
      }    

#endif
      break;
    }

    case processUnstructuredSS_Request:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_data_info = (ss_data_info_T *) &buffer.e;

#else
      ss_data_info = (ss_data_info_T *) modem_mem_calloc(1, sizeof(ss_data_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_data_info != NULL)
      {

#endif
        /* Wipe the data space */

        memset(&(ss_data_info->data), NULL, sizeof(ie_cm_ss_error_T));
        ss_data_info->data_type = SS_ERROR_INFO;
        memscpy(&(ss_data_info->data), sizeof(ss_data_info->data) ,&cm_ss_error, sizeof(ie_cm_ss_error_T));

        MN_put_cm_process_unstructured_ss_conf(TRUE, 
                                                cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                as_id,
#endif
                                                ss_data_info);


#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_data_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_process_unstructured_ss_conf(FALSE, 
                                                cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                as_id,
#endif
                                                ss_data_info);
      }  

#endif

      break;
    }

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
    case lcs_MOLR:
    {

#ifndef FEATURE_MODEM_HEAP
      lcs_molr_res = (lcs_molr_res_T *) &buffer.f;

#else
      lcs_molr_res = (lcs_molr_res_T *) modem_mem_calloc(1, sizeof(lcs_molr_res_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (lcs_molr_res != NULL)
      {

#endif

        /* Wipe the data space */

        memset(&(lcs_molr_res->data), NULL, sizeof(ie_cm_ss_error_T));
        lcs_molr_res->data_type = SS_ERROR_INFO;
        memscpy(&(lcs_molr_res->data), sizeof(lcs_molr_res->data) ,&cm_ss_error, sizeof(ie_cm_ss_error_T));

        MN_put_cm_lcs_molr_conf(TRUE, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 lcs_molr_res);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(lcs_molr_res, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_lcs_molr_conf(FALSE, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 lcs_molr_res);
      }
#endif

      break;
    }
#endif

#ifdef FEATURE_CCBS
    case eraseCCEntry:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer.b;

#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_info != NULL)
      {
#endif

        /* Wipe the data space */
        memset(&ss_info->info, 0x00, sizeof(ie_cm_ss_error_T));

        /* Copy the ss error info */
        ss_info->info_type = SS_ERROR_INFO;
        memscpy(&ss_info->info, sizeof(ss_info->info) ,&cm_ss_error, sizeof(ie_cm_ss_error_T));

        MN_put_cm_erase_cc_entry_conf (TRUE, 
                                        cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                        as_id,
#endif
                                        ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_erase_cc_entry_conf (FALSE, 
                                        cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                        as_id,
#endif
                                        NULL);
      }
#endif
  
      break;
    }
#endif /* FEATURE_CCBS */

    default:
    {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid SS operation type"); 

      break;
    }
  }
}




/*
 * Function name :  MN_select_cc_cause
 * -------------
 *
 * Description :  Requests invoke data
 * -----------
 *
 * Parameters: cm_ss_ref
 * ----------  ss_operation
 *             cm_cc_cause
 *
 * Returns:    none
 * --------
 *
 */

VOID_FUNC MN_select_cc_cause( byte              cm_ss_ref,
                              byte              ss_operation,
#if defined(FEATURE_DUAL_SIM )
                              sys_modem_as_id_e_type as_id,
#endif
                              ie_cm_cc_cause_T  cm_cc_cause)
{

  InterrogateSS_Res_T *InterrogateSS_Res;
  ss_info_T           *ss_info;
  ss_password_info_T  *ss_password_info;
  ss_UserData_info_T  *ss_UserData_info;
  ss_data_info_T      *ss_data_info;

  cm_cc_cause.present = TRUE;  /* for CM */

  switch (ss_operation)
  {
  
    case interrogateSS:
    {

#ifndef FEATURE_MODEM_HEAP
      InterrogateSS_Res = (InterrogateSS_Res_T *) &buffer;
#else
      InterrogateSS_Res = (InterrogateSS_Res_T *) modem_mem_calloc(1, sizeof(InterrogateSS_Res_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (InterrogateSS_Res != NULL)
      {

#endif

        InterrogateSS_Res->cm_cc_cause = cm_cc_cause;

        MN_put_cm_interrogate_ss_conf(TRUE, 
                                       cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                       as_id,
#endif
                                       CC_CAUSE_INFO,
                                       InterrogateSS_Res);
#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(InterrogateSS_Res, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_interrogate_ss_conf(FALSE, 
                                       cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                       as_id,
#endif
                                       NO_INFO,
                                       InterrogateSS_Res);
      }
#endif

      break;
    }

    case registerSS:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer;

#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_info != NULL)
      {
#endif

        memset(&(ss_info->info),  NULL,  sizeof(ie_cm_cc_cause_T));

        ss_info->info_type = CC_CAUSE_INFO;
        memscpy(&(ss_info->info), sizeof(ss_info->info) ,&cm_cc_cause, sizeof(ie_cm_cc_cause_T));

        MN_put_cm_register_ss_conf(TRUE, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_register_ss_conf(FALSE, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);
      }
#endif

      break;
    }

    case eraseSS:
    {

#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer;
#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);
      if (ss_info != NULL)
      {
#endif

        memset(&(ss_info->info),  NULL,  sizeof(ie_cm_cc_cause_T));

        ss_info->info_type = CC_CAUSE_INFO;
        memscpy(&(ss_info->info), sizeof(ss_info->info) ,&cm_cc_cause, sizeof(ie_cm_cc_cause_T));

        MN_put_cm_erase_ss_conf(TRUE, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_erase_ss_conf(FALSE, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 ss_info);
      } 
#endif
      break;
    }

    case activateSS:
    {

#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer;

#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);


      if (ss_info != NULL)
      {
#endif

        memset(&(ss_info->info),  NULL,  sizeof(ie_cm_cc_cause_T));

        ss_info->info_type = CC_CAUSE_INFO;
        memscpy(&(ss_info->info), sizeof(ss_info->info) ,&cm_cc_cause, sizeof(ie_cm_cc_cause_T));

        MN_put_cm_activate_ss_conf(TRUE, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_activate_ss_conf(FALSE, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);
      }  
#endif

      break;
    }

    case deactivateSS:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer;
#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_info != NULL)
      {
#endif

        memset(&(ss_info->info),  NULL,  sizeof(ie_cm_cc_cause_T));

        ss_info->info_type = CC_CAUSE_INFO;
        memscpy(&(ss_info->info), sizeof(ss_info->info) ,&cm_cc_cause, sizeof(ie_cm_cc_cause_T));

        MN_put_cm_deactivate_ss_conf(TRUE, 
                                      cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                      as_id,
#endif
                                      ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_deactivate_ss_conf(FALSE, 
                                      cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                      as_id,
#endif
                                      ss_info);
      }
#endif
         
      break;

    }

    case registerPassword:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_password_info = (ss_password_info_T *) &buffer;  
#else
      ss_password_info = (ss_password_info_T *) modem_mem_calloc(1, sizeof(ss_password_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);


      if (ss_password_info != NULL)
      {
#endif

        memset(&(ss_password_info->data), NULL,  sizeof(ie_cm_cc_cause_T));

        ss_password_info->data_type = CC_CAUSE_INFO;
        memscpy(&(ss_password_info->data), sizeof(ss_password_info->data) ,&cm_cc_cause, sizeof(ie_cm_cc_cause_T));

        MN_put_cm_register_password_conf(TRUE, 
                                          cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                           as_id,
#endif
                                          ss_password_info);


#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_password_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_register_password_conf(FALSE, 
                                          cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                           as_id,
#endif
                                          ss_password_info);
      }
#endif
      
      break;
    }

    case processUnstructuredSS_Data:
    {
#ifndef FEATURE_MODEM_HEAP

      ss_UserData_info = (ss_UserData_info_T *) &buffer;       

#else
      ss_UserData_info = (ss_UserData_info_T *) modem_mem_calloc(1, sizeof(ss_UserData_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);


      if (ss_UserData_info != NULL)
      {
#endif

        memset(&(ss_UserData_info->data),  NULL,  sizeof(ie_cm_cc_cause_T));

        ss_UserData_info->data_type = CC_CAUSE_INFO;
        memscpy(&(ss_UserData_info->data), sizeof(ss_UserData_info->data) ,&cm_cc_cause, sizeof(ie_cm_cc_cause_T));

        MN_put_cm_process_unstructured_ss_data_conf(TRUE, 
                                                     cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                     as_id,
#endif
                                                     ss_UserData_info);     

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_UserData_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_process_unstructured_ss_data_conf(FALSE, 
                                                     cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM ) 
                                                     as_id,
#endif
                                                     ss_UserData_info);
      }
#endif
      
      break;
    }

    case processUnstructuredSS_Request:
    {
#ifndef FEATURE_MODEM_HEAP
      ss_data_info = (ss_data_info_T *) &buffer;
#else
      ss_data_info = (ss_data_info_T *) modem_mem_calloc(1, sizeof(ss_data_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);


      if (ss_data_info != NULL)
      {
#endif

        memset(&(ss_data_info->data), NULL,  sizeof(ie_cm_cc_cause_T));

        ss_data_info->data_type = CC_CAUSE_INFO;
        memscpy(&(ss_data_info->data), sizeof(ss_data_info->data) ,&cm_cc_cause, sizeof(ie_cm_cc_cause_T));

        MN_put_cm_process_unstructured_ss_conf(TRUE, 
                                                cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                as_id,
#endif
                                                ss_data_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_data_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_process_unstructured_ss_conf(FALSE, 
                                                cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                as_id,
#endif
                                                ss_data_info);
      }
#endif
         
      break;
    }

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
    case lcs_MOLR:
    {
#ifndef FEATURE_MODEM_HEAP
      lcs_molr_res_T  *lcs_molr_res = (lcs_molr_res_T *)&buffer;

#else
      lcs_molr_res_T  *lcs_molr_res = (lcs_molr_res_T *) modem_mem_calloc(1, sizeof(lcs_molr_res_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (lcs_molr_res != NULL)
      {

#endif

        memset(lcs_molr_res, NULL, sizeof(lcs_molr_res_T));

        lcs_molr_res->data_type = CC_CAUSE_INFO;
        memscpy(&(lcs_molr_res->data), sizeof(lcs_molr_res->data) ,&cm_cc_cause, sizeof(ie_cm_cc_cause_T));

        MN_put_cm_lcs_molr_conf(TRUE, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 lcs_molr_res);


#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(lcs_molr_res, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_lcs_molr_conf(FALSE, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 lcs_molr_res);
      }     
#endif
      break;
    }
#endif

    default:
    {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid SS operation type"); 

      break;
    }
  }
}


/*
 * Function name :  MN_handle_error
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
 * Parameters: parameters
 * ----------  ss_info
 *
 *
 *
 * Returns:    none
 * --------
 *
 */
VOID_FUNC MN_handle_error(MN_FACILITY_DATA_T *facility_ptr, 
                          byte cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                         sys_modem_as_id_e_type    as_id,
#endif
                          byte ss_operation)
{
   ie_cm_ss_error_T cm_ss_error;
   boolean success;
   /* Zero the data */

   memset(&(cm_ss_error), NULL, sizeof(cm_ss_error));

   cm_ss_error.present = TRUE;
   success = TRUE;

   if(facility_ptr != NULL)
   {
       switch (facility_ptr->facility_component_data[0].component_type)
       {

          case RETURN_ERROR:
          {
             cm_ss_error.error_code_tag = MNSS_ERROR_TAG;
             cm_ss_error.error_code = facility_ptr->facility_component_data[0].error_code;
             MN_decode_error_parameters(&(cm_ss_error), facility_ptr);
             break;
          }

          case REJECT:
          {
             cm_ss_error.error_code_tag = facility_ptr->facility_component_data[0].problem_tag;
             cm_ss_error.error_code = facility_ptr->facility_component_data[0].problem_code;
             break;
          }

          default:
          {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Unknown facility componenet type");
              success = FALSE;
              break;
          }
       }
   }
   else
   {
       cm_ss_error.present = FALSE;
   }

   /* Send the appropriate message */
   if(success)
       MN_select_ss_error(cm_ss_ref, 
                          ss_operation, 
#if defined(FEATURE_DUAL_SIM )
                          as_id,
#endif
                          cm_ss_error);
}



/*
 * Function name :  MN_handle_invoke
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
 * Parameters: parameters
 * ----------  ss_info
 *
 *
 *
 * Returns:    none
 * --------
 *
 */
VOID_FUNC MN_handle_invoke(MN_FACILITY_DATA_T *facility_ptr
#if defined(FEATURE_DUAL_SIM )
       , sys_modem_as_id_e_type as_id
#endif

)
{
   byte                    guidance;
   boolean                 success;
   byte                   *components;
   ss_reject_T             ss_reject;
   connection_id_T         connection_id;
   uss_data_T              uss_data;
   byte                    invoke_id,*param_ptr,*pointer;
   cm_get_password_res_T  cm_get_password_res;
   unsigned int                     param_length,index;
   return_size_T           size;
   byte                    invoke_index = 0;
#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
   location_notification_T location_notification;
   ie_cm_ss_error_T        ss_error;
#endif
   byte                   *last_octet_data_pointer;
   byte                   *last_octet_component_pointer;

   /* Assign param_ptr to facility parameters */

   param_ptr = facility_ptr->facility_component_data[0].parameters;
   last_octet_data_pointer = param_ptr + MAX_SS_PARAMETER_LENGTH - 1;

   connection_id = facility_ptr->connection_id;


   switch (facility_ptr->facility_component_data[0].operational_code)
   {

   case getPassword:
      {
         /* Get the guidance info */

         if((facility_ptr->facility_component_data[0].parameter_length == 3) &&
             (facility_ptr->facility_component_data[0].parameters[1] == 1))
         {
            guidance = param_ptr[2];
#if defined(FEATURE_DUAL_SIM )
            if(MNCNM_IS_SINGLE_STACK)
            {
              cm_get_password_res.as_id = as_id;
            }
#endif
            switch (guidance)
            {
            case enterPW:  /* 0 */
               {
                  if (facility_ptr->facility_component_data[0].linked_id_present == TRUE)
                  {

                     /* This getPassword is in response to a previous MO SS operation

                                              */

                     if (ss_invoke_data[
                         facility_ptr->facility_component_data[0].linked_id].
                         ss_password_present == TRUE)
                     {
                        /* Send back a facility message */

                        cm_get_password_res.invoke_id =facility_ptr->facility_component_data[0].
                            invoke_id;

                        cm_get_password_res.ss_password = ss_invoke_data[facility_ptr->
                             facility_component_data[0].linked_id].ss_password;

                        MN_handle_cm_get_password_res(&cm_get_password_res);
                     }

                     else
                     {
                        /* Ask the user */

                            MN_put_cm_get_password_ind(
                            ss_invoke_data[facility_ptr->
                             facility_component_data[0].linked_id].cm_ss_ref,
#if defined(FEATURE_DUAL_SIM )
                            as_id,
#endif
                            guidance);
                     }
                  }

                  else
                  {
                     invoke_index = MN_find_mt_invoke_id( facility_ptr->
                              facility_component_data[0].invoke_id );
                     if(invoke_index >= MAXNO_INVOKE_IDS)
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
                     }
                     else
                     {
                        MN_put_cm_get_password_ind(ss_invoke_data[invoke_index].
                                                 mt_cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                 as_id,
#endif
                                                 guidance);
                     }
                  }
                  break;

               }

            case enterNewPW:
               {
                  if (facility_ptr->facility_component_data[0].linked_id_present == TRUE)
                  {
                     if (ss_invoke_data[facility_ptr->facility_component_data[0].
                   linked_id].ss_new_password_present
                         == TRUE)
                     {
                        /* Send back a facility message */

                        cm_get_password_res.invoke_id =facility_ptr->facility_component_data[0].
                            invoke_id;

                        cm_get_password_res.ss_password = ss_invoke_data[facility_ptr->
                             facility_component_data[0].linked_id].ss_new_password;

                        MN_handle_cm_get_password_res(&cm_get_password_res);
                     }

                     else
                     {
                        /* Ask the user */

                        MN_put_cm_get_password_ind(
                             ss_invoke_data[facility_ptr->
                             facility_component_data[0].linked_id].cm_ss_ref,
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                            guidance);
                     }
                  }

                  else
                  {
                     invoke_index = MN_find_mt_invoke_id( facility_ptr->
                              facility_component_data[0].invoke_id );
                     if(invoke_index >= MAXNO_INVOKE_IDS)
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
                     }
                     else
                     {
                        MN_put_cm_get_password_ind(ss_invoke_data[invoke_index].
                            mt_cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                as_id,

#endif
                            guidance);
                     }
                  }
                  break;
               }

            case enterNewPW_Again:
               {
                  if (facility_ptr->facility_component_data[0].linked_id_present == TRUE)
                  {
                     if (ss_invoke_data[facility_ptr->facility_component_data[0].
                   linked_id].ss_new_password_present
                         == TRUE)
                     {
                        /* Send back a facility message */

                        cm_get_password_res.invoke_id =facility_ptr->facility_component_data[0].
                            invoke_id;

                        cm_get_password_res.ss_password = ss_invoke_data[facility_ptr->
                             facility_component_data[0].linked_id].ss_new_password_again;

                        MN_handle_cm_get_password_res(&cm_get_password_res);

                     }

                     else
                     {
                        /* Ask the user */

                        MN_put_cm_get_password_ind(
                            ss_invoke_data[facility_ptr->
                             facility_component_data[0].linked_id].cm_ss_ref,
#if defined(FEATURE_DUAL_SIM )
                             as_id,
#endif
                            guidance);
                     }
                  }

                  else
                  {
                     invoke_index = MN_find_mt_invoke_id( facility_ptr->
                              facility_component_data[0].invoke_id );
                     if(invoke_index >= MAXNO_INVOKE_IDS)
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
                     }
                     else
                     {
                       MN_put_cm_get_password_ind(ss_invoke_data[invoke_index].
                           mt_cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM ) 
                           as_id,
#endif
                           guidance);
                     }
                  }
                  break;

               }

            case badPW_TryAgain:
            case badPW_FormatTryAgain:
               {
                  if(facility_ptr->facility_component_data[0].linked_id_present == TRUE)
                  {
                     /* Ask the user */

                     MN_put_cm_get_password_ind(
                      ss_invoke_data[facility_ptr->
                       facility_component_data[0].linked_id].cm_ss_ref,
#if defined(FEATURE_DUAL_SIM )
                       as_id,
#endif
                      guidance);

                  }

                  else
                  {
                     invoke_index = MN_find_mt_invoke_id( facility_ptr->
                              facility_component_data[0].invoke_id );
                     if(invoke_index >= MAXNO_INVOKE_IDS)
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
                     }
                     else
                     {
                       MN_put_cm_get_password_ind(
                         ss_invoke_data[invoke_index].mt_cm_ss_ref,
#if defined(FEATURE_DUAL_SIM )
                             as_id,
#endif
                         guidance);
                     }
                  }
                  break;

               }

            default:
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected guidance value in MN SS get PW operation"); 

                  break;
               }

            }
         }

         break;

      }

   case forwardCheckSS_Indication:

      {
#if defined(FEATURE_DUAL_SIM )
         MN_put_cm_forward_check_ss_ind(as_id);

#else
         MN_put_cm_forward_check_ss_ind();  
#endif


         if( MN_release_ci_invoke_id(facility_ptr->
             facility_component_data[0].invoke_id,
             forwardCheckSS_Indication,
             &connection_id,
             TRUE)
             == TRUE)
         {
            MN_put_MNSS_END_REQ(connection_id, NULL);
         }


         break;
      }

   case unstructuredSS_Request:
   case unstructuredSS_Notify:
      {
         success = FALSE;
         index = 0;

         if(param_ptr[index] == SEQUENCE_TAG)
         {
            index++;

/* Get the sequence size */

            pointer = &(param_ptr[index]);
            size = MN_get_element_length(pointer,SS_MAX_USS_REQ_LENGTH,(last_octet_data_pointer - pointer + 1));

       /* Adjust the pointers */ 

            pointer += size.increment;
            index += size.increment;

            /* Zero unused structure members */

            memset(&(uss_data), NULL, sizeof(uss_data_T));

            /* Decode the ussd-DataCodingScheme */

            if((&(param_ptr[index +1]) <= last_octet_data_pointer) && 
                (param_ptr[index] == OCTET_STRING) &&
                (param_ptr[index + 1] == 0x01))
            {
               index += 2;
               uss_data.uss_data_coding_scheme = param_ptr[index];
               index++;
               success = TRUE;
            }

            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected IE in parameter list"); 

               success = FALSE;
            }

            /* Decode the ussData */

            if((&(param_ptr[index]) <= last_octet_data_pointer) && 
                param_ptr[index] == OCTET_STRING)
            {
               index++;

               pointer = &(param_ptr[index]);

                 /* get the string size */

               size = MN_get_element_length(pointer,SS_MAX_USS_DATA_LENGTH,(last_octet_data_pointer - pointer + 1));
               param_length = size.length;

                 /* adjust the pointers */

               pointer += size.increment;
               index += size.increment;

               if (((unsigned int)param_length <= sizeof(uss_data.ussData)) &&
                    &(param_ptr[index+param_length - 1]) <= last_octet_data_pointer)
               {
                  uss_data.size = (byte)param_length;

                  /* Copy ussd Data from parameters list */

                  memscpy(uss_data.ussData,
                      sizeof(uss_data.ussData),
                      &(param_ptr[index]),
                      (unsigned int)param_length);

                  success = TRUE;
               }
            }

            else
            {
                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: ussd data IEI incorrect");

               success = FALSE;
            }
         }

         if (success == TRUE)
         {
            if (facility_ptr->facility_component_data[0].operational_code ==
                unstructuredSS_Request)
            {
               MN_put_cm_unstructured_ss_ind(facility_ptr->
                   facility_component_data[0].invoke_id,
#if defined(FEATURE_DUAL_SIM )
                   as_id,
#endif
                   &uss_data);


               /* Possible network response to MO processUSSD request */
               for ( invoke_id=0; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
               {
                    if ( (ss_invoke_data[invoke_id].connection_id == connection_id) &&
                         (ss_invoke_data[invoke_id].ss_stored_operation == processUnstructuredSS_Request))
                    {
                       MN_stop_ss_timer(invoke_id);
                       break;
                    }
               }

            }

            if (facility_ptr->facility_component_data[0].operational_code ==
                unstructuredSS_Notify)
            {
               invoke_id = facility_ptr->facility_component_data[0].invoke_id;

               MN_put_cm_unstructured_ss_notify_ind(
                        invoke_id,
#if defined(FEATURE_DUAL_SIM )
                        as_id,
#endif
                        &uss_data);
            }
         }

         else
         {
            /* Unstructured ss invokes can be rejected by the MS */
#ifdef FEATURE_MODEM_HEAP
              byte             *reject_component = (byte *) modem_mem_calloc(1, MAX_COMPONENTS_LENGTH,MODEM_MEM_CLIENT_NAS);
              ie_facility_T     *facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T),MODEM_MEM_CLIENT_NAS);

#else
              byte     *reject_component = (byte *) gs_alloc(MAX_COMPONENTS_LENGTH);
              ie_facility_T  *facility =   (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));

#endif
              if((reject_component != NULL) && (facility != NULL))
              {
                 ss_reject.problem_code_tag = 0x80;   /* general problem tag */
                 ss_reject.problem_code = 0x02;       /* badly structured component */
     
        /* Set up buffer */
     
                 components = &(reject_component[0]);
                 last_octet_component_pointer = components + MAX_COMPONENTS_LENGTH -1; /* components points to buffer of size 220*/
     
                 MN_put_reject(ss_reject,
                     facility_ptr->facility_component_data[0].invoke_id,
                     components);    /* components returned from function */
     
                 MN_put_facility(components, facility, last_octet_component_pointer);
     
                 MN_put_MNSS_FACILITY_REQ(connection_id,
                                          facility);
#ifndef FEATURE_MODEM_HEAP
                 gs_free( reject_component );
                 gs_free(facility);
#else
                 modem_mem_free(reject_component, MODEM_MEM_CLIENT_NAS);
                 modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
           }
           else
           {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Mem Alloc failed for reject_component or facility");
           }
         }
         break;
      }

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
   case lcs_LocationNotification:
      {
         enum
         {
           SUCCESS,
           COMPONENT_FAIL,
           DATA_VALUE_FAIL
         } success_status;

         success_status = SUCCESS;
         index = 0;

         if(param_ptr[index] == SEQUENCE_TAG)
         {
            index++;

           /* Get the sequence size */

            pointer = &(param_ptr[index]);
            size = MN_get_element_length(pointer,SS_MAX_LOC_NOTIFICATION_REQ_LENGTH, (last_octet_data_pointer - pointer + 1));

            /* Adjust the pointers */ 

            pointer += size.increment;
            index += size.increment;

            /* Zero unused structure members */

            memset(&(location_notification), NULL, sizeof(location_notification_T));

            /* Decode the Notification Type */

            if((&(param_ptr[index+1]) <= last_octet_data_pointer) &&
                (param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 0 ) ) &&
                (param_ptr[index + 1] == 0x01))
            {
               index += 2;
               location_notification.notification_type = (notification_type_e_type)param_ptr[index];

               /* Check for bounds */
               if (location_notification.notification_type > (notification_type_e_type)2)
               {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid Location Notification Type"); 
                    success_status = DATA_VALUE_FAIL;
               }
               index++;
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected IE in parameter list"); 

               success_status = COMPONENT_FAIL;
            }

            /* Decode the Location Type */
            if( success_status == SUCCESS )
            {
              if((&(param_ptr[index]) <= last_octet_data_pointer) && 
                  ( param_ptr[index] == ( CONTEXT_SPECIFY_CONSTRUCT | 1 ) ))
              {
                 index++;

                 pointer = &(param_ptr[index]);
  
                 /* get the sequence size */
                 size = MN_get_element_length(pointer,SS_MAX_LOC_TYPE_LENGTH,(last_octet_data_pointer - pointer + 1));
                 param_length = size.length;
  
                 /* adjust the pointers */
                 pointer += size.increment;
                 index += size.increment;

                 if( (&(param_ptr[index]) <= last_octet_data_pointer) && 
                      ( (param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 0 ))  ||
                     (param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 1 )) ) )
                 {
                   /* Decode the Location Estimated Type */
                   if((&(param_ptr[index + 2]) <= last_octet_data_pointer) && /* Reading three octets*/
                       (param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 0 )) &&
                      (param_ptr[index + 1] == 0x01))
                   {
                      index += 2;
                      location_notification.location_type = (location_type_e_type)param_ptr[index];

                      /* Check for bounds */
                      if(location_notification.location_type > (location_type_e_type)4) 
                      {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid Location Type"); 
                        success_status = DATA_VALUE_FAIL; 
                      }

                      index++;
                   }

                   /* Decode the deferred Location Event Type */
                   if((&(param_ptr[index + 3]) <= last_octet_data_pointer) && /* Reading 4 octets*/
                       (param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 1 )) &&
                       (param_ptr[index + 1] == 2))
                   {
                      index += 2; /* tag +  length */

                      location_notification.deferred_location_type.present = TRUE ;

                      location_notification.deferred_location_type.unused_bits = param_ptr[index];
                      index++;

                      location_notification.deferred_location_type.ms_available = param_ptr[index];
                      index++;
                   } 
                 }
                 else
                 {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid Location Type"); 
                   success_status = DATA_VALUE_FAIL; 
                 }
              }
              else
              {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR:  lcs_locationNotification data IEI incorrect");

                 success_status = COMPONENT_FAIL;
              }

            } /* lcs location type */

            /* lcs Client External ID */
            if( ((&(param_ptr[index]) <= last_octet_data_pointer)) && (param_ptr[index] == ( CONTEXT_SPECIFY_CONSTRUCT | 2 )) && ( success_status == SUCCESS ) )
            {
              index++;

              pointer = &(param_ptr[index]);

              /* get the string size */

              size = MN_get_element_length(pointer,SS_MAX_LCS_CLIENT_EXT_ID_LENGTH,(last_octet_data_pointer - pointer + 1));
              param_length = size.length;

              /* adjust the pointers */

              pointer += size.increment;
              index += size.increment;
                 
              location_notification.external_id.present = TRUE;

              /* Decode External Address */
              if((&(param_ptr[index]) <= last_octet_data_pointer) && (param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 0 )))
              {
                byte cnt;
                byte *data_ptr;

                index++;

                pointer = &(param_ptr[index]);

                /* get the string size */
                size = MN_get_element_length(pointer,SS_MAX_LCS_EXT_ADDR_LENGTH,(last_octet_data_pointer - pointer + 1));
                param_length = size.length;

                /* adjust the pointers */
                pointer += size.increment;
                index += size.increment;

                if (((&(param_ptr[index + param_length - 1]) > last_octet_data_pointer)) || (param_length > sizeof(location_notification.external_id.external_address.data) ))
                {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR:  lcs_locationNotification data IEI incorrect");

                  success_status = DATA_VALUE_FAIL;
                }
                else
                {
                  location_notification.external_id.external_address.present = TRUE;
                  location_notification.external_id.external_address.size = (uint8)param_length;

                  data_ptr = location_notification.external_id.external_address.data;

                  /* Extract external address */
                  for ( cnt = (uint8)param_length; cnt; cnt-- )
                  {
                    *data_ptr++ = param_ptr[index++]; 
                  }
                }
              }

              /* Extension container not supported */
              if((&(param_ptr[index]) <= last_octet_data_pointer) && 
                  ( param_ptr[index] == ( CONTEXT_SPECIFY_CONSTRUCT | 1 )) && ( success_status == SUCCESS ) )
              {
                index++;

                pointer = &(param_ptr[index]);

                /* get the sequence size */
                size = MN_get_element_length(pointer,MAX_SS_PARAMETER_LENGTH,(last_octet_data_pointer - pointer + 1));
                param_length = size.length;

                /* adjust the pointers */
                pointer += size.increment;
                index += size.increment;

                /* Skip the extension container */
                index += param_length;
              }
            }

            /* lcs Client Name */
            if( (&(param_ptr[index]) <= last_octet_data_pointer) && (param_ptr[index] == ( CONTEXT_SPECIFY_CONSTRUCT | 3 )) && ( success_status == SUCCESS ) )
            {
              index++;

              pointer = &(param_ptr[index]);

              /* get the sequence size */
              size = MN_get_element_length(pointer,SS_MAX_LCS_CLIENT_NAME_LENGTH,(last_octet_data_pointer - pointer + 1));
              param_length = size.length;

              /* adjust the pointers */

              pointer += size.increment;
              index += size.increment;
            
              location_notification.client_name.present = TRUE;

              /* Decode Data Coding Scheme */
              if((&(param_ptr[index + 1]) <= last_octet_data_pointer) && 
                  (param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 0 )) &&
                  (param_ptr[index + 1] == 0x01))
              {
                 index += 2;
                 location_notification.client_name.data_coding_scheme = param_ptr[index];
                 index++;
              }
              else
              {
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected IE in parameter list"); 
                 success_status = COMPONENT_FAIL;
              }

              /* Decode Name String */
              if ( success_status == SUCCESS )
              {
                if ((&(param_ptr[index]) <= last_octet_data_pointer) &&( param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 2 ) ) )
                {
                  byte cnt;
                  byte *data_ptr;

                  index++;

                  pointer = &(param_ptr[index]);

                  /* get the string size */
                  size = MN_get_element_length(pointer,SS_MAX_LCS_NAMESTRING_LENGTH,(last_octet_data_pointer - pointer + 1));
                  param_length = size.length;

                  /* adjust the pointers */
                  pointer += size.increment;
                  index += size.increment;

                  if ((&(param_ptr[index + param_length - 1]) > last_octet_data_pointer) || ( param_length > SS_MAX_LCS_NAMESTRING_LENGTH ))
                  {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR:  lcs_locationNotification data IEI incorrect");
  
                    success_status = DATA_VALUE_FAIL;
                  }
                  else
                  {
                    location_notification.client_name.namestring.present = TRUE;
                    location_notification.client_name.namestring.size = (uint8)param_length;
  
                    data_ptr = location_notification.client_name.namestring.data;
  
                    for ( cnt = (uint8)param_length; cnt; cnt-- )
                    {
                      *data_ptr++ = param_ptr[ index++ ];
                    }
                  }
                }
                else
                {
                  success_status = COMPONENT_FAIL;
                }
              }

            }  /* lcs client_name */

            /* lcs requestor ID */
            if ( (&(param_ptr[index]) <= last_octet_data_pointer) && 
                  (param_ptr[index] == ( CONTEXT_SPECIFY_CONSTRUCT | 4 )) && 
                  ( success_status == SUCCESS ) )
            {
              index++;

              pointer = &(param_ptr[index]);

              /* get the sequence size */
              size = MN_get_element_length(pointer,SS_MAX_LCS_CLIENT_NAME_LENGTH,(last_octet_data_pointer - pointer + 1));
              param_length = size.length;

              /* adjust the pointers */

              pointer += size.increment;
              index += size.increment;
            
              location_notification.requestor_id.present = TRUE;

              /* Decode Data Coding Scheme */
              if((&(param_ptr[index + 1]) <= last_octet_data_pointer) && 
                  (param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 0 )) &&
                  (param_ptr[index + 1] == 0x01))
              {
                 index += 2;
                 location_notification.requestor_id.data_coding_scheme = param_ptr[index];
                 index++;
              }
              else
              {
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected IE in parameter list"); 
                 success_status = COMPONENT_FAIL;
              }

              /* Decode Name String */
              if ( success_status == SUCCESS )
              {
                if ( (&(param_ptr[index]) <= last_octet_data_pointer) && 
                       (param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 1 )) ) 
                {
                  byte cnt;
                  byte *data_ptr;

                  index++;

                  pointer = &(param_ptr[index]);

                  /* get the string size */
                  size = MN_get_element_length(pointer, SS_MAX_LCS_NAMESTRING_LENGTH,(last_octet_data_pointer - pointer + 1));
                  param_length = size.length;

                  /* adjust the pointers */
                  pointer += size.increment;
                  index += size.increment;

                  if ( (&(param_ptr[index + param_length - 1]) > last_octet_data_pointer) ||
                        (param_length > SS_MAX_LCS_NAMESTRING_LENGTH) )
                  {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR:  lcs_locationNotification data IEI incorrect");
  
                    success_status = DATA_VALUE_FAIL;
                  }
                  else
                  {
                    location_notification.requestor_id.requestorIDString.present = TRUE;
                    location_notification.requestor_id.requestorIDString.size = (uint8)param_length;
  
                    data_ptr = location_notification.requestor_id.requestorIDString.data;
  
                    for ( cnt = (uint8)param_length; cnt; cnt-- )
                    {
                      *data_ptr++ = param_ptr[ index++ ];
                    }
                  }
                }
                else
                {
                  success_status = COMPONENT_FAIL;
                }
              } /* name string */

            } /* lcs requestor ID */

            /* lcs codeword string */
            if ( (&(param_ptr[index]) <= last_octet_data_pointer) &&
                  (param_ptr[index] == ( CONTEXT_SPECIFY_CONSTRUCT | 5 )) && 
                  ( success_status == SUCCESS ) )
            {
              index++;

              pointer = &(param_ptr[index]);

              /* get the sequence size */
              size = MN_get_element_length(pointer,SS_MAX_LCS_CLIENT_NAME_LENGTH,(last_octet_data_pointer - pointer + 1));
              param_length = size.length;

              /* adjust the pointers */
              pointer += size.increment;
              index += size.increment;
            
              location_notification.codeword_string.present = TRUE;

              /* Decode Data Coding Scheme */
              if((&(param_ptr[index + 1]) <= last_octet_data_pointer) &&
                  (param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 0 )) &&
                  (param_ptr[index + 1] == 0x01))
              {
                 index += 2;
                 location_notification.codeword_string.data_coding_scheme = param_ptr[index];
                 index++;
              }
              else
              {
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected IE in parameter list"); 
                 success_status = COMPONENT_FAIL;
              }

              /* Decode Name String */
              if ( success_status == SUCCESS )
              {
                if ( (&(param_ptr[index]) <= last_octet_data_pointer) &&
                       param_ptr[index] == ( CONTEXT_SPECIFY_PRIMITIVE | 1 ) ) 
                {
                  byte cnt;
                  byte *data_ptr;

                  index++;

                  pointer = &(param_ptr[index]);

                  /* get the string size */
                  size = MN_get_element_length(pointer, SS_MAX_LCS_NAMESTRING_LENGTH,(last_octet_data_pointer - pointer + 1));
                  param_length = size.length;

                  /* adjust the pointers */
                  pointer += size.increment;
                  index += size.increment;

                  if ( (&(param_ptr[index + param_length -1]) > last_octet_data_pointer) || 
                        param_length > SS_MAX_LCS_NAMESTRING_LENGTH )
                  {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR:  lcs_locationNotification data IEI incorrect");
  
                    success_status = DATA_VALUE_FAIL;
                  }
                  else
                  {
                    location_notification.codeword_string.lcsCodewordString.present = TRUE;
                    location_notification.codeword_string.lcsCodewordString.size = (uint8)param_length;
  
                    data_ptr = location_notification.codeword_string.lcsCodewordString.data;
  
                    for ( cnt = (uint8)param_length; cnt; cnt-- )
                    {
                      *data_ptr++ = param_ptr[ index++ ];
                    }
                  }
                }
                else
                {
                  success_status = COMPONENT_FAIL;
                }

              } /* name string */

            } /* lcs codeword_string */

            /* lcs service type */
            if ( (&(param_ptr[index + 1]) <= last_octet_data_pointer) && /* reading two octets in this block */
                  (param_ptr[index] == ( CONTEXT_SPECIFY_CONSTRUCT | 6 )) && ( success_status == SUCCESS ) )
            {
              index++;  /* length */

              pointer = &(param_ptr[index]);

              if(param_ptr[index] == 0x01)
              {
                 index++;
                 location_notification.service_type.present = TRUE; 
                 location_notification.service_type.lcsServiceTypeID = param_ptr[index];

                 index++;
              }
              else
              {
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected IE in parameter list"); 
                 success_status = COMPONENT_FAIL;
              }

            }  /* lcs service type */
         }
         else
         {
           success_status = COMPONENT_FAIL;
         }

         switch ( success_status )
         {
           case SUCCESS:
           {
             MN_put_cm_lcs_location_notification_ind(facility_ptr->
                 facility_component_data[0].
                 invoke_id,
#if defined(FEATURE_DUAL_SIM )
                 as_id,
#endif
                 &location_notification);

             break;
           }

           case DATA_VALUE_FAIL:
           {
             
             if ( MN_release_ci_invoke_id(  facility_ptr->facility_component_data[0].invoke_id,
                                            lcs_LocationNotification,
                                            &connection_id,
                                            TRUE) == TRUE )
             {

#ifdef FEATURE_MODEM_HEAP
                byte       *error_component = (byte *) modem_mem_calloc(1, MAX_COMPONENTS_LENGTH,MODEM_MEM_CLIENT_NAS);
                ie_facility_T  *facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T),MODEM_MEM_CLIENT_NAS);
#else
                byte       *error_component = (byte *) gs_alloc(MAX_COMPONENTS_LENGTH);
                ie_facility_T  *facility =    (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));

#endif

                 if((error_component != NULL) && (facility != NULL))
                 {
                    /* Send back a Release Complete with Return Error */
                    ss_error.error_code_tag = 0x2;
                    ss_error.error_code = 36; /* Unexpected Data Value */ 
       
                    /* Set up buffer */
        
                    components = &(error_component[0]);
                    last_octet_component_pointer = components + MAX_COMPONENTS_LENGTH - 1;
        
                    MN_put_error(ss_error,
                        facility_ptr->facility_component_data[0].invoke_id,
                        NULL,
                        0,              /* include only the error code, SEQUENCE+NULL is not required */
                        components);    /* components returned from function */
       
                    MN_put_facility(components, facility, last_octet_component_pointer);
       
                    MN_put_MNSS_END_REQ(connection_id,
                                             facility);

#ifndef FEATURE_MODEM_HEAP
                    gs_free( error_component );
                    gs_free(facility);

#else
                    modem_mem_free(error_component, MODEM_MEM_CLIENT_NAS);
                    modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
               }
               else
               {
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Mem Alloc2 failed for error_component or facility");
               }
             }
             else
             {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Error Releasing Invoke ID %d:",
                 facility_ptr->facility_component_data[0].invoke_id ); 
             }
   
             break;
           }

           case COMPONENT_FAIL:
           {
             ss_reject.problem_code_tag = 0x80;   /* general problem tag */
             ss_reject.problem_code = 0x02;       /* badly structured component */

             if ( MN_release_ci_invoke_id(  facility_ptr->facility_component_data[0].invoke_id,
                                            lcs_LocationNotification,
                                            &connection_id,
                                            TRUE) == TRUE )
             {
               /* Set up buffer */

#ifdef FEATURE_MODEM_HEAP
               byte  *reject_component = (byte *) modem_mem_calloc(1, MAX_COMPONENTS_LENGTH,MODEM_MEM_CLIENT_NAS);
               ie_facility_T  *facility = (ie_facility_T *) modem_mem_calloc(1, sizeof(ie_facility_T),MODEM_MEM_CLIENT_NAS);
#else
               byte  *reject_component = (byte *) gs_alloc(MAX_COMPONENTS_LENGTH);
               ie_facility_T  *facility =  (ie_facility_T *) gs_alloc(sizeof(ie_facility_T));
#endif
               if((reject_component != NULL) && (facility != NULL))
               {
                  components = &(reject_component[0]);
                  last_octet_component_pointer = components + MAX_COMPONENTS_LENGTH - 1;
     
                  MN_put_reject(ss_reject,
                      facility_ptr->facility_component_data[0].invoke_id,
                      components);    /* components returned from function */
     
                  MN_put_facility(components,facility, last_octet_component_pointer);
     
                  MN_put_MNSS_END_REQ(connection_id,
                                       facility);
#ifndef FEATURE_MODEM_HEAP
                  gs_free( reject_component );
                  gs_free( facility );

#else
                  modem_mem_free(reject_component, MODEM_MEM_CLIENT_NAS);
                  modem_mem_free(facility, MODEM_MEM_CLIENT_NAS);
#endif
               }
               else
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Mem Alloc failed for reject_component or facility");
               }
             }
             else
             {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Error Releasing Invoke ID %d:",
                 facility_ptr->facility_component_data[0].invoke_id ); 
             }

             break;
           }

           default:
             break;
         }


         break;
      }
#endif

   default:
      {
          MNSS_REJ_IND_T     mnss_rej_ind;
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected ss operation in MT SS CI operation");
          MN_put_MNSS_END_REQ(connection_id, NULL);

          /** If this is response from NW for an MO USSD, upper layer need to be informed that session is closed 
           ** as UE is responding with RELEASE_COMPLETE in response to NW's faulty INVOKE.
           ** */
           for (invoke_id = 0; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
           {
                  if (ss_invoke_data[invoke_id].connection_id == connection_id)
                  {
                       mnss_rej_ind.connection_id = connection_id;
                       mnss_rej_ind.mm_cause = OTHER_CAUSE;   
                       mnss_rej_ind.lower_layer_cause = NULL;
#if defined(FEATURE_DUAL_SIM )
                       mnss_rej_ind.as_id = as_id;
#endif
                       MN_get_MNSS_REJ_IND(&mnss_rej_ind);                  
                  }
           }
      }
      break;
   }
}


/*
 * Function name :  MN_load_invoke_id
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
 * Parameters: connection_id
 * ----------  invoke_id
 *             ss_operation
 *
 *
 * Returns:    boolean success
 * --------
 *
 */
boolean
MN_load_invoke_id(connection_id_T connection_id,
byte            invoke_id,
byte            ss_operation,
byte            cm_ss_ref)
{
   boolean success = FALSE;
   boolean success_2 = TRUE;
   int          i;


   /* Find out if the invoke id is already in use */

   for (i=0 ; i< MAXNO_INVOKE_IDS; i++)

   {

      if ((ss_invoke_data[i].mt_invoke_id_valid == TRUE)&&
          (ss_invoke_data[i].mt_invoke_id == invoke_id))

      {

         success_2 = FALSE;

      }

   }

   /* If not used, store details and return successs = TRUE */

   if (success_2 == TRUE)
   {
      for (i=0; ((i < MAXNO_INVOKE_IDS)&&(success == FALSE));i++)

      {

         if (ss_invoke_data[i].mt_invoke_id_valid == FALSE)

         {
            success = TRUE;
            ss_invoke_data[i].mt_invoke_id_valid = TRUE;
            ss_invoke_data[i].mt_invoke_id = invoke_id;
            ss_invoke_data[i].mt_ss_operation = ss_operation;
            ss_invoke_data[i].mt_connection_id = connection_id;
            if( cm_ss_ref == 0xFF )
            {
               /*
               ** no previous cm_ss_ref,
               ** so use invoke_id from net as cm_ss_ref
               */
               
               ss_invoke_data[i].mt_cm_ss_ref = invoke_id;
            }
            else
            {
               ss_invoke_data[i].mt_cm_ss_ref = cm_ss_ref;               
            }
         }
      }
   }

   return (success);
}


/*
 * Function name :  MN_check_for_parallel_SS
 * -------------
 *
 * Description :  Looks for parallel call Independent
 * -----------     SS transactions
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  invoke_id
 *             ss_operation
 *
 *
 * Returns:    boolean success
 * --------
 *
 */
boolean
MN_check_for_parallel_SS(connection_id_T connection_id)
{
   boolean success = TRUE;
   int          i;
   int    ss_operation;

   /* Find out if the invoke id is already in use */

   for (i=0 ; i< MAXNO_INVOKE_IDS; i++)

   {

      if ((ss_invoke_data[i].mt_invoke_id_valid == TRUE)||
          (ss_invoke_data[i].ss_stored_operation != NULL_SS_OPERATION))

      { 
         if (ss_invoke_data[i].mt_invoke_id_valid == TRUE)
         {
            ss_operation = ss_invoke_data[i].mt_ss_operation;
         }
         else
         {
            ss_operation = ss_invoke_data[i].ss_stored_operation;
         }
         if (( ss_operation == registerSS )||
             ( ss_operation == eraseSS )||
             ( ss_operation == activateSS )||
             ( ss_operation == deactivateSS )||
             ( ss_operation == interrogateSS )||
             ( ss_operation == registerPassword )||
             ( ss_operation == getPassword )||
             ( ss_operation == forwardCheckSS_Indication )||
             ( ss_operation == processUnstructuredSS_Request )||
             ( ss_operation == unstructuredSS_Request )||
             ( ss_operation == unstructuredSS_Notify )||
             ( ss_operation == lcs_MOLR))
          {
             success = FALSE;
          }
       }
      else if ((ss_invoke_data[i].mt_invoke_id_valid == FALSE) &&
                   ((ss_invoke_data[i].mt_ss_operation == unstructuredSS_Request) ||
                   (ss_invoke_data[i].mt_ss_operation == unstructuredSS_Notify)) &&
                   (ss_invoke_data[i].mt_connection_id != connection_id))
      {
           success = FALSE;
      }
    }

   return (success);
}



/*
 * Function name:  MN_find_mt_invoke_id
 * -------------
 *
 * Description:    Finds given mt_invoke_id inn the ss_invoke_data[]
 * -----------     store.
 *
 * Parameters:     mt_invoke_id
 * ----------
 *
 * Returns:        byte index
 * --------
 *
 */

static byte MN_find_mt_invoke_id( byte mt_invoke_id )
{
   byte    index = 0;
   boolean success = FALSE;


   for( index = 0; index < MAXNO_INVOKE_IDS; index++ )
   {
      if( ( ss_invoke_data[index].mt_invoke_id_valid ) ||
          ( ss_invoke_data[index].mt_invoke_id == mt_invoke_id ) )
      {
         success = TRUE;
         break;
      }
   }

   if( !success )
   {
      index = mt_invoke_id;
   }

   return index;
}


/*
 *   Function name : MN_decode_error_parameters
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function decodes the SS error parameters 
 *     as in GSM 04.80
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


VOID_FUNC   MN_decode_error_parameters(ie_cm_ss_error_T    *cm_ss_error,
                                       MN_FACILITY_DATA_T   *facility_ptr)
{

   /* Locals */

   byte  parameter_length;
   byte  index;
   byte  *parameter, *last_octet_parameter_length;

   index = 0;

   parameter_length = facility_ptr->facility_component_data[0].parameter_length;

   parameter = facility_ptr->facility_component_data[0].parameters;
   last_octet_parameter_length = parameter + parameter_length -1;

   /*
   * Validate the parameter_length variable.
   * As long as parameter_length is byte, it can't exceed 255 and we have that
   * much space in parameters array.
   */
    
   if (*parameter == NULL)
   {  /* Possible indefinite_end */
      index++;
      parameter++;

      if (*parameter != NULL)

      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect length for null element");

       }

       else

       {   /* it is the end of the paramters */
           parameter_length = index;

        }

     }

   while(index < parameter_length)
   {

      switch(facility_ptr->facility_component_data[0].error_code)
      {

         /************************************************************
          *
          *                CALL BARRING ERROR        
          *
          **************************************************************
          */


         case 13:   /* Call Barring Error */
         {
            cm_ss_error->error_type = CALL_BARRED_ERR;
            if ( ((parameter + 2) < last_octet_parameter_length) && (*parameter == ENUMERATED) ) /* reading three octets inside this block */
            {

               parameter++;
               index++;

               if (*parameter == 1 )
               {

                  parameter++;
                  index++;

                  cm_ss_error->parameter.cm_called_barred_error.present = TRUE;

                  cm_ss_error->parameter.cm_called_barred_error.
                      call_barring_cause = *parameter;

                  parameter++;
                  index++;

               }
               else
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_called_barred_error element length incorrect");

                  index = parameter_length;

               }
            }
            else
            {

                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: tag = %d Incorrect Tag for cm_called_barred_error or Buffer Overflow",*parameter);


               index = parameter_length;

            }
            break;

         }

         /************************************************************
          *
          *                SS ERROR STATUS ERROR        
          *
          **************************************************************
          */


         case 17:   /* Error Status Error */
         {
            cm_ss_error->error_type = SS_ERROR_STATUS;  /* adding New error type for SS-ErrorStatus */
            if (((parameter + 2) <= last_octet_parameter_length) && (*parameter == OCTET_STRING)) /* reading three octets from parameter buf inside this block*/
            {
               parameter++;
               index++;

               if (*parameter == 1 )
               {

                  parameter++;
                  index++;


                  cm_ss_error->parameter.cm_ss_errors_status.present = TRUE;
                  
				  /* ss-Status */
                  MN_get_ss_status(&(cm_ss_error->parameter. 
                              cm_ss_errors_status.ss_status),
                              parameter);

                  parameter++;
                  index++;

               }
               else
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: error status element length incorrect");

                  index = parameter_length;

               }

            }
            else
            {

               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: tag = %d, Incorrect Tag or Buffer Overflow",*parameter);

               index = parameter_length;

            }
            break;
         }

         case 19:   /* SS subscription violation */
         {
#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
           uint32        ss_viol_param_len;
           uint32        ext_cont_len;
#endif /*  end of (FEATURE_CM_MMGPS || FEATURE_CM_LCS) */

            /************************************************************
             *
             *                SUBSCRIPTION VIOLATION ERROR        
             *
             **************************************************************
             */

            cm_ss_error->error_type = SUBSCRIPTION_VIOLATION_ERR;
            cm_ss_error->parameter.cm_subscription_violation.present = TRUE;

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
            if ((parameter < last_octet_parameter_length) && ( *parameter == SEQUENCE_TAG )) /*Need atleast two octets to read in the block*/
            {
               return_size_T size;

               parameter++;
               index++;

               /* Get Subscription Violation Sequence Length */
               size = MN_get_element_length(parameter,SS_MAX_SUBS_VIOL_LEN, (last_octet_parameter_length - parameter + 1));
               ss_viol_param_len = (uint32)size.length;

               /* Adjust the pointers */ 
               parameter += size.increment;
               index += (byte)size.increment;

               if ((parameter < last_octet_parameter_length) && ( *parameter == SEQUENCE_TAG ) && ( ss_viol_param_len >= 2 ) ) /*Need atleast two octets to read in the block*/
               {
                 /* Extension Container Sequence Included */
                 parameter++;
                 index++;

                 /* Get Extension Container Sequence Length */
                 size = MN_get_element_length(parameter,SS_MAX_SUBS_VIOL_EXT_CONT_LEN, (last_octet_parameter_length - parameter + 1));
                 ext_cont_len = (uint32)size.length;

                 /* Adjust the pointers */ 
                 parameter += size.increment;
                 index += (byte)size.increment;

                 if ( ((parameter + ext_cont_len) <= last_octet_parameter_length) && ( ext_cont_len <= CM_SUBS_VIOL_EXT_CONTAINER_LENGTH ))
                 {
                   uint32        cnt;
                   uint8         *p_ExtContData = 
                                   cm_ss_error->parameter.cm_subscription_violation.extension_container.data;

                   cm_ss_error->parameter.cm_subscription_violation.extension_container.present = TRUE;
                   cm_ss_error->parameter.cm_subscription_violation.extension_container.size = (byte)ext_cont_len;

                   /* Copy extension container payload */
                   for ( cnt = ext_cont_len; cnt; cnt-- )
                     *p_ExtContData++ = *parameter++;

                 }
                 else
                 {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: SS Subs Violation Extension Container Data exceeds storage, discarding");
                   index += (byte)ext_cont_len;
                   parameter += (byte)ext_cont_len;
                 }
               }
               else
               {
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: SS Subs Violation Sequence Length Incorrect");
                 index += (byte)ss_viol_param_len;
                 parameter += (byte)ss_viol_param_len;
               }
            }
            else 
#endif
            if ( (parameter <= last_octet_parameter_length) && (*parameter == 0x81))   /* override catagory */
            {

               parameter++;
               index++;

               if ( ( (parameter + sizeof(ie_override_category_T)) <= last_octet_parameter_length ) && (*parameter == 1 ) )
               {

                  parameter++;
                  index++;


                  cm_ss_error->parameter.cm_subscription_violation.
                      ss_subscription_option.option_type = CLI_OVERRIDE_OPTION;

                  memscpy(&(cm_ss_error->parameter.cm_subscription_violation.
                           ss_subscription_option.option), 
                           sizeof(cm_ss_error->parameter.cm_subscription_violation.
                           ss_subscription_option.option),
                         parameter, 
                         sizeof(ie_override_category_T));

                  parameter++;
                  index++;

               }
               else
               {

                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: ss_subscription_option element length incorrect");

                  index = parameter_length;
               }
            }
            else if ( (parameter <= last_octet_parameter_length) && (*parameter == 0x82))   /* cli restriction option */ 
            {

               parameter++;
               index++;

               if ( ( (parameter + sizeof(cm_cli_restriction_option_T)) <= last_octet_parameter_length ) && (*parameter == 1 ) )
               {

                  parameter++;
                  index++;


                  cm_ss_error->parameter.cm_subscription_violation.
                      ss_subscription_option.option_type = CLI_RESTRICT_OPTION;


                  memscpy(&(cm_ss_error->parameter.cm_subscription_violation.
                           ss_subscription_option.option), 
                           sizeof(cm_ss_error->parameter.cm_subscription_violation.
                           ss_subscription_option.option),
                         parameter, 
                         sizeof(cm_cli_restriction_option_T));

                  parameter++;
                  index++;

               }
               else
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: clir_restriction_option element length incorrect"); 

                  index = parameter_length;

               }

            }
            else
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: tag = %d, cm_subscription_violation contains invalid component or Buffer Overflow",*parameter);

               index = parameter_length;

            }
            break;
         }

         /************************************************************
          *
          *                SS INCOMPATABILITY ERROR        
          *
          **************************************************************
          */

         case 20:   /* SS IncompatabilityError */
         {

            cm_ss_error->error_type = SS_INCOMPATIBILITY_ERR;
            cm_ss_error->parameter.cm_ss_incompatability_error.present = TRUE;

            if (( (parameter + 1) <= last_octet_parameter_length) && (*parameter == SEQUENCE_TAG)) /* we are reading 2 octets before next check */
            {

               byte sequence_end;

               parameter++;
               index++;
               /* fixed compile warning C4244 */
               sequence_end = (byte)(index + *parameter);
               /* We have already checked that sequence_end falls within boundary */
  
               parameter++;
               index++;

               if(&(parameter[sequence_end - 1]) > last_octet_parameter_length)
               {
                 parameter += (sequence_end - index);
                 index = sequence_end;
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Buffer Overflow");
               }

               while (index < sequence_end )
               {

                  switch (*parameter)
                  {

                     case 0x81:   /* ss_code */
                     {

                        parameter++;
                        index++;

                        if ( (parameter < last_octet_parameter_length) && (*parameter == 1 )) /* Reading two octets */
                        {

                           parameter++;
                           index++;

                           cm_ss_error->parameter.cm_ss_incompatability_error.
                               ss_code.present = TRUE;

                           cm_ss_error->parameter.cm_ss_incompatability_error.
                               ss_code.ss_code = *parameter;

                           parameter++;
                           index++;

                        }
                        else
                        {
                           MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: tag = %d, ss_code element length incorrect or Buffer Overflow",*parameter);

                           index = parameter_length; /* for coming out of the outer loop */
                           sequence_end = parameter_length; /*for coming out of the inner loop*/
                        }
                        break;
                     }

                     case 0x82:   /* bearer service */
                     {

                        parameter++;
                        index++;

                        if ( (parameter < last_octet_parameter_length) && (*parameter == 1 )) /* Reading two octets */
                        {

                           parameter++;
                           index++;


                           cm_ss_error->parameter.cm_ss_incompatability_error.
                               basic_service.present = TRUE;

                           cm_ss_error->parameter.cm_ss_incompatability_error.
                               basic_service.code_type = BS_CODE;

                           memscpy(&(cm_ss_error->parameter.cm_ss_incompatability_error.
                                    basic_service.bs_code), 
                                    sizeof(cm_ss_error->parameter.cm_ss_incompatability_error.
                                    basic_service.bs_code),
                                  parameter, sizeof(byte));

                           parameter++;
                           index++;

                        }
                        else
                        {

                            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: bs_code element length incorrect");

                           index = parameter_length; /* for coming out of the outer loop */
                           sequence_end = parameter_length; /*for coming out of the inner loop*/
                        }
                        break;
                     }

                     case 0x83:   /* tele service */
                     {

                        parameter++;
                        index++;

                        if ( (parameter < last_octet_parameter_length) && (*parameter == 1 )) /* Reading two octets */
                        {

                           parameter++;
                           index++;


                           cm_ss_error->parameter.cm_ss_incompatability_error.
                               basic_service.present = TRUE;

                           cm_ss_error->parameter.cm_ss_incompatability_error.
                               basic_service.code_type = TS_CODE;

                           memscpy(&(cm_ss_error->parameter.cm_ss_incompatability_error.
                                    basic_service.bs_code), 
                                    sizeof(cm_ss_error->parameter.cm_ss_incompatability_error.
                                    basic_service.bs_code),
                                  parameter, sizeof(byte));

                           parameter++;
                           index++;

                        }
                        else
                        {
                           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: ts_code element length incorrect");

                           index = parameter_length; /* for coming out of the outer loop */
                           sequence_end = parameter_length; /*for coming out of the inner loop*/
                        }

                        break;
                     }


                     case 0x84:   /* ss_status */
                     {

                        parameter++;
                        index++;

                        if ( (parameter < last_octet_parameter_length) && (*parameter == 1 )) /* Reading two octets */
                        {

                           parameter++;
                           index++;


                           cm_ss_error->parameter.cm_ss_incompatability_error.
                              ss_status.present = TRUE;

                           MN_get_ss_status(&(cm_ss_error->parameter.
                              cm_ss_incompatability_error.ss_status),
                              parameter);

                           parameter++;
                           index++;

                        }
                        else
                        {
                           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: ss_status element length incorrect"); 

                           index = parameter_length; /* for coming out of the outer loop */
                           sequence_end = parameter_length; /*for coming out of the inner loop*/
                        }
                        break;
                     }

                     default:
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: unknown element in cm_ss_incompatability_error");

                        index = parameter_length; /* for coming out of the outer loop */
                        sequence_end = parameter_length; /*for coming out of the inner loop*/

                        break;

                     }  /* end of case */

                  }  /* End of switch */

               }  /* end of while */

            }  /* end of IF */
            else
            {
 
                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Tag = %d, incorrect tag in cm_ss_incompatability_error or Buffer Overflow",*parameter);

               index = parameter_length;

            }

            break;

         }  /* End of case */

         /************************************************************
          *
          *               ABSENT SUBSCRIBER        
          *
          **************************************************************
          */

         case 27:   /* Absent Subscriber */
         {

            cm_ss_error->error_type = ABSENT_SUBSCRIBER_ERR;
            cm_ss_error->parameter.cm_absent_subscriber_error.present = TRUE;

            if ( ( (parameter + 2) <= last_octet_parameter_length) && (*parameter == UNIVERSAL_BOOLEAN)) /* Reading three octets in the block */
            {

               parameter++;
               index++;

               if (*parameter == 1 )
               {

                  parameter++;
                  index++;

                  if ( *parameter == 0 )
                  {

                     cm_ss_error->parameter.cm_absent_subscriber_error.
                        mwd_set = FALSE;

                  }
                  else
                  {

                     cm_ss_error->parameter.cm_absent_subscriber_error.
                        mwd_set = TRUE;

                  }

                  parameter++;
                  index++;


               }
               else
               {

                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_absent_subscriber_error element length incorrect");

               }

            }
            else
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: tag = %d, Incorrect Tag for cm_absent_subscriber_error or Buffer Overflow",*parameter);


               index = parameter_length;
            }
            break;
         }

         /************************************************************
          *
          *               SYSTEM FAILURE        
          *
          **************************************************************
          */

         case 34:
         {

            cm_ss_error->error_type = SYSTEM_FAILURE_ERR;
            cm_ss_error->parameter.cm_system_failure_error.present = TRUE;

            if (( (parameter + 2) <= last_octet_parameter_length) && (*parameter == ENUMERATED)) /* Reading three octets in the block */
            {

               parameter++;
               index++;

               if (*parameter == 1 )
               {

                  parameter++;
                  index++;

                  cm_ss_error->parameter.cm_system_failure_error.
                     network_resource = *parameter;

                  parameter++;
                  index++;

               }
               else
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_system_failure_error element length incorrect"); 

               }

            }
            else
            {

               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: tag = %d, Incorrect Tag for cm_system_failure_error or Buffer Overflow",*parameter);

               index = parameter_length;

            }
            break;
         }

         /************************************************************
          *
          *               PASSWORD REGISTRATION FAILURE        
          *
          **************************************************************
          */

         case 37:
         {

            cm_ss_error->error_type = PW_REG_FAILURE_ERR;
            cm_ss_error->parameter.cm_pw_registration_failure_error.present = TRUE;

            if (( (parameter + 2) <= last_octet_parameter_length) && (*parameter == ENUMERATED)) /* Reading three octets in the block */
            {

               parameter++;
               index++;

               if (*parameter == 1 )
               {

                  parameter++;
                  index++;

                  cm_ss_error->parameter.cm_pw_registration_failure_error.
                     pw_registration_failure_cause = *parameter;

                  parameter++;
                  index++;

               }
               else
               {

                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_pw_registration_failure_error length");

               }

            }
            else
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: tag = %d, Incorrect Tag for cm_pw_registration_failure_error or Buffer Overflow",*parameter);


               index = parameter_length;

            }
            break;
         }

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
         /************************************************************
          *
          *               POSITION METHOD FAILURE        
          *
          **************************************************************
          */

         case 54:
         {
            cm_ss_error->error_type = POSITION_METHOD_FAILURE_ERR;
            if ( ( (parameter + 4) <= last_octet_parameter_length) && (*parameter == SEQUENCE_TAG)) /* reading five octets in the block */
            {
               parameter++;
               index++;

               if (*parameter == 3 )
               {
                  parameter++;
                  index++;

                  if ( *parameter == ( CONTEXT_SPECIFY_PRIMITIVE | 0 ) )
                  {
                    parameter++;
                    index++;

                    if ( *parameter == 1 )
                    {
                      parameter++;
                      index++;
                    
                      cm_ss_error->parameter.cm_position_method_failure_error.
                      position_method_failure_diagnostic = *parameter;

                      cm_ss_error->parameter.cm_position_method_failure_error.present = TRUE;
                    }
                    else
                    {
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_position_method_failure_error length");
                    }
                  } /* context | 0 */

                  parameter++;
                  index++;
               }
               else
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: cm_position_method_failure_error length");
               }
            }
            else
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: tag = %d, Incorrect Tag for cm_position_method_failure_error or Buffer Overflow",*parameter);

               index = parameter_length;
            }
            break;
         }
#endif

         default:
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to identify SS Error");
            cm_ss_error->error_type = UNKNOWN_ERR;

            index = parameter_length;

         }

      }  /* End of switch statement */

   }   /* End of while statement */

}



/*
 *   Function name : MN_encode_error_parameters
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function encodes the SS error parameters 
 *     as in GSM 04.80
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN cm_ss_error
 *
 *  Returns: size, parameter(a byte string)
 *
 *
 */


int   MN_encode_error_parameters(ie_cm_ss_error_T    *cm_ss_error,
                                 byte                 *parameter)
{

   /* Locals */

   int  size = 0;

   /* Only 2 SS errors have parameters for MT SS transactions */

   switch(cm_ss_error->error_code)

   {

      /*
       ****************************************************
       *
       *                ABSENT SUBSCRIBER
       *
       *****************************************************
       */

      case 27:
      {

         if (cm_ss_error->parameter.cm_absent_subscriber_error.present == TRUE)
         {

            *parameter = UNIVERSAL_BOOLEAN;

            parameter++;
            size++;

            *parameter = 0x01;

            parameter++;
            size++;

            if (cm_ss_error->parameter.cm_absent_subscriber_error.
                mwd_set == TRUE)
            {

               *parameter = 0xff;

            }
            else
            {

               *parameter = 0x00;

            }

            parameter++;
            size++;

         }
         else 
         {
             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: absent subscriber SS Error"); 

         }
         /* end of case */

         break;
      }

      /*
       ****************************************************
       *
       *                SYSTEM FAILURE
       *
       *****************************************************
       */

      case 34:
      {

         if (cm_ss_error->parameter.cm_system_failure_error.present == TRUE)
         {

            *parameter = ENUMERATED;

            parameter++;
            size++;

            *parameter = 0x01;

            parameter++;
            size++;

            *parameter = cm_ss_error->parameter.
                cm_system_failure_error.network_resource;

            parameter++;
            size++;

         }
         else 
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: SS Error has incorrect parameter for system failure");

         }
         /* end of case */

         break;
      }

      default:
      {

         break;

      } /* end of case */

   }  /* end of switch */

   return(size);

}


/*
 * Function name :  MN_find_associated_invoke_id
 * -------------
 *
 * Description :  CM has responded to a SS Invoke
 * -----------    with a linked invoke ID referring to the original
 *                MO SS operation
 *
 *
 *
 *
 *
 * Parameters: cm_ss_ref
 * ----------  ss_operation
 *
 *
 *
 * Returns:    success,
 * --------    invoke_id
 *
 */

boolean MN_find_associated_invoke_id(byte  ss_operation,
                                     byte  cm_ss_ref,
                                     byte  *invoke_id )
{
   boolean success;
   int      i;

   /* These transactions already have a transaction ID from the network */

   /* Find if an invoke id exists for the same ss operation, and cm_ss_ref */


   for(success = FALSE,i = 0;
       ((i<MAXNO_INVOKE_IDS)&&(success==FALSE));
       i++)
   {
        if ((ss_invoke_data[i].mt_invoke_id_valid == TRUE) &&
           (ss_invoke_data[i].mt_cm_ss_ref == cm_ss_ref) &&
           (ss_invoke_data[i].mt_ss_operation == ss_operation))
        {
               success = TRUE;
               *invoke_id = ss_invoke_data[i].mt_invoke_id;             
        }

        else
        {
           if ((ss_invoke_data[i].connection_id != UNKNOWN_CONNECTION_ID) &&
               (ss_invoke_data[i].cm_ss_ref == cm_ss_ref) &&
               (ss_invoke_data[i].ss_stored_operation == ss_operation))
           {
                 success = TRUE;
                 *invoke_id =(byte) i; 
           }
        }
   }

   return(success);
}




/*
 * Function name :  MN_put_facility
 * -------------
 *
 * Description :  Put the facility data in to IE structure
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: *component - points at input component data
 * ----------  
 *
 *
 *
 * Returns:    none
 * --------
 *
 */
VOID_FUNC
MN_put_facility(byte *component, ie_facility_T *facility, byte *last_octet_component_pointer)
{
   /* Locals */
   byte *stream_ptr,facility_len;
   return_size_T   size;

   facility->Iei = IEI_FACILITY;

   stream_ptr = &(component[1]);

   size =  MN_get_element_length( stream_ptr,SS_MAX_FACILITY_LENGTH,(last_octet_component_pointer - stream_ptr + 1) );
   facility_len = (byte)size.length;
   

   /* adjust for 1 or 2 octet length elements and the component tag*/

   /* fixed compile warning C4244 */
   /* facility_len  = (byte)size.length;
      facility_len += (size.increment + 1);
   */
   facility_len =  (byte)(size.length + size.increment +1);

   if (facility_len > MAX_COMPONENTS_LENGTH)

   {
      facility_len = MAX_COMPONENTS_LENGTH;

      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: SS Facility component too long");

   }

   facility->Length = facility_len;

   memscpy(facility->data,
       sizeof(facility->data),
       component,
       facility_len);
}



/*
 * Function name :  MN_put_invoke
 * -------------
 *
 * Description :  Create an SS invoke component with argument of length size
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: argument
 * ----------  size
 *             ss operation
 *             invoke id
 *
 *
 * Returns:    component
 * --------
 *
 */
VOID_FUNC
MN_put_invoke(byte *argument,
int size,
byte ss_operation,
byte invoke_id,
byte *component)
{

   /*Locals*/

   int   index;

   index = 0;

   component[index++] = INVOKE; /* component type */


   MN_put_element_length(component,
       &index,
       size + 6);

   component[index++] = INVOKE_ID_TAG;      /* invoke id tag */
   component[index++] = 0x01;      /* invoke id length */
   component[index++] = invoke_id;
   component[index++] = OP_CODE_TAG;      /* operational code */
   component[index++] = 0x01;      /* operational code length */
   component[index++] = ss_operation;

   memscpy(&(component[index]), sizeof(component)-index ,argument,(unsigned int)size);  /* copy the invoke components parameters */
}


/*
 * Function name :  MN_put_ss_ForBS
 * -------------
 *
 * Description :  Create an SSforBS component
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: argument
 * ----------  ss_ForBS
 *             
 *            
 *
 *
 * Returns:    component
 * --------
 *
 */
int
MN_put_ss_ForBS(ss_ForBS_T ss_for_bs, byte *argument)
{
   int size;
   int index;
   byte *size_ptr;

   /* Create an SS message argument from the input data */


   argument[0] = 0x30;    /* sequence id */

   size_ptr = &(argument[1]); /* fill in size later */

   argument[2] = 0x04;    /* universal octet string */
   argument[3] = 0x01;    /* size (1) */

   argument[4] = ss_for_bs.ss_code;

   index = 5;

   if (ss_for_bs.basic_service.present == TRUE)
   {
       if (ss_for_bs.basic_service.code_type == BS_CODE)
       {
          argument[index++] = 0x82;    /* context[2] */
          argument[index++] = 0x01;    /* size of bs code */
          argument[index++] = ss_for_bs.basic_service.bs_code;
       }

       else if (ss_for_bs.basic_service.code_type == TS_CODE)
       {
          argument[index++] = 0x83;    /* context[3] */
          argument[index++] = 0x01;    /* size of bs code */
          argument[index++] = ss_for_bs.basic_service.bs_code;
       }
   } 
   size = (index);

   *size_ptr = (byte)(size - 2);

   return (size);


}


/*
 * Function name :  MN_put_error
 * -------------
 *
 * Description :  Create an SS invoke component with argument of length size
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: ss_operation
 * ----------  cm_ss_error
 *             invoke_id
 *             *argument
 *             size ( of argument string)
 *
 * Returns:    *components
 * --------
 *
 */
VOID_FUNC
MN_put_error(ie_cm_ss_error_T cm_ss_error,
byte invoke_id,
byte *argument,
int size,
byte *components)
{
   int index = 0;
   int i;

   /* Create an SS return error component with the argument of length 'size' */

   components[index++] = RETURN_ERROR;

   /* Generate a size value */

   MN_put_element_length(components,
       &index,
       size + 6);



   components[index++] = INVOKE_ID_TAG;
   components[index++] = 0x01;       /* invoke id length */
   components[index++] = invoke_id;
   components[index++] = cm_ss_error.error_code_tag;
   components[index++] = 0x01;       /* error code length */
   components[index++] = cm_ss_error.error_code;

   for (i = 0; i < size; i++)
   {
      components[index++] = argument[i];
   }

}



/*
 * Function name :  MN_put_result
 * -------------
 *
 * Description :  Create an SS Return Result component with argument of length size
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: ss_operation
 * ----------  size (of argument)
 *             invoke_id
 *             argument
 *
 *
 * Returns:    components
 * --------
 *
 */
VOID_FUNC
MN_put_result(byte invoke_id,
int size,
byte *argument,
byte ss_operation,
byte *components)
{
   int index = 0;

   /* Load the data */

   components[index++] = RETURN_RESULT;

   /* Calculate the component length allowing for values > 127 */

   if ( size > 124 ) /* 127 - invoke ID - Sequence Tag - op Code */

   {

      MN_put_element_length(components,
          &index,
          (size + 9));
   }

   else
   {
      if ( size == 0 )  /* No parameters, so no Sequence or Op Code */
      {
         MN_put_element_length(components,
                 &index,
                  3 );
      }
      else
      {

         MN_put_element_length(components,
                &index,
                (size + 8));
      }
   }
   /* Insert the Invoke ID element */

   components[index++] = INVOKE_ID_TAG;
   components[index++] = 0x01;
   components[index++] = invoke_id;

   /* No multiple components for call ind SS */


   if (size != 0)
   {

      /* Insert the Sequence IE */

      components[index++] = SEQUENCE_TAG;


      MN_put_element_length(components,
          &index,
          size + 3);


      /* Insert the Operational Code IE */

      components[index++] = OP_CODE_TAG;
      components[index++] = 0x01;
      components[index++] = ss_operation;

      /* Insert the Parameters into the message */

      memscpy(&(components[index]), sizeof(components)-index ,argument,(unsigned int)size);

      index += size;


   }
}


/*
 * Function name :  MN_put_reject
 * -------------
 *
 * Description :  Create an SS Reject component with no parameters
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: ss_reject
 * ----------  invoke_id
 *             argument
 *             
 *
 *
 * Returns:    components
 * --------
 *
 */
VOID_FUNC
MN_put_reject(ss_reject_T ss_reject,
byte invoke_id,
byte *components)
{
   int index = 0;


   components[index++] = REJECT;
   components[index++] = 6;    /* component length */
   components[index++] = INVOKE_ID_TAG;
   components[index++] = 0x01;
   components[index++] = invoke_id;
   components[index++] = ss_reject.problem_code_tag;
   components[index++] = 0x01;
   components[index++] = ss_reject.problem_code;

}


/*
 * Function name :  MN_put_registerSS_ARG
 * -------------
 *
 * Description :  Create an SS message argument from the input data
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: argument
 * ----------  registerSS_ARG
 *             
 *             
 *
 *
 * Returns:    
 * --------
 *
 */
int
MN_put_registerSS_ARG(byte *argument,
registerSS_ARG_T registerSS_ARG)
{
   int index  = 0;
   byte *size_ptr;

   /* Create the SS message */


   argument[index++] = SEQUENCE_TAG;
   size_ptr = &(argument[index++]);
   argument[index++] = 0x04;   /* Universal octet string */
   argument[index++] = 0x01;
   argument[index++] = registerSS_ARG.ss_code;

   if (registerSS_ARG.basic_service.present == TRUE)
   {
       if (registerSS_ARG.basic_service.code_type == BS_CODE)
       {
          argument[index++] = 0x82;
          argument[index++] = 0x01; /* Length */
          argument[index++] = registerSS_ARG.basic_service.bs_code;
       }

       else if(registerSS_ARG.basic_service.code_type == TS_CODE)
       {
          argument[index++] = 0x83;
          argument[index++] = 0x01; /* Length */
          argument[index++] = registerSS_ARG.basic_service.bs_code;
       }
   }

   if (registerSS_ARG.cm_forwarded_to_number.present == TRUE && registerSS_ARG.cm_forwarded_to_number.length <= sizeof(registerSS_ARG.cm_forwarded_to_number.data))
   {
      argument[index++] = 0x84;
      argument[index++] = registerSS_ARG.cm_forwarded_to_number.length;
      memscpy(&(argument[index]),
          sizeof(argument)-index,
          registerSS_ARG.cm_forwarded_to_number.data,
          registerSS_ARG.cm_forwarded_to_number.length);
      index += registerSS_ARG.cm_forwarded_to_number.length;
   }

   if (registerSS_ARG.cm_forwarded_to_subaddress.present == TRUE && registerSS_ARG.cm_forwarded_to_subaddress.length <= sizeof(registerSS_ARG.cm_forwarded_to_subaddress.data))
   {
      argument[index++] = 0x86;
      argument[index++] = registerSS_ARG.cm_forwarded_to_subaddress.length;
      memscpy(&(argument[index]),
          sizeof(argument)-index,
          registerSS_ARG.cm_forwarded_to_subaddress.data,
          registerSS_ARG.cm_forwarded_to_subaddress.length);
      index += registerSS_ARG.cm_forwarded_to_subaddress.length;
   }

   if (registerSS_ARG.no_reply_condition_timer.present == TRUE)
   {
      argument[index++] = 0x85;
      argument[index++] = 0x01;      /* integer of max value = 30 ( 1 byte needed) */
      argument[index++] = registerSS_ARG.no_reply_condition_timer.timer_value;
   }

   if((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL7 && mm_nv_nas_rel_compliance < NV_NAS_COMPLIANCE_REL_MAX) &&
      long_ftn_support)
   {
     if (registerSS_ARG.cm_forwarded_to_number.present == TRUE)
     {
     /* longFTN-Supported */ 
        argument[index++] = 0x89; /* Tag */
        argument[index++] = 0x00; /* Length */
     }
   }
   /* fixed compile warning C4244 */

   *size_ptr = (byte)(index - 2);

   return (index);

}

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
/*
 * Function name :  MN_put_lcs_molr_ARG
 * -------------
 *
 * Description :  Create an SS message argument from the input data
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: argument
 * ----------  lcs_molr_ARG
 *             
 *             
 *
 *
 * Returns:    
 * --------
 *
 */
int
MN_put_lcs_molr_ARG(byte *argument,
lcs_molr_ARG_T *lcs_molr_ARG)
{
   byte index  = 0;
   byte *size_ptr;

   /* Create the SS message */

   argument[index++] = SEQUENCE_TAG;
   size_ptr = &(argument[index++]);

   /* MOLR Type */
   argument[index++] = (CONTEXT_SPECIFY_PRIMITIVE | 0); 
   argument[index++] = 0x01;
   argument[index++] = (byte)lcs_molr_ARG->molr_type;

   /* Location Method */
   if (lcs_molr_ARG->location_method.present == TRUE)
   {
      argument[index++] = (CONTEXT_SPECIFY_PRIMITIVE | 1);
      argument[index++] = 0x01; /* Length */
      argument[index++] = (byte)lcs_molr_ARG->location_method.data;
   }

   /* LCS QOS */
   if (lcs_molr_ARG->lcs_qos.present == TRUE)
   {
      byte *qos_size_ptr;
      byte begin_index = index;

      argument[index++] = ( CONTEXT_SPECIFY_CONSTRUCT | 2);
      qos_size_ptr = &argument[index++];

      /* Horiz Accuracy */
      if (lcs_molr_ARG->lcs_qos.horiz_accuracy.present == TRUE)
      {
         argument[index++] = ( CONTEXT_SPECIFY_PRIMITIVE | 0 );
         argument[index++] = 0x01;
         argument[index++] = lcs_molr_ARG->lcs_qos.horiz_accuracy.data;
      }

      /* Vertical Coord Request */
      if (lcs_molr_ARG->lcs_qos.vertical_coord_req.present == TRUE)
      {
         argument[index++] = ( CONTEXT_SPECIFY_PRIMITIVE | 1 );
         argument[index++] = 0x00;
      }

      /* Vertical Accuracy */
      if (lcs_molr_ARG->lcs_qos.vertical_accuracy.present == TRUE)
      {
         argument[index++] = ( CONTEXT_SPECIFY_PRIMITIVE | 2 );
         argument[index++] = 0x01;
         argument[index++] = lcs_molr_ARG->lcs_qos.vertical_accuracy.data;
      }

      /* Response Time */
      if (lcs_molr_ARG->lcs_qos.response_time.present == TRUE)
      {
         argument[index++] = ( CONTEXT_SPECIFY_CONSTRUCT | 3 );
         argument[index++] = 0x03;
         argument[index++] = ENUMERATED;
         argument[index++] = 0x01;
         argument[index++] = (byte)lcs_molr_ARG->lcs_qos.response_time.response_time_category;
      }

      /* Extension Container Not Supported */
      *qos_size_ptr = (index - begin_index) - 2;
   }

   /* LCS Client External ID */
   if (lcs_molr_ARG->external_id.present == TRUE)
   {
      byte begin_index = index;
      byte *external_id_size_ptr;

      argument[index++] = ( CONTEXT_SPECIFY_CONSTRUCT | 3);
      external_id_size_ptr = &argument[index++];

      /* External Address */
      if ( ( lcs_molr_ARG->external_id.external_address.present == TRUE ) &&
          (lcs_molr_ARG->external_id.external_address.size <= sizeof(lcs_molr_ARG->external_id.external_address.data) ) )
      {
        argument[index++] = ( CONTEXT_SPECIFY_PRIMITIVE | 0 );
        argument[index++] = lcs_molr_ARG->external_id.external_address.size;

        memscpy(&argument[index],
               sizeof(argument)-index,
               lcs_molr_ARG->external_id.external_address.data,
               lcs_molr_ARG->external_id.external_address.size);
        index += lcs_molr_ARG->external_id.external_address.size;
      }

      *external_id_size_ptr = (index - begin_index) - 2;
   }

   /* MLC Number */
   if (lcs_molr_ARG->mlc_number.present == TRUE)
   {
     mlc_number_T *mlc_number = &lcs_molr_ARG->mlc_number;

     /* Clip MLC number at 9 bytes */
     if ( mlc_number->size > sizeof(mlc_number->data) )
     {
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= truncating mlc number length, length before truncating = %d ",mlc_number->size);
       mlc_number->size = sizeof(mlc_number->data);
     }

     argument[index++] = ( CONTEXT_SPECIFY_PRIMITIVE | 4 );
     argument[index++] = mlc_number->size;

     memscpy(&argument[index], sizeof(argument)-index ,mlc_number->data, mlc_number->size );

     index += mlc_number->size;

   }

   /* GPS Assistance Data */
   if (lcs_molr_ARG->gps_assistance_data.present == TRUE)
   {

     /* Clip size at 38 bytes */
     if ( lcs_molr_ARG->gps_assistance_data.size > sizeof(lcs_molr_ARG->gps_assistance_data.data) )
     {
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= truncating gps assistance data length, length before truncating = %d ",lcs_molr_ARG->gps_assistance_data.size);
       lcs_molr_ARG->gps_assistance_data.size = sizeof(lcs_molr_ARG->gps_assistance_data.data);
     }

     argument[index++] = ( CONTEXT_SPECIFY_PRIMITIVE | 5 );
     argument[index++] = lcs_molr_ARG->gps_assistance_data.size;

     memscpy(&argument[index], sizeof(argument)-index ,
            lcs_molr_ARG->gps_assistance_data.data, 
            lcs_molr_ARG->gps_assistance_data.size);
     index += lcs_molr_ARG->gps_assistance_data.size;
   }

   /* Supported GAD Shapes */
   if (lcs_molr_ARG->supported_gad_shapes.present == TRUE)
   {
      argument[index++] = ( CONTEXT_SPECIFY_PRIMITIVE | 6 );
      argument[index++] = 0x2;
      argument[index++] = 0x1; /* One zero bit at end */
      argument[index++] = lcs_molr_ARG->supported_gad_shapes.data;
   }

   /* fixed compile warning C4244 */
   *size_ptr = (index - 2);

   return (index);

}

#endif

#ifdef FEATURE_CCBS

/*
 * Function name :  MN_put_eraseCCEntry_ARG
 * -------------
 *
 * Description :  Create an SS message argument from the input data
 * -----------    
 *
 * Parameters: argument
 * ---------- 
 *
 * Returns:    
 * --------
 */
int MN_put_eraseCCEntry_ARG (byte *argument, cm_erase_cc_entry_req_T *pMsg)
{
  int    index=0;
  byte  *size_ptr;

  /* Create the SS message */
  argument[index++] = SEQUENCE_TAG;
  size_ptr = &(argument[index++]);

  argument[index++] = 0x80;   /* [0] IMPLICIT OCTET STRING */
  argument[index++] = 0x01;
  argument[index++] = pMsg->ss_code;

  if (pMsg->ccbs_index.present)
  {
    argument[index++] = 0x81; /* [1] IMPLICIT INTEGER ( 1 .. 5 ) */
    argument[index++] = 0x01;
    argument[index++] = pMsg->ccbs_index.ccbs_Index;
  }

  *size_ptr = (byte)index - 2;

  return index;
}
#endif /* FEATURE_CCBS */


/*
 * Function name :  MN_put_ss_code
 * -------------
 *
 * Description :  Create an SS message argument from the input data
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: argument
 * ----------  registerSS_ARG
 *             
 *             
 *
 *
 * Returns:    
 * --------
 *
 */
int
MN_put_ss_code(byte ss_code,
byte *argument)
{
   int size;


   argument[0] = 0x04;   /* universal octet string */
   argument[1] = 0x01;   /* ss code length */
   argument[2] = ss_code;

   size = 3;

   return (size);

}


/*
 * Function name :  MN_put_ussd_Res
 * -------------
 *
 * Description :  Create an SS message argument from the input data
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: argument
 * ----------  ussd_Res
 *                    argument
 *                    argument_data_size: size of buffer allocated to argument is (argument_data_size + USSD_RESPONSE_OCTET_SIZE)
 *             
 *             
 *
 *
 * Returns:    
 * --------
 *
 */
int
MN_put_ussd_Res(uss_data_T *ussd_Res,
byte *argument,
byte argument_data_size)
{
   int index;

   index = 0;
   argument_data_size = MIN(argument_data_size, sizeof(ussd_Res->ussData));
   argument_data_size = MIN(argument_data_size, ussd_Res->size); /* Void check as caller passed a_d_s as ussd_Res->size*/

   argument[index++] = SEQUENCE_TAG;

   if (argument_data_size > 127)
   {
      MN_put_element_length(argument,
          &index,
          (argument_data_size + 6));
   }

   else
   {
      MN_put_element_length(argument,
          &index,
          (argument_data_size + 5));
   }

   argument[index++] = 0x04;   /* universal octet string */
   argument[index++] = 0x01;  /* string size */
   argument[index++] = ussd_Res->uss_data_coding_scheme;
   argument[index++] = 0x04;   /* universal octet string */

   MN_put_element_length(argument,
       &index,
       argument_data_size);

   memscpy(&(argument[index]), sizeof(argument) -index,
       ussd_Res->ussData,
       argument_data_size);

   return (argument_data_size + index);

}


/*
 * Function name :  MN_put_ussd_Arg
 * -------------
 *
 * Description :  Create an SS message argument from the input data
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: argument
 * ----------  ussd_Arg
 *             
 *             
 *
 *
 * Returns:    
 * --------
 *
 */
int
MN_put_ussd_Arg(uss_data_T ussd_Arg,
byte *argument, byte argument_data_size)
{
   byte size;
   int index = 0;

   /* Insert a sequence IE */

   argument_data_size = MIN( argument_data_size, (sizeof(ussd_Arg.ussData)) );
   argument_data_size = MIN( argument_data_size, ussd_Arg.size);
  
   argument[index++] = SEQUENCE_TAG;

   /* Calculate the sequence size */

   size = argument_data_size + 5;

   if (  argument_data_size > 127 )

   {

      size++;   /* two octet size */

   }

   MN_put_element_length(argument,
       &index,
       size);

   /* Put the ussd data coding scheme */

   argument[index++] = 0x04;   /* universal octet string */
   argument[index++] = 0x01;  /* string size */
   argument[index++] = ussd_Arg.uss_data_coding_scheme;

   /* Put the uss Data */

   argument[index++] = 0x04;   /* universal octet string */

   MN_put_element_length(argument,
       &index,
       argument_data_size);

   memscpy(&(argument[index]), sizeof(argument)-index,
       ussd_Arg.ussData,
       argument_data_size);

   size = (argument_data_size + index);

   return (size);

}



/*
 * Function name :  MN_put_crss_reject
 * -------------
 *
 * Description :  Create an SS message argument from the input data
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: ss_problem
 * ----------  connection_id
 *             invoke_id
 *             
 *
 *
 * Returns:    None
 * --------
 *
 */
VOID_FUNC
MN_put_crss_reject(byte problem_code_tag,
byte problem_code,
connection_id_T connection_id,
byte invoke_id)
{
   /* Create reject component for a  Facility IE */

   byte   *components;
#ifndef FEATURE_MODEM_HEAP
   if((components = (byte *)gs_alloc(8)) != NULL)
#else
   if((components = (byte *)modem_mem_calloc(1, 8, MODEM_MEM_CLIENT_NAS)) != NULL)
#endif

   {
      components[0] = REJECT;
      components[1] = 0x06;    /* length */
      components[2] = INVOKE_ID_TAG;
      components[3] = 0x01;
      components[4] = invoke_id;
      components[5] = problem_code_tag;
      components[6] = 0x01;
      components[7] = problem_code;

      MN_put_MNCC_FACILITY_REQ(connection_id,
                               components,
                               0x08,      /* length */
                               NULL,      /* ss version */
                               0x00);

#ifndef FEATURE_MODEM_HEAP
      gs_free(components);
#else
      modem_mem_free(components, MODEM_MEM_CLIENT_NAS);
#endif
   }
}


/*
 * Function name :  MN_put_element_length
 * -------------
 *
 * Description :  Create an X.209 element length element
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: *index which points at the place for the 
 * ----------   size element in the output octet stream.
 *              *stream which points at the start of the
 *              output stream.
 *              length
 *
 *
 * Returns:    
 * --------
 *
 */
VOID_FUNC
MN_put_element_length(byte *stream,
int    *index,
int    length)
{

   /*Locals*/

   if ( length > 127)

   {

      stream[*index] = (byte)(0x81);
      (*index)++;

   }

   stream[*index] = (byte)length;

   (*index)++;


}


/*
 * Function name :  MN_handle_ss_info
 * -------------
 *
 * Description :  Breaks down the ss_info from parameter list
 * -----------    in to structures understood by MMI
 *
 *
 *
 *
 *
 *
 * Parameters: parameters
 * ----------  ss_info
 *
 *
 *
 * Returns:    none
 * --------
 *
 */
boolean MN_handle_ss_info(byte parameters[], ss_info_T *ss_info, byte *last_octet_parameters_buf)
{
   int                       index = 0;
   int                       ff_index = 0;
   int                       parameter_length,sequence_length;
   byte                      *pointer;
   int                       end_of_inner_sequence,end_of_outer_sequence;
   boolean                   success = TRUE;
   return_size_T             size;


   /* Decode parameters containg an ss_info parameter */

   switch (parameters[index])
   {
     case 0xa0:  /*Context specific Construct [0] forwardingInfo */
     {
     ss_info->info_type = FWD_INFO;
     ss_info->info.forwarding_info.present = TRUE;

     index++;

     /* Get the parameter length */

     pointer=&(parameters[index]);

     size = MN_get_element_length(pointer, SS_MAX_FORWARDING_INFO_LENGTH, (last_octet_parameters_buf - pointer + 1));

     parameter_length = size.length;

     index += size.increment;


     if ((&(parameters[index+2]) <= last_octet_parameters_buf) && /* reading three octets in this block*/
          ( parameters[index] == 0x04))   /* Universal Octet String */
     {

        index++;

        if (parameters[index++] != 1)

        {

           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect length of SS_Code element");

           success = FALSE;
        }
        else
        {
           ss_info->info.forwarding_info.cm_ss_code.present = TRUE;
           ss_info->info.forwarding_info.cm_ss_code.ss_code = parameters[index];
           index ++;
        }
     }

     if ((&(parameters[index]) <= last_octet_parameters_buf) && 
          (parameters[index] == 0x30))    /* Universal constructor sequence */
     {

        /* This is the forwarding feature list */

        index++;

        if (MN_get_ff_list(parameters, &index,
        &(ss_info->info.forwarding_info.forwarding_feature_list), last_octet_parameters_buf)
        == FALSE)

        {
           success = FALSE;

        }
     }
     else if (&(parameters[index]) > last_octet_parameters_buf)
     {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Buffer Overflow in handling SS info");
       success = FALSE;
     }

     break;

     }

     case 0xa1:  /* Context-specific Construct callBarringInfo */
     {
     ss_info->info_type =  CALL_BARRING_INFO;
     ss_info->info.call_barring_info.present = TRUE;
     /* Calculate parameter length */

     index ++;

     pointer=&(parameters[index]);

     size = MN_get_element_length(pointer, SS_MAX_CALL_BARRING_LENGTH, (last_octet_parameters_buf - pointer + 1));

     parameter_length = size.length;

     index += size.increment;

     if ((&(parameters[index+2]) <= last_octet_parameters_buf) && /* Reading three octets in this block*/
          (parameters[index] == 0x04)) /* Universal Octet String */
     {
        index++;

        if (parameters[index] != 1)
        {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect element in parameter list");

           success = FALSE;
        }

        else
        /* This is the ss code */
        {
           index++;
           ss_info->info_type = CALL_BARRING_INFO;
           ss_info->info.call_barring_info.present = TRUE;
           ss_info->info.call_barring_info.cm_ss_code.present = TRUE;
           ss_info->info.call_barring_info.cm_ss_code.ss_code = parameters[index];
        }
        index++;
     }
     else if (&(parameters[index + 2]) > last_octet_parameters_buf)
     {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Buffer Overflow in handling SS info");
       success = FALSE;
     }


     if ((&(parameters[index]) <= last_octet_parameters_buf) && (parameters[index] == SEQUENCE_TAG))

     /* Universal constructor sequence */

     {
        /* This is the call barring feature list */

        ff_index = 0;
        index ++;

        /* Calculate the end of the sequence */

        pointer=&(parameters[index]);

        size = MN_get_element_length(pointer,SS_MAX_CB_FEATURE_LIST_LENGTH, (last_octet_parameters_buf - pointer + 1));

        sequence_length = size.length;

        index +=  size.increment;

        end_of_outer_sequence = sequence_length + index;

        if(end_of_outer_sequence > MAX_SS_PARAMETER_LENGTH)
        {
          end_of_outer_sequence = MAX_SS_PARAMETER_LENGTH;
        }

        if(&(parameters[end_of_outer_sequence - 1]) > last_octet_parameters_buf)
        {
          end_of_outer_sequence = index; /* end_of_outer_sequence cannot point beyond last octet of the buffer */
        }

        while ((&(parameters[index]) <= last_octet_parameters_buf) &&
                   (index < end_of_outer_sequence))
        {

           if (parameters[index ] == SEQUENCE_TAG)
           {
               index ++;
               pointer=&(parameters[index]);
               size = MN_get_element_length(pointer,SS_MAX_CB_FEATURE_LENGTH, (last_octet_parameters_buf - pointer + 1));
               sequence_length = size.length;
               index += size.increment;
               end_of_inner_sequence = index + sequence_length;

               if(end_of_inner_sequence > end_of_outer_sequence)
               {
                 end_of_inner_sequence = end_of_outer_sequence;
               }

               if(&(parameters[end_of_inner_sequence - 1]) > last_octet_parameters_buf)
               {
                 end_of_inner_sequence = index; /* end_of_inner_sequence cannot point beyond last octet of the buffer */
               }

               while ((&(parameters[index]) <= last_octet_parameters_buf) &&
                          (index < end_of_inner_sequence))
               {

                   switch (parameters[index])
                   {
                       case 0x82:  /* bearer service */
                           {
                               if(index >= end_of_inner_sequence - 2)
                               {
                                 /*
                                  * This case is valid if reading at-least three octets from parameter
                                  * array won't cause read-overflow. Otherwise, we need not proceed with
                                  * any more octets and break.
                                  */
                                 index = end_of_inner_sequence;
                                 break;
                               }

                               ss_info->info.call_barring_info.
                                  call_barring_feature_list[ff_index]. 
                                  basic_service.present = TRUE;

                               ss_info->info.call_barring_info.
                                  call_barring_feature_list[ff_index]. 
                                  basic_service.code_type = BS_CODE;

                               index ++;

                               if (parameters[index] != 1)
                               {
                                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect element in parameter list");

                                   success = FALSE; 
                               }

                               index ++;
                               ss_info->info.call_barring_info.
                               call_barring_feature_list[ff_index].
                               basic_service.bs_code = 
                               parameters[index];
                               index++;
                               break;
                           }

                       case 0x83:  /* tele service */
                           {
                               if(index >= end_of_inner_sequence - 2)
                               {
                                 /*
                                  * This case is valid if reading at-least three octets from parameter
                                  * array won't cause read-overflow. Otherwise, we need not proceed with
                                  * any more octets and break.
                                  */
                                 index = end_of_inner_sequence;
                                 break;
                               }

                               ss_info->info.call_barring_info.
                               call_barring_feature_list[ff_index].
                               basic_service.present = TRUE;

                               ss_info->info.call_barring_info.
                               call_barring_feature_list[ff_index].
                               basic_service.code_type = TS_CODE;
                               index ++;

                               if (parameters[index] != 1)
                               {
                                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect element in parameter list");

                               }

                               index ++;
                               ss_info->info.call_barring_info.
                               call_barring_feature_list[ff_index].
                               basic_service.bs_code = 
                               parameters[index];
                               index++;
                               break;

                           }

                       case 0x84:  /* ss status */
                           {

                               if(index >= end_of_inner_sequence - 2)
                               {
                                 index = end_of_inner_sequence;
                                 break;  // to prevent read overflow
                               }

                               index ++;

                               if (parameters[index] != 1)
                               {
                                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect ss_status element length");

                                   success = FALSE;
                               }

                               index++;

                               MN_get_ss_status(&(ss_info->info.call_barring_info.
                                                call_barring_feature_list[ff_index].
                                                cm_ss_status),
                                                &(parameters[index]));

                               index++;
                               break;
                           }

                       case NULL:  /* Indefinite mode - end of sequence */
                           {
                               index++;

                               if(index >= end_of_inner_sequence)
                               {
                                 break;  // to prevent read overflow
                               }


                               if (parameters[index] != 0)
                               {
                                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect null element length");

                                   success = FALSE;
                               }
                               else
                               {
                                   end_of_inner_sequence = index;
                                   index++;
                               }
                               break;
                           }

                       default:
                           {
                               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unknown element identifier");
                               index++;

                               if(index >= end_of_inner_sequence)
                               {
                                 break;  // to prevent read overflow
                               }


                              /* get size of unknown element */

                               size = MN_get_element_length(&(parameters[index]),
                                                            SS_MAX_RETURN_RESULT_LENGTH,
                                                            (last_octet_parameters_buf - &(parameters[index]) + 1));

                              /* move to end of element size */

                               index+= size.increment;

                             /* move to end of element */

                               index+= size.length;
                               break;
                           }

                   }     /* end switch */
               }     /* end while - inner sequence */
           }        /* end if */

           else if ((&(parameters[index]) <= last_octet_parameters_buf) &&
                        ( parameters[index] == NULL)) /* End of sequence with a indefinite length */
           {
               index++;

               if (parameters[index] != 0)
               {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect null element length");

                   success = FALSE;
               }
               end_of_outer_sequence = index; /* leave sequence */
               index++;

           }
           else
           {
               /* Unexpected value in sequence */
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected component in  parameter list"); 

               success = FALSE;
               end_of_outer_sequence = index;  /* leave sequence */
               index++;
           }


           ff_index ++;

           if(ff_index >= CALL_BARRING_LIST_SIZE)
           {
             index = end_of_outer_sequence;
           }


        }           /* end while */
     }              /* end if */
     else if (&(parameters[index]) > last_octet_parameters_buf)
     {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Buffer Overflow in handling SS info");
       success = FALSE;
     }


     /* Store the number of used elements */

     ss_info->info.call_barring_info.call_barring_info_length = 
         (byte)ff_index;

     break;
     }        /* end case 0xa1 */

     case 0xa2:  /* Construct-specific Construct [2]cugInfo */
     {
     ss_info->info_type = CUG_INFO;
     ss_info->info.cug_info.present = TRUE;
     index ++;

     pointer=&(parameters[index]);

     size = MN_get_element_length(pointer,SS_MAX_CUG_INFO_LENGTH, (last_octet_parameters_buf - pointer + 1));

     parameter_length = size.length;

     index += size.increment;

     if ((&(parameters[index]) <= last_octet_parameters_buf) &&
          (parameters[index] == SEQUENCE_TAG))
     {

        /* This is the CUG subscription list */

        index ++;
        ff_index = 0;

        pointer=&(parameters[index]);

        size = MN_get_element_length(pointer,SS_MAX_CUG_SUB_LIST_LENGTH, (last_octet_parameters_buf - pointer + 1));

        sequence_length = size.length;

        index += size.increment;

        end_of_outer_sequence = index + sequence_length;

        if(end_of_outer_sequence > MAX_SS_PARAMETER_LENGTH)
        {
          end_of_outer_sequence = MAX_SS_PARAMETER_LENGTH;
        }

        if(&(parameters[end_of_outer_sequence - 1]) > last_octet_parameters_buf)
        {
          end_of_outer_sequence = index; /* end_of_outer_sequence cannot point beyond last octet of the buffer */
        }


        while ((&(parameters[index]) <= last_octet_parameters_buf) &&
                   (index < end_of_outer_sequence - 1)) /* Reading at-least two octets, before next check on index*/
        {

           if (parameters[index++] == SEQUENCE_TAG)
           {

               pointer=&(parameters[index]);

               size = MN_get_element_length(pointer,SS_MAX_CUG_SUB_LENGTH, (last_octet_parameters_buf - pointer + 1));

               sequence_length = size.length;

               index += size.increment;

               end_of_inner_sequence = index + sequence_length;

               if(end_of_inner_sequence > end_of_outer_sequence)
               {
                 end_of_inner_sequence = end_of_outer_sequence;
               }

               if(&(parameters[end_of_inner_sequence]) > last_octet_parameters_buf)
               {
                 end_of_inner_sequence = index; /* end_of_inner_sequence cannot point beyond last octet of the buffer */
               }


               ss_info->info.cug_info.cug_subscription_list[ff_index].present = TRUE;

               while ((&(parameters[index]) <= last_octet_parameters_buf) &&
                          (index < end_of_inner_sequence))

              /*****   Decode a CUG subscription *****/

               {

                  /*
                   * If Universal constructor int
                   */

                  if ((parameters[index] == 0x02) && (index >= end_of_inner_sequence -4))
                  {

                    /* This must be the CUG index */

                     index ++;

                    /*cugIndex length = 1 ?? */

                     if (parameters[index] == 0x01)
                     {
                         index++;
                         ss_info->info.cug_info.cug_subscription_list[ff_index].
                            cug_index[0] = parameters[index];
                         ss_info->info.cug_info.cug_subscription_list[ff_index].
                            cug_index[1] = 0;
                         index++;
                     }
                     else if (parameters[index] == 0x02)
                     {
                         index ++;
                         ss_info->info.cug_info.cug_subscription_list[ff_index].
                            cug_index[0] = parameters[index++];
                         ss_info->info.cug_info.cug_subscription_list[ff_index].
                            cug_index[1] = parameters[index++];
                     }
                     else
                     {
                         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length for cug-Index given"); 

                         index=end_of_inner_sequence;
                         success = FALSE;
                     }
                  }
                  else
                  {
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Mandatory element - cug-Index missing");

                      index=end_of_inner_sequence;
                      success = FALSE;
                  }

                  if ((parameters[index] == 0x04)&& 
                         /* Universal octet string */
                      (index < end_of_inner_sequence -2))

                         /*  Decode cug-Interlock */
                  {
                      index++;

                      if (parameters[index++] == 0x04)  /* 4 octets long ? */
                      {
                          memscpy(ss_info->info.cug_info.cug_subscription_list[ff_index].cug_interlock,
                                 sizeof(ss_info->info.cug_info.cug_subscription_list[ff_index].cug_interlock),
                                 &parameters[index], CUG_INTERLOCK_SIZE);

                          /*4 = size of cug interlock*/

                          index+= 4;
                      }
                      else
                      {
                          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect length for cugInterlock given");

                          index=end_of_inner_sequence;
                          success = FALSE;
                      }
                  }

                  else
                  {
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Mandatory element cug-Interlock missing");

                  }


                  if ((parameters[index] == ENUMERATED)&& 

                  /*Universal enumeration*/

                      (index < end_of_inner_sequence - 2))

                  {
                      /* Decode intra-CUG-Options  */

                      index++;

                      if (parameters[index++] == 0x01)/* Must be 1 octet */
                      {
                          ss_info->info.cug_info.cug_subscription_list[ff_index].
                             intra_cug_options = parameters[index++];
                      }

                      else
                      {
                          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Length element in parameters invalid"); 

                          index=end_of_inner_sequence;
                          success = FALSE;
                      }
                  }

                  else
                  {
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Missing mandatory parameter intraCUGOption");

                      index=end_of_inner_sequence;
                      success = FALSE;
                  }

                  if ((parameters[index] == SEQUENCE_TAG)&&
                      (index < end_of_inner_sequence))

                     /* Decode basic service group list */

                  {
                      index++;

                      if(index >= end_of_inner_sequence)
                      {
                        break;
                      }

                      if (MN_get_bsg_list(parameters, &index,
                                          &(ss_info->info.cug_info.cug_subscription_list[ff_index].
                                             basic_service_group_list),
                                             last_octet_parameters_buf)
                             == FALSE)
                      {
                          success = FALSE;
                      }
                  }
               }  /* end while inside inner sequence*/

               ff_index++;

               if(ff_index >= CUG_SUBSCRIPTION_LIST_SIZE)
               {
                 index = end_of_outer_sequence;
               }


           }  /* end if    */
        }     /* end while */

        ss_info->info.cug_info.cug_subscription_list_length = (byte)ff_index;

     }     /* end if    */

     else if (&(parameters[index]) > last_octet_parameters_buf)
     {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Buffer Overflow in handling SS info");
       success = FALSE;
     }


     /* END of CUG Subscription List */

     if ((&(parameters[index]) <= last_octet_parameters_buf) &&
          (parameters[index++] == SEQUENCE_TAG))
     {

        /* This is the CUG feature list */

        ff_index = 0;

        pointer=&(parameters[index]);

        size = MN_get_element_length( pointer,
                      SS_MAX_CUG_FEATURE_LIST_LENGTH,
                      (last_octet_parameters_buf - pointer + 1));

        sequence_length = size.length;

        index += size.increment;

        end_of_outer_sequence = index + sequence_length;

        if(end_of_outer_sequence > MAX_SS_PARAMETER_LENGTH)
        {
          end_of_outer_sequence = MAX_SS_PARAMETER_LENGTH;
        }

        if(&(parameters[end_of_outer_sequence -1]) > last_octet_parameters_buf)
        {
          end_of_outer_sequence = index; /* end_of_outer_sequence cannot point beyond last octet of the buffer */
        }

        while ((&(parameters[index]) <= last_octet_parameters_buf) && 
                   (index < end_of_outer_sequence - 1)) /* Reading two octets from parameter array before next check on index*/
        {

           if(parameters[index++] == SEQUENCE_TAG)
           {

               ss_info->info.cug_info.cug_feature_list[ff_index].present = TRUE;

               pointer=&(parameters[index]);

               size = MN_get_element_length( pointer,SS_MAX_CUG_FEATURE_LENGTH, (last_octet_parameters_buf - pointer + 1));

               sequence_length = size.length;

               index += size.increment;

               end_of_inner_sequence = index + sequence_length;

               if(end_of_inner_sequence > end_of_outer_sequence)
               {
                 end_of_inner_sequence = end_of_outer_sequence;
               }

               if(&(parameters[end_of_inner_sequence -1]) > last_octet_parameters_buf)
               {
                 end_of_inner_sequence = index; /* end_of_inner_sequence cannot point beyond last octet of the buffer */
               }

               while ((&(parameters[index]) <= last_octet_parameters_buf) &&
                          (index < end_of_inner_sequence))
               {

                   switch(parameters[index])
                   {
                       case 0x82:

                           {
                               if(index >= end_of_inner_sequence -2)
                               {
                                 index = end_of_inner_sequence;
                                 break;
                               }
                            /* bearer service */
                               ss_info->info.cug_info.cug_feature_list[ff_index].
                                  basic_service.present = TRUE;

                               ss_info->info.cug_info.cug_feature_list[ff_index].
                                  basic_service.code_type= BS_CODE;
                               index ++;

                               if (parameters[index++] != 1)
                               {
                                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length of bs-code");

                                   index = end_of_inner_sequence;
                                   success = FALSE;
                               }

                               ss_info->info.cug_info.cug_feature_list[ff_index].
                                  basic_service.bs_code = parameters[index];
                               index++;
                               break;
                           }

                       case 0x83:
                           {

                               if(index >= end_of_inner_sequence -2)
                               {
                                 index = end_of_inner_sequence;
                                 break;
                               }

                               /* tele service */
                               ss_info->info.cug_info.cug_feature_list[ff_index].
                                  basic_service.present = TRUE;

                               ss_info->info.cug_info.cug_feature_list[ff_index].
                                  basic_service.code_type = TS_CODE;
                               index ++;

                               if (parameters[index++] != 1)
                               {
                                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid value in parameter list"); 

                                   index = end_of_inner_sequence;
                                   success = FALSE;
                               }

                               ss_info->info.cug_info.cug_feature_list[ff_index].
                                  basic_service.bs_code = parameters[index];
                               index++;
                               break;
                           }

                       case UNIVERSAL_INTEGER:
                           {

                               if(index >= end_of_inner_sequence -4)
                               {
                                 index = end_of_inner_sequence;
                                 break;
                               }

                               /* preferentialCUGIndicator */

                               ss_info->info.cug_info.cug_feature_list[ff_index].
                                  preferential_cug_indicator.present = TRUE;
                               index ++;

                               if (parameters[index] == 0x01)
                               {
                                   index++;
                                   ss_info->info.cug_info.cug_feature_list[ff_index].
                                      preferential_cug_indicator.cug_indicator[0] = parameters[index++];
                                   ss_info->info.cug_info.cug_feature_list[ff_index].
                                      preferential_cug_indicator.cug_indicator[1] = 0;
                               }

                               else if (parameters[index] == 0x02)
                               {
                                   index++;
                                   ss_info->info.cug_info.cug_feature_list[ff_index].
                                      preferential_cug_indicator.cug_indicator[0] = parameters[index++];
                                   ss_info->info.cug_info.cug_feature_list[ff_index].
                                      preferential_cug_indicator.cug_indicator[1] = parameters[index++];
                               }

                               else
                               {
                                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid value for length of cug indicator");

                                   index = end_of_inner_sequence;
                                   success = FALSE;
                               }
                               break;
                           }

                       case OCTET_STRING:
                           {

                               if(index >= end_of_inner_sequence -2)
                               {
                                 index = end_of_inner_sequence;
                                 break;
                               }

                               /* interCugRestrictions */

                               ss_info->info.cug_info.cug_feature_list[ff_index].
                                  inter_cug_restrictions.present = TRUE;
                               index ++;
                               if (parameters[index++] == 0x01)
                               {

                                   /* interCugRestrictions is 1 byte long */

                                   ss_info->info.cug_info.cug_feature_list[ff_index].
                                      inter_cug_restrictions.restrictions = parameters[index++];
                               }
                               else
                               {
                                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid value for length of restrictions");

                                   index = end_of_inner_sequence;
                                   success = FALSE;
                               }
                               break;
                           }

                       default:
                           {
                               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid id for cug_feature_list parameters");

                               index = end_of_inner_sequence;
                               success = FALSE;
                           }
                   }  /* end switch */
               }  /* end while  */
           }   /* end if     */

           ff_index ++;   /* increment feature list index */

           if(ff_index >= CUG_FEATURE_LIST_SIZE)
           {
             index = end_of_outer_sequence;
           }


        }     /* end while inside outer sequence */

        ss_info->info.cug_info.cug_feature_list_length = (byte)ff_index;
     }     /* end if  SEQUENCE TAG*/

     else if (&(parameters[index]) > last_octet_parameters_buf)
     {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Buffer Overflow in handling SS info");
       success = FALSE;
     }

     break;
     }        /* end case   */

     case 0xa3:  /* Context-specific Construct */
     {

     /* ss data */
     ss_info->info_type = SS_DATA_INFO;
     ss_info->info.ss_data.present = TRUE;
     index++;

     pointer=&(parameters[index]);

     size = MN_get_element_length(pointer,SS_MAX_SS_DATA_LENGTH, (last_octet_parameters_buf - pointer + 1));

     parameter_length = size.length + index + 1;

     if(parameter_length > MAX_SS_PARAMETER_LENGTH)
     {
       parameter_length = MAX_SS_PARAMETER_LENGTH;
     }

     if(&(parameters[parameter_length -1]) > last_octet_parameters_buf)
     {
       parameter_length = index; /* parameter_length cannot point beyond last octet of the buffer */
     }


     index += size.increment; /* point at IE tag */

     while ((&(parameters[index]) <= last_octet_parameters_buf) && 
                (index < (parameter_length)))
     {
        switch(parameters[index])
        {
           case OCTET_STRING:   /* ss code */
           {
               if(index >= parameter_length -2)
               {
                 index = parameter_length;
                 break; // to prevent read overflow
               }
               index ++;

               if (parameters[index++] == 0x01) /* size of ss-code */
               {
                   ss_info->info.ss_data.cm_ss_code.present = TRUE;
                   ss_info->info.ss_data.cm_ss_code.ss_code = parameters[index++];
               }
               else
               {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid value for size of ss code");

                   index = parameter_length;
                   success = FALSE;
               }
               break;
           }

           case 0x84:
           {
           /* ss status */

               if(index >= parameter_length -2)
               {
                 index = parameter_length;
                 break; // to prevent read overflow
               }


               index++;
               if (parameters[index++] ==  0x01)   /*size of ss_status */
               {

                   MN_get_ss_status(&(ss_info->info.ss_data.cm_ss_status),
                      &(parameters[index]));
                   index++;
               }
               else
               {

                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length element for ss_status");

                   index = parameter_length;
                   success = FALSE;
               }
               break;
           }

           case 0x82:
           {

               if(index >= parameter_length -2)
               {
                 index = parameter_length;
                 break; // to prevent read overflow
               }

              /* clir restriction option */
               index++;
               if (parameters[index++] ==  0x01) /*size of clir restricted */
               {
                   ss_info->info.ss_data.ss_subscription_option.option.
                      clir_restriction_option.present = TRUE;
                   ss_info->info.ss_data.ss_subscription_option.option.
                      clir_restriction_option.option = parameters[index++];
               }

               else
               {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length element for clir restriction");

                   index = parameter_length;
                   success = FALSE;
               }
               break;
           }

           case 0x81:
           {

               if(index >= parameter_length -2)
               {
                 index = parameter_length;
                 break; // to prevent read overflow
               }

               index ++;
               if (parameters[index++] == 0x01)  /* size of override cat */ 
               {
                   ss_info->info.ss_data.ss_subscription_option.option.
                      override_category.present = TRUE;
                   ss_info->info.ss_data.ss_subscription_option.option.
                      override_category.value = parameters[index++];
               }

               else
               {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid length element for override cat");

                   index = parameter_length;
                   success = FALSE;
               }
               break;
           }

           case SEQUENCE_TAG:
           {
              /* Basic group service list */
               index++;

               if (MN_get_bsg_list(parameters, &index, &(ss_info->info.ss_data.basic_service_group_list), last_octet_parameters_buf)
                      == FALSE)
               {
                   success = FALSE;
               }
               break;
           }

           default:
           {

               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid data type identifier for ss_data");

               index = parameter_length;
               success = FALSE;
               break;
           }
        }  /* end switch */
     }      /* end while  */

     ss_info->info.ss_data.length = 1;

     break;

     }         /* end case   */

     default:
     {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invalid ss_info parameter identifier");
         success = FALSE;
     }
   }
   return(success);
}



/*
 * Function name :  MN_handle_result
 * -------------
 *
 * Description :  Breaks down the ss_info from parameter list
 * -----------    in to structures understood by MMI
 *
 *
 * Parameters: facility
 * ----------  cm_ss_ref
 *             ss_operation
 *
 *
 * Returns:    none
 * --------
 *
 */
boolean MN_handle_result( const MN_FACILITY_DATA_T *facility_data, 
                          byte cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                          sys_modem_as_id_e_type as_id,
#endif
                          byte ss_operation)
{

  ss_password_info_T   *ss_password_info;
  byte                 *parameters,*ptr;
  byte                 *last_octet_parameters_buf;
  boolean               success = TRUE;
  ss_UserData_info_T   *ss_UserData_info;
  InterrogateSS_Res_T  *InterrogateSS_Res;
  ss_data_info_T       *ss_data_info;
#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
  lcs_molr_res_T       *lcs_molr_res;
#endif
  int                   index;
  return_size_T         size;
  ss_info_T            *ss_info;
  cm_ss_conf_e_type     ss_conf_type = NO_INFO;


  parameters = (byte *)facility_data->facility_component_data[0].parameters;
  last_octet_parameters_buf = parameters + MAX_SS_PARAMETER_LENGTH - 1;

  switch (ss_operation)
  {
    case interrogateSS:
    {
/* get a buffer */
#ifndef FEATURE_MODEM_HEAP

      InterrogateSS_Res = (InterrogateSS_Res_T *) &buffer;
#else
      InterrogateSS_Res = (InterrogateSS_Res_T *) modem_mem_calloc(1, sizeof(InterrogateSS_Res_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (InterrogateSS_Res != NULL)
      {
#endif

/* Clear the local buffer */
        memset(InterrogateSS_Res, NULL, sizeof(InterrogateSS_Res_T));

        if (facility_data->facility_component_data[0].parameter_length != 0)
        {
          (void)MN_handle_interrogate_ss_res(parameters, &ss_conf_type, InterrogateSS_Res, last_octet_parameters_buf);
        }

        MN_put_cm_interrogate_ss_conf(success, 
                                       cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                       as_id,
#endif
                                       ss_conf_type, 
                                       InterrogateSS_Res);     
#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(InterrogateSS_Res, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
        
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_interrogate_ss_conf(FALSE, 
                                       cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                       as_id,
#endif
                                       NO_INFO, 
                                       InterrogateSS_Res);
        success = FALSE;
      }
#endif
      break;
    }

    case activateSS:
    {
/* requisition temporary memory */
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer;
#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);


      if (ss_info != NULL)
      {

#endif
/* Clear the local buffer */
        memset(ss_info, NULL, sizeof(ss_info_T));
        if (facility_data->facility_component_data[0].parameter_length == 0)
        {
          success = TRUE;
          ss_info->info_type = NO_INFO;
        }
        else
        {
          (void)MN_handle_ss_info(parameters, ss_info,last_octet_parameters_buf);
        }

        MN_put_cm_activate_ss_conf(success, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_activate_ss_conf(FALSE, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);
        success = FALSE; 
      }   

#endif
      break;
    }

    case registerSS:
    /* Requisition some memory */     
    {
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer;
#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);


      if (ss_info != NULL)
      {

#endif
        /* Clear the local buffer */
        memset(ss_info, NULL, sizeof(ss_info_T));

        if (facility_data->facility_component_data[0].parameter_length == 0)
        {
          success = TRUE;
          ss_info->info_type = NO_INFO;
        }
        else
        {
          (void)MN_handle_ss_info(parameters, ss_info, last_octet_parameters_buf);
        }

        MN_put_cm_register_ss_conf(success, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_register_ss_conf(FALSE, 
                                    cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                    as_id,
#endif
                                    ss_info);
        success = FALSE;
      }   
#endif

      break;
    }

    case eraseSS:
    {
      /* requisition some memory */
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer;      
#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_info != NULL)
      {
#endif

        /* Clear the local buffer */
        memset(ss_info, NULL, sizeof(ss_info_T));

        if (facility_data->facility_component_data[0].parameter_length == 0)
        {
          success = TRUE;
          ss_info->info_type = NO_INFO;
        }
        else
        {
          (void)MN_handle_ss_info(parameters, ss_info, last_octet_parameters_buf);
        }
        MN_put_cm_erase_ss_conf(success, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 ss_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_erase_ss_conf(FALSE, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 ss_info);
        success = FALSE;
      }
#endif
      
      break;
    }

    case deactivateSS:
    {
      /* Get some memory */
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer;

#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);
      if (ss_info != NULL)
      {
#endif
      
        /* Clear the local buffer */
        memset(ss_info, NULL, sizeof(ss_info_T));

        if (facility_data->facility_component_data[0].parameter_length == 0)
        {
          success = TRUE;
          ss_info->info_type = NO_INFO;
        }
        else
        {
          (void)MN_handle_ss_info(parameters, ss_info, last_octet_parameters_buf);
        }

        MN_put_cm_deactivate_ss_conf(success, 
                                      cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                      as_id,
#endif
                                      ss_info);
#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_deactivate_ss_conf(FALSE, 
                                      cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                      as_id,
#endif
                                      ss_info);
        success = FALSE;
      }
#endif
      
      break;
    }

    case registerPassword:
    {
      /* Requisition some memory */
#ifndef FEATURE_MODEM_HEAP
      ss_password_info = (ss_password_info_T *) &buffer;
#else
      ss_password_info = (ss_password_info_T *) modem_mem_calloc(1, sizeof(ss_password_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_password_info != NULL)
      {
#endif

        memset(ss_password_info, NULL, sizeof(ss_password_info_T));

        if ((parameters[0] == NUMERIC_STRING) &&
            (parameters[1] == 4))  /* Length */
        {
          ss_password_info->data_type = NEW_PWD_INFO;
          ss_password_info->data.ss_new_password.present = TRUE;

          memscpy(ss_password_info->data.ss_new_password.ss_password,
                 sizeof(ss_password_info->data.ss_new_password.ss_password),
                 parameters + 2,
                 4);

          success = TRUE;
        }
        else
        {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected contents in SS Register Password Result");
        }

        MN_put_cm_register_password_conf(success, 
                                          cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                           as_id,
#endif
                                          ss_password_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_password_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_register_password_conf(FALSE, 
                                          cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM ) 
                                           as_id,
#endif
                                          ss_password_info);
        success = FALSE;
      }
#endif
   
      break;
    }

    case processUnstructuredSS_Data:
    {
      success = FALSE;
      index = 0;

#ifndef FEATURE_MODEM_HEAP
      ss_UserData_info =  (ss_UserData_info_T *) &buffer;         

#else
      ss_UserData_info = (ss_UserData_info_T *) modem_mem_calloc(1, sizeof(ss_UserData_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_UserData_info != NULL)
      {
#endif

        memset(ss_UserData_info, NULL, sizeof(ss_UserData_info_T));

        if (facility_data->facility_component_data[0].parameter_length != 0)
        {
          if (parameters[index++] == IA5_STRING)
          {
            ss_UserData_info->data_type = USS_DATA_INFO;
            ss_UserData_info->data.ss_User_Data.present = TRUE;

            ptr = &(parameters[index]);

            size = MN_get_element_length(ptr,SS_MAX_USS_DATA_LENGTH, (last_octet_parameters_buf - ptr + 1));

            index += size.increment;

            ss_UserData_info->data.ss_User_Data.size = (byte)size.length;

            memscpy(ss_UserData_info->data.ss_User_Data.ss_UserData, sizeof(ss_UserData_info->data.ss_User_Data.ss_UserData) ,&(parameters[index]),
                   (unsigned int)size.length);

            success = TRUE;
          }
          else
          {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected contents in SS Process Unstructured");
          }
        }
        else
        {
          ss_UserData_info->data_type = NO_INFO;
          ss_UserData_info->data.ss_User_Data.present = FALSE;

          success = TRUE;
        }

        MN_put_cm_process_unstructured_ss_data_conf(success, 
                                                     cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                     as_id,
#endif
                                                     ss_UserData_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_UserData_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_process_unstructured_ss_data_conf(FALSE, 
                                                     cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                     as_id,
#endif
                                                     ss_UserData_info);
        success = FALSE;
      }
#endif

      break;
    }

    case processUnstructuredSS_Request:
    {


      success = FALSE;
      index = 0;

#ifndef FEATURE_MODEM_HEAP
      ss_data_info = (ss_data_info_T *) &buffer;          

#else
      ss_data_info = (ss_data_info_T *) modem_mem_calloc(1, sizeof(ss_data_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_data_info != NULL)
      {
#endif

        memset(ss_data_info, NULL, sizeof(ss_data_info_T));

        /* Decode SEQUENCE */
        if (parameters[index++] == SEQUENCE_TAG)
        {
          return_size_T  sequence_size;

          ss_data_info->data_type = USS_DATA_INFO;
          ss_data_info->data.uss_data.present = TRUE;

          ptr = &(parameters[index]);

          sequence_size = MN_get_element_length(ptr,SS_MAX_USS_DATA_LENGTH, (last_octet_parameters_buf - ptr + 1));

          index += sequence_size.increment;

          /* Decode USS_DATA_CODING_SCHEME  */
          if ((parameters[index++] == OCTET_STRING) &&
              (parameters[index++] == 1))   /* Length */
          {
            ss_data_info->data.uss_data.uss_data_coding_scheme = 
            parameters[index++];

            /* Decode USS_DATA */
            if (parameters[index++] == OCTET_STRING)
            {
              ptr = &(parameters[index]);

              size = MN_get_element_length(ptr,SS_MAX_USS_REQ_LENGTH, (last_octet_parameters_buf - ptr + 1));

              ss_data_info->data.uss_data.size = (byte)size.length;

              index += size.increment;

              memscpy(ss_data_info->data.uss_data.ussData, sizeof(ss_data_info->data.uss_data.ussData) ,&(parameters[index]),
                     (unsigned int)size.length);

              /* Check if USSD contents doesn't go over what it is supposed
               * to be by comparing with SEQUENCE length and etc. */
              if ((size.length + index) == (sequence_size.length + sequence_size.increment + 1/*SEQUENCE type octet*/))
              {
                success = TRUE;
              }
              else
              {
                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Unexpected parameters");
              }
            }
            else
            {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Unexpected parameters tag");
            }
          }
          else
          {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Unexpected parameters tag");
          }
        }
        else
        {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected contents in unstructured_ss_conf");

        }

        MN_put_cm_process_unstructured_ss_conf(success, 
                                                cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                as_id,
#endif
                                                ss_data_info);

#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_data_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_process_unstructured_ss_conf(FALSE, 
                                                cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                as_id,
#endif
                                                ss_data_info);
        success = FALSE;
      }
#endif

      break;
    }

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
    case lcs_MOLR:
    /* get a buffer */     
    {
#ifndef FEATURE_MODEM_HEAP
      lcs_molr_res = (lcs_molr_res_T *) &buffer;
#else
      lcs_molr_res = (lcs_molr_res_T *) modem_mem_calloc(1, sizeof(lcs_molr_res_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (lcs_molr_res != NULL)
      {
#endif


        /* Clear the local buffer */
        memset(lcs_molr_res, NULL, sizeof(lcs_molr_res_T));

        if (facility_data->facility_component_data[0].parameter_length == 0)
        {
          success = TRUE;  
          lcs_molr_res->data_type = LCS_MOLR_INFO;  
        }
        else
        {
          (void)MN_handle_lcs_molr_res(parameters, lcs_molr_res, last_octet_parameters_buf);
        }

        MN_put_cm_lcs_molr_conf(success, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 lcs_molr_res);


#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(lcs_molr_res, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);
      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_lcs_molr_conf(FALSE, 
                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                 as_id,
#endif
                                 lcs_molr_res);
        success = FALSE;
      }
#endif
         
      break;
    }
#endif

#ifdef FEATURE_CCBS
    case eraseCCEntry:
    {
      /* requisition temporary memory */
#ifndef FEATURE_MODEM_HEAP
      ss_info = (ss_info_T *) &buffer;

#else
      ss_info = (ss_info_T *) modem_mem_calloc(1, sizeof(ss_info_T),MODEM_MEM_CLIENT_NAS); 
      MN_print_ss_status(MEM_ALLOC,ss_operation);

      if (ss_info != NULL)
      {
#endif

        /* Clear the local buffer */
        memset(ss_info, NULL, sizeof(ss_info_T));

        success            = TRUE;
        ss_info->info_type = NO_INFO;

        MN_put_cm_erase_cc_entry_conf (success, 
                                        cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                        as_id,
#endif
                                        ss_info); 
#ifdef FEATURE_MODEM_HEAP
        modem_mem_free(ss_info, MODEM_MEM_CLIENT_NAS);
        MN_print_ss_status(MEM_FREE,ss_operation);

      }
      else
      {
        MN_print_ss_status(MEM_ALLOC_FAILURE,ss_operation);
        MN_put_cm_erase_cc_entry_conf (FALSE, 
                                        cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                        as_id,
#endif
                                        ss_info);
        success = FALSE;
        success = FALSE;
      }
#endif
   
      break;
    }
#endif /* FEATURE_CCBS */

    default:
    {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected SS Operation in Return Result");

      success = FALSE;

      break;
    }
  }

  return(success);
}



/*
 * Function name :  MN_get_ect_indicator()
 * -------------
 *
 * Description :  
 * -----------    
 *   Parse the ECT indicator information element into friendly data structure that 
 *    sent to CM/UI for User notification.
 *
 * Parameters: 
 * ----------  
 *     ie_cm_notify_ss_T - notify SS structure sent to CM.
 *     parameter_pointer - the ect indication IE.
 *     index             - index to the byte order of the IE.
 *
 * Returns:    none
 * --------
 *
 */


boolean MN_get_ect_indicator(ie_cm_notify_ss_T *notify_data, byte *parameter_pointer, int *notify_index, byte *last_octet_data_pointer )
{
    int index = 0, seq_length = 0, seq_end, number = 0;
    boolean success = TRUE;
    byte size;

    if( (parameter_pointer + 1) > last_octet_data_pointer)
    {
      success = FALSE;
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Inconsistent data stream, not procesing inside MN_get_ect_indicator");
    }
    else
    {
      index++; /* ect-Indicator TAG */
      parameter_pointer++;
  
      seq_length = *parameter_pointer;
  
      index++; /* ect-Indicator TAG length */
      parameter_pointer++;
    }

    if( &(parameter_pointer[seq_length -1]) > last_octet_data_pointer)
    {
      success = FALSE;
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Inconsistent data stream, not procesing inside MN_get_ect_indicator");
    }


    while( (success == TRUE) && (index < seq_length))
    {
        switch (*parameter_pointer)
        {
             case 0x80:  /* ect0CallState */
             {
                 notify_data->cm_ect_indicator.present = TRUE; 

                 index++;
                 parameter_pointer++;  /* call-State Tag */
    
                 index++;
                 parameter_pointer++;  /* call-State Length */

                 notify_data->cm_ect_indicator.callState = (ect_call_state_T)(*parameter_pointer); 

                 index++;
                 parameter_pointer++;  /* call-State */

                 break;
             }     

             case (0x81):
             case (0xA1): /* rdn */
             { 
                 if(*parameter_pointer == 0xA1)
                 {
                     notify_data->cm_ect_indicator.routeDestNumber.type = presentationAllowedAddress;
                 }  
                 else
                 {
                     notify_data->cm_ect_indicator.routeDestNumber.type = presentationRestrictedECT;
                 }

                 notify_data->cm_ect_indicator.routeDestNumber.present = TRUE;

                 index++; /* rdn - CHOICE TAG */
                 parameter_pointer++;

                 seq_end = *parameter_pointer + index;

                 index++;  /* rdn - CHOICE length */
                 parameter_pointer++;

                 while((index < seq_end) && (number < MAXNO_FORWARD_TO_NUMBER))   /* ect number data */
                 {
                     switch(*parameter_pointer)
                     {
                         case 0xA0:  /* PresentationAllowAddress  - [0] SEQUENCE */
                         case 0xA3:  /* PresentationRestrictedAddress - [1] SEQUENCE */
                         {
                             index++; /* PresentationAllowAddress TAG */
                             parameter_pointer++;

                             index++;  /* PresentationAllowAddress length */
                             parameter_pointer++; 
                            
                             if((*parameter_pointer == 0x80) || 
                                (*parameter_pointer == 0xA0))      /* partyNumber */
                             {

                                 index++;
                                 parameter_pointer++; /* partyNumber Tag */

                                 size = *parameter_pointer;   

                                 index++;
                                 parameter_pointer++; /* partyNumber Length */

                                 if(size > CM_CALLED_PARTY_BCD_NO_LENGTH)
                                 {
                                     MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ECT NUMBER size = %d, truncated to MAX", size);

                                     size = CM_CALLED_PARTY_BCD_NO_LENGTH;
                                 }
                                      
                                 notify_data->cm_ect_indicator.routeDestNumber.number[number].present = TRUE;

                                 notify_data->cm_ect_indicator.routeDestNumber.number[number].length = size;

                                 memscpy(notify_data->cm_ect_indicator.routeDestNumber.number[number].data,
                                        sizeof(notify_data->cm_ect_indicator.routeDestNumber.number[number].data),
                                        parameter_pointer,
                                        size);
                                 
                                 index += size;
                                 parameter_pointer += size;

                                 number += 1;
                             }
                             else if((*parameter_pointer == 0x81) || 
                                     (*parameter_pointer == 0xA1))      /* partyNumberSubAddress */
                             {
                                 index++;
                                 parameter_pointer++; /* partyAddress Tag */

                                 size = *parameter_pointer;   

                                 index++;
                                 parameter_pointer++; /* partyNumber Length */
                                     
                                 if(size > CM_CALLED_PARTY_SUBADDRESS_LENGTH)
                                 {
                                    MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ECT Subaddress size = %d, truncated to MAX", size);

                                    size = CM_CALLED_PARTY_SUBADDRESS_LENGTH;
                                 }
                                      
                                 notify_data->cm_ect_indicator.routeDestNumber.address.present = TRUE;

                                 notify_data->cm_ect_indicator.routeDestNumber.address.length = size;
  
                                 memscpy(notify_data->cm_ect_indicator.routeDestNumber.address.data,
                                        sizeof(notify_data->cm_ect_indicator.routeDestNumber.address.data),
                                        parameter_pointer,
                                        size);

                                 index += size;
                                 parameter_pointer += size;
                                 

                                 number += 1;  /* forward to number */
                             }
                             else
                             {
                                 /* log a problem */
                                 MSG_MED_DS_2(MN_SUB,"=MNCNM= Recoverable error in ECT IE: %d, %d",*parameter_pointer , index);

                                 index = seq_end;
                             }

                             break;

                         } /* 0xA0 || 0xA3 */ 


                         case (0xA1):
                         case (0x81): /* rdn - presentationRestricted */
                         {
                             index++; /* presentationRestricted NULL TAG */
                             parameter_pointer++;

                             index++;  /* NULL length */
                             parameter_pointer++;

                             notify_data->cm_ect_indicator.routeDestNumber.present = TRUE;

                             notify_data->cm_ect_indicator.routeDestNumber.type = presentationRestrictedECT;
            
                             break;
                         }
             
                         case (0xA2):
                         case (0x82): /* rdn - numberNotAvailableDueToInterworking */
                         {
                             index++; /* NULL TAG */
                             parameter_pointer++;

                             index++;  /* NULL length */
                             parameter_pointer++;

                             notify_data->cm_ect_indicator.routeDestNumber.present = TRUE;

                             notify_data->cm_ect_indicator.routeDestNumber.type =numberNotAvailable;

                             break;
                         }

                         default:
                         {
                              /* log a problem */
                              MSG_MED_DS_2(MN_SUB,"=MNCNM= Recoverable error in ECT IE: %d, %d",*parameter_pointer , index);

                              index = seq_end;

                              break;
                         }

                      } /* switch presentation */

                 } /* while presentation */

                 break;

             } /* rdn */

             default:  /* ect-callState required parameters */
             {
                 MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ECT missing required callState parameter = %d", *parameter_pointer);

                 success = FALSE;

                 index = seq_length; /* to break the outer while loop */

                 break;
             }

         } /* switch ect indicator */

    } /* while (seq_length) */

    *notify_index += index;

    return success;
}


/*
 * Function name :  MN_get_name_indicator()
 * -------------
 *
 * Description :  
 * -----------    
 *   Parse the call name (CNAP) indicator information element into friendly data structure that 
 *    sent to CM/UI for User notification.
 *
 * Parameters: 
 * ----------  
 *     ie_cm_notify_ss_T - notify SS structure sent to CM.
 *     parameter_pointer - the ect indication IE.
 *     index             - index to the byte order of the IE.
 *
 * Returns:    none
 * --------
 *
 */

boolean MN_get_name_indicator(ie_cm_notify_ss_T *notify_data, byte *parameter_pointer, int *notify_index, byte *last_octet_data_pointer )
{
    boolean success = TRUE;

    int choice = 0, index = 0, seq_length, seq_end;

    if( (parameter_pointer + 3) > last_octet_data_pointer)
    {
      success = FALSE;
    }
    else
    {
      notify_data->cm_name_indicator.present = TRUE;
  
      index++; 
      parameter_pointer++;  /* name-Indicator sequence TAG */
  
      index++; 
      parameter_pointer++;  /* name-Indicator Length */
  
      choice = (int)((byte)*parameter_pointer);  /* callingName - [0] CHOICE */
  
      index++;
      parameter_pointer++;        /* callingName TAG */
  
      index++;
      parameter_pointer++;        /* callingName Length */
    }

    if( (success == TRUE) && (parameter_pointer < last_octet_data_pointer) && (choice == 0xa0)) /* reading two octets before next check*/
    {   /* IMPLICIT SEQUENCE */ 
 
        notify_data->cm_name_indicator.name.present = TRUE;/* namePresetationAllowed or 
                                                              namePresentationRestricted SEQUENCE */

        notify_data->cm_name_indicator.type = 
            ((*parameter_pointer == 0xa0) ? namePresentationAllowed :
                                            namePresentationRestricted);

        index++;
        parameter_pointer++;        /* namePresentation  TAG */

        seq_length  = *parameter_pointer;
        seq_end = seq_length + index;

        index++;                    /* namePresentation Length */                 
        parameter_pointer++;

        if( &(parameter_pointer[seq_end-1]) > last_octet_data_pointer)
        {
          success = FALSE;
        }

        while((index < seq_end) && (success == TRUE))  
        {
            switch(*parameter_pointer)
            {
                case 0x80:
                case 0xA0:
                if((parameter_pointer + 2)<= last_octet_data_pointer)
                {
                    index++;                 
                    parameter_pointer++;   /* OCTEC length specific */   

                    index++;                 
                    parameter_pointer++;   /* OCTEC data */   
                    notify_data->cm_name_indicator.name.dataCodingScheme = *parameter_pointer;
                             
                    index++;          /* next ie tag */ 
                    parameter_pointer++; 
                }
                else
                {
                  parameter_pointer += 3;
                  index = seq_end;
                  success = FALSE;
                }
                break;

                case 0x81:  /* length in character : INTEGER */
                if((parameter_pointer + 2)<= last_octet_data_pointer)
                {
                    index++;                 
                    parameter_pointer++;   /* INTEGER length specific */   

                    index++;                 
                    parameter_pointer++;   /* INTERGER data */   
                    notify_data->cm_name_indicator.name.lengthInCharacter = *parameter_pointer;
                  
                    index++;
                    parameter_pointer++;
                } 
                else
                {
                  parameter_pointer += 3;
                  index = seq_end;
                  success = FALSE;
                }

                break;

                case 0x82: /* OCTET STRING */
                case 0xA2: 
                if((parameter_pointer + *(parameter_pointer + 1) + 1)<= last_octet_data_pointer)
                {
                    byte size;
                             
                    index++;
                    parameter_pointer++;   /* STRING length specific */ 
                             
                    index++;                 
                    size = *parameter_pointer++;   /* OCTET STRING length specific */   

                    if(size > MAX_CNAP_LENGTH)
                    {
                        /* log an error and crop the number */
                        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= CNAP Name size = %d truncated to MAX", size);

                        size = MAX_CNAP_LENGTH;
                    }
                    notify_data->cm_name_indicator.name.nameStringLength = size;          
                    memscpy(notify_data->cm_name_indicator.name.nameString, sizeof(notify_data->cm_name_indicator.name.nameString) ,parameter_pointer, size);

                    parameter_pointer += size;

                    index +=size;

                }
                else
                {
                  parameter_pointer += *(parameter_pointer + 1) + 2;
                  index = seq_end;
                  success = FALSE;
                }
                break;

                default:
                {
                    /* log an error */
                    MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid CNAP Name context = %d, ", *parameter_pointer);

                    index = seq_end;

                    break;
                }

            } /* switch name sequence */

        } /* while seq_end */

    } /* SEQUENCE */
    else if( (success == TRUE) && ( (parameter_pointer + 3) <= last_octet_data_pointer ) && (choice == 0x80)) /* read four octets in the block */
    {   
        index++;
        parameter_pointer++; /* CHOICE Tag */

        index++;
        parameter_pointer++; /* CHOICE Length */

        /* NULL = no length */
        notify_data->cm_name_indicator.type = ((*parameter_pointer == 0x81) ? presentationRestricted :
                                                                           nameUnavailable);

        index++;
        parameter_pointer++; /* NULL Tag length */

        if(*parameter_pointer != 0x00)
        {
            success = FALSE;
        }
       
        index++;
        parameter_pointer++; /* NULL length  */
    }
    else
    {
        /* log a problem */
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Invalid CNAP type ");

        notify_data->cm_name_indicator.type = nameUnavailable;

        success = FALSE;
    }

    *notify_index += index;

    return success;
}

#ifndef FEATURE_CCBS
/*
 * Function name :  MN_get_ccbs_indicator()
 * -------------
 *
 * Description :  
 * -----------    
 *   Parse the CCBS feature indicator information element into friendly data structure that 
 *    sent to CM/UI for User notification.
 *
 * Parameters: 
 * ----------  
 *     ie_cm_notify_ss_T - notify SS structure sent to CM.
 *     parameter_pointer - the ect indication IE.
 *     index             - index to the byte order of the IE.
 *
 * Returns:    none
 * --------
 *
 */
boolean MN_get_ccbs_indicator(ie_cm_notify_ss_T *notify_data, byte *parameter_pointer, int *notify_index, byte *last_octet_data_pointer )
{
    boolean success = TRUE;
    int seq_end, number = 0, index = 0, choice;
    byte size;

    index++;
    parameter_pointer++; /* sequence length */
    seq_end = *parameter_pointer + index;

    if(&(parameter_pointer[seq_end-1]) > last_octet_data_pointer)
    {
      success = FALSE;
    }

    notify_data->cm_ccbs_indicator.present = TRUE;  /* set the ccbs FEATURE */

    while((index < seq_end) && (number < MAXNO_FORWARD_TO_NUMBER) && (success == TRUE))
    {
        index++;
        parameter_pointer++;
        choice = (int)((byte)*parameter_pointer);
 
        switch (choice)
        {
            case 0x80:  /* ccbs-index */
            {
                index++;
                parameter_pointer++;  /* INTEGER LENGTH */

                notify_data->cm_ccbs_indicator.ccbsIndex.present = TRUE;
                notify_data->cm_ccbs_indicator.ccbsIndex.ccbs_Index = *parameter_pointer;

                break;
            }

            case 0x81:  /* b-subscrfiber number */
            case 0xA1:  /* OCTETSTRING */
            {
                index++;
                size = *parameter_pointer++;  /* INTEGER LENGTH */
                         
                if (size > CM_CALLED_PARTY_BCD_NO_LENGTH)
                {
                    MSG_ERROR_DS_1(MN_SUB,"=MNCNM= CCBS BCD Number is too long = %d", size);

                    size = CM_CALLED_PARTY_BCD_NO_LENGTH;
                }

                memscpy(notify_data->cm_ccbs_indicator.subscriberNumber[number].data,
                         sizeof(notify_data->cm_ccbs_indicator.subscriberNumber[number].data),
                         parameter_pointer,
                         size);

                notify_data->cm_ccbs_indicator.subscriberNumber[number].length = size;

                notify_data->cm_ccbs_indicator.subscriberNumber[number].present = TRUE;
 
                parameter_pointer += size;

                index += size;

                number++;

                break;
            }
            case 0x82:  /* b-subscriber subaddress */
            case 0xA2:
            {
                index++;                 
                size = *parameter_pointer++;   /* OCTET length specific */   
                                     
                if(size > CM_CALLED_PARTY_SUBADDRESS_LENGTH)
                {
                    MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ECT Subaddress size = %d, truncated to MAX", size);

                    size = CM_CALLED_PARTY_SUBADDRESS_LENGTH;
                }
                                      
                memscpy(notify_data->cm_ccbs_indicator.subscriberAddress.data,
                       sizeof(notify_data->cm_ccbs_indicator.subscriberAddress.data),
                       parameter_pointer,
                                size);

                notify_data->cm_ccbs_indicator.subscriberAddress.present = TRUE;

                notify_data->cm_ccbs_indicator.subscriberAddress.length = size;

                parameter_pointer += size;

                index += size;

                break;
            }

            case 0x83:  /* basic service group */
            {
                index++;                 
                size = *parameter_pointer++;   /* OCTET length specific */   

                notify_data->cm_ccbs_indicator.basicService.bs_code = *parameter_pointer;
                          
                notify_data->cm_ccbs_indicator.basicService.present = TRUE;

                break;
            }

            default:
            {

                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= CCBS received unknown context type = %d", *parameter_pointer);

                success = FALSE;

                index = seq_end;
   
                number = MAXNO_FORWARD_TO_NUMBER;

                break;
            }

        } /* choice */

    } /* while (seq_end) */

    *notify_index += index;

    return success;
}
#endif /* #ifndef FEATURE_CCBS */


/*
 * Function name :  MN_release_invoke_ids
 * -------------
 *
 * Description :  Releases all invoke ids  for a MO/MT operation
 * -----------
 *
 * Parameters: connection id
 * ----------  
 *
 *
 * Returns:    none
 * --------
 *
 */

VOID_FUNC MN_release_invoke_ids(connection_id_T connection_id)
{
   int      i;

   /* Clears the MO and MT invoke ids, which exist specially in USSD session */

   MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Clearing the invoke ids due to dupliacate invoke id detection");

   for( i = 0; (i<MAXNO_INVOKE_IDS); i++)
   {

       if (ss_invoke_data[i].connection_id == connection_id)
       {

           ss_invoke_data[i].ss_stored_operation = NULL_SS_OPERATION;
           ss_invoke_data[i].connection_id = UNKNOWN_CONNECTION_ID;
       }

       if ((ss_invoke_data[i].mt_invoke_id_valid == TRUE)&&
          (ss_invoke_data[i].mt_connection_id == connection_id))
       {

           ss_invoke_data[i].mt_invoke_id_valid = FALSE;
           ss_invoke_data[i].ss_stored_operation = NULL_SS_OPERATION;
           ss_invoke_data[i].mt_connection_id=UNKNOWN_CONNECTION_ID;            
       }      
      
   }

}

/*
 * Function name :  MN_release_mo_invoke_id
 * -------------
 *
 * Description :  Releases invoke data for a MO operation
 * -----------
 *
 * Parameters: invoke id
 *             ss operation
 *-----------
 *
 * Returns:    success
 *             connection id
 * --------
 *
 */


boolean MN_release_mo_invoke_id( byte            invoke_id,
                                 byte            ss_operation,
                                 connection_id_T *connection_id_ptr,
                                 boolean         release)
{
   boolean success;
   int      i;

   /* These transaactions already have a transaction ID from the UE */

   /* Find if the invoke id has been stored, if so, release and return the 
            corresponding connection id and set success true */


   for( success = FALSE,i = 0;
        ((i<MAXNO_INVOKE_IDS)&&(success==FALSE));
        i++)

   {
      if ((ss_invoke_data[i].connection_id != UNKNOWN_CONNECTION_ID) &&
           (ss_invoke_data[i].cm_ss_ref == invoke_id) &&
           (ss_invoke_data[i].ss_stored_operation == ss_operation))
      {
         success = TRUE;

         *connection_id_ptr = ss_invoke_data[i].connection_id;

         if ( release == TRUE)
         {
            ss_invoke_data[i].ss_stored_operation = NULL_SS_OPERATION;
            ss_invoke_data[i].connection_id=UNKNOWN_CONNECTION_ID;   
         }
      }
   }

   return(success);
}
#if defined(FEATURE_DUAL_SIM )
boolean MN_validate_ss_as_id(sys_modem_as_id_e_type as_id, byte cm_ss_ref, byte ss_operation, boolean send_rsp)
{
  ie_cm_ss_error_T          cm_ss_error;
  boolean sucess= FALSE;
  memset(&(cm_ss_error),'\0',sizeof(ie_cm_ss_error_T));

  if(as_id != SYS_MODEM_AS_ID_NONE && (mn_as_id == SYS_MODEM_AS_ID_NONE || as_id == mn_as_id))
  {
    sucess = TRUE;
  }
  else
  {
    sucess = FALSE;
    MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Received Invalid as_id=%d, mn_as_id=%d\n", as_id,mn_as_id); 
    if(send_rsp) 
    {
   
      cm_ss_error.present = TRUE;
      cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
      cm_ss_error.error_code = AS_REJ_ABORT_RADIO_UNAVAILABLE;

      MN_select_ss_error(cm_ss_ref, 
                         ss_operation, 
#if defined(FEATURE_DUAL_SIM )
                         as_id,
#endif
                         cm_ss_error);
    }
  }
  return sucess;
}
#endif
