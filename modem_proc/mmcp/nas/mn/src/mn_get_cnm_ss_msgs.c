/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_get_cnm_ss_msgs.c_v   1.9   11 Jul 2002 17:30:02   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_get_cnm_ss_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/4/01      CD     Fixed several compile warnings C4244 conversion from int to byte
                   Fixed several compile warnings C4701 local variable may be
                   used without being initialized 
                   stored_operation(=NULL_SS_OPERATION),
                   mmi_ss_ref(=0xff)
                   mmi_cause (set entire structure to 0s)

5/10/01     CD     Removed unnecessary header file includes

5/21/01     CD     Removed #include for cause.h and ms.h since they are
                   not needed

5/22/01     CD     Corrected use of 'data_pointer' in MN_get_MNSS_BEGIN_IND,
                   MN_get_MNSS_END_IND and MN_get_MNSS_FACILITY_IND to avoid 
                   lint error 415 and 416

                   Changed declaration of message_length,message_index,facility_length
                   in MN_get_MNSS_BEGIN_IND from int to unsigned int to avoid 
                   lint errors 574 and 737

                   Corrected use of 'invoke_id' index in MN_get_MNSS_END_IND to
                   avoid possible access of out-of-bounds array -- lint 661

7/18/01     CD     Removed duplicate external declaration of ss_invoke_data

8/10/01     CD     Removed unnecessary queue.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

10/07/01    AB     Removed un-referenced header file, customer.h, and changed SS
                     primitives handler prototypes per Lint.

01/25/02    CD     Updated copyright
                   Replaced calls to ERR() by MSG_ERR()
                   Replaced IEI by boolean flag
                   Added customer.h

07/10/02    AB     In MN_get_MNSS_END_IND(), ignore the processing
                   of CAUSE IE when the FACILITY IE is also included 
                   in the RELEASE_COMPLETED message.

10/02/02    AB     In MN_get_MNSS_FACILITY_IND(), sent an ERROR to CM
                   if the received a poorly structured RETURN_RESULT 
                   components from the network.

01/28/03    AB     In MN_get_MNSS_END_IND(), initialized the invoke
                   data buffer, ss_invoke_data[], to removed unwanted 
                   data for subsequence SS operation.  

05/12/03    AB     In MN_get_MNSS_END_IND(), ensure the the SS 
                     call independent guard timer not stop
                     when there is a format error of a valid
                     components.

07/25/03    AB     Fixed CR33317, Handling of infinite length in Interrogate
                     SS operation.

08/20/03    AB     Fixed CR26517, handling of invalid or corrupted SS 
                     component.

09/10/03    AB     Removed lint errors.

06/02/04    AB     Fixed the memory leak in Error handling for unsupported
                     parallel operations for same invoke id.

01/20/05    AB     Fixed MNSS_REJ_IND handling of open MT invoke ID.

05/03/05    HS     Modified MN_get_element_length() and added MN_get_element_length2()
                   to correctly calulate parameter length with indefinite length

05/08/05    AB     Added non-standard LCS abort handling in MNSS_END_IND.

06/14/05    AB     Stop the timer associated MO SS operation on MNSS_REJ_IND.

06/27/05    HS     Added MN_fix_ie_sequence_in_MNSS_END_IND() to prevent potential
                   bug in MN_get_MNSS_END_IND.
                   Modified invoke data release timing to prevent same SS indication
                   being sent to CM twice.

04/21/06    NR     Clearing the invoke id properly in  MN_get_MNSS_REJ_IND()                

08/28/06    NR     Clearing invoke id while sending RELEASE COMPLETE message
                   due to duplicate invoke id received.

09/14/06    NR     Adding changes for passing raw byte stream to CATAPPS

01/11/07    NR     Adding fix for rejecting multiple USSD with retaining transactin id
                   inspite of invoke id being cleared
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

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include <stringl/stringl.h>
#include "nasutils.h"


/*
 * Function name :  MN_get_MNSS_BEGIN_IND
 * -------------
 *
 * Description :  Start of a MT SS operation
 * -----------
 *
 *
 *
 *
 *
 *
 * Parameters: MNSS_BEGIN_IND
 * ----------  
 *
 *
 *
 * Returns:    boolean (last_invoke_id)
 * --------
 *
 */

VOID_FUNC MN_get_MNSS_BEGIN_IND(MNSS_BEGIN_IND_T *message)
{

   MN_FACILITY_DATA_T *facility_data;
   boolean            success = TRUE;
   byte               invoke_id,operation_code;
   word               facility_length;
   byte               *data_pointer;
   byte               *last_octet_data_pointer;
   ie_facility_T      facility;
   byte               *components,return_component[MAX_COMPONENTS_LENGTH];
   byte               *last_octet_component_pointer;
   ss_reject_T        ss_reject;
   ie_cm_ss_error_T   cm_ss_error;
   byte               cm_ss_ref;

#ifndef FEATURE_MODEM_HEAP
   facility_data = (MN_FACILITY_DATA_T *) gs_alloc(sizeof(MN_FACILITY_DATA_T));
#else
   facility_data = (MN_FACILITY_DATA_T *) modem_mem_calloc(1, sizeof(MN_FACILITY_DATA_T), MODEM_MEM_CLIENT_NAS);
#endif
   

   if ( facility_data != NULL )
   {

      /* Initialise facility data */

      memset(&facility, '\0', sizeof(facility));

      facility_data->connection_id = message->connection_id;

      data_pointer = message->data;
      last_octet_data_pointer = data_pointer + sizeof(ie_facility_T) - 1; /* pointer to last octet of data; pointer to first octet + size of array - 1*/

      /* Set up pointer for generating components for Return Error or Reject */

      components = &(return_component[0]);
      last_octet_component_pointer = components + sizeof(return_component) - 1;

      if (*data_pointer == IEI_FACILITY)
      {
         /* Break down the facility data */

         data_pointer++;

         facility_length = *data_pointer;

         if (facility_length > 0)
         {
            data_pointer++;

            MN_get_MNCC_FACILITY_IND( data_pointer,
                                      facility_length, 
                                      facility_data,
                                      last_octet_data_pointer);
         }
      }

     /* Check for parallel SS operations */

      operation_code = 
                  facility_data->facility_component_data[0].operational_code;

      if (( operation_code == unstructuredSS_Notify)||
          ( operation_code == unstructuredSS_Request))
      {

         success = MN_check_for_parallel_SS(message->connection_id);

         if ( success == FALSE )
         {

             /* Send a return error */
             cm_ss_error.error_code_tag = 2;
             cm_ss_error.error_code = 72 ;           /* USSD busy */
         
             MN_put_error( cm_ss_error,
                           facility_data->facility_component_data[0].invoke_id,
                           components,
                           0,
                           components);

             MN_put_facility(components, &facility, last_octet_component_pointer);

             MN_put_MNSS_END_REQ(message->connection_id,&facility);
             

             MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: SS parallel invoke_id = %d, operation = %d",
                                          facility_data->facility_component_data[0].invoke_id,
                                          operation_code); 

#ifndef FEATURE_MODEM_HEAP
             gs_free( facility_data );
#else
             modem_mem_free(facility_data, MODEM_MEM_CLIENT_NAS);
#endif

             return;
         }
      }
    
      if (facility_data->facility_component_data[0].linked_id_present == TRUE)
      {
         cm_ss_ref = ss_invoke_data[facility_data->
                             facility_component_data[0].linked_id].cm_ss_ref;
      }
      else
      {
         cm_ss_ref = 0xff;
      }

      invoke_id = facility_data->facility_component_data[0].invoke_id;

      success = MN_load_invoke_id(message->connection_id,
                                  invoke_id,
                                  facility_data->facility_component_data[0].
                                                               operational_code,
                                  cm_ss_ref
                                  );


      if (success == FALSE)
      {

         /* Reject the SS transaction */

         ss_reject.problem_code_tag = INVOKE_PROBLEM;
         ss_reject.problem_code = DUPLICATE_INVOKE_ID;

         MN_put_reject(ss_reject, invoke_id, components);

         MN_put_facility(components, &facility, last_octet_component_pointer);

         MN_put_MNSS_END_REQ(message->connection_id, &facility);
      }

      else
      {

         /* Check the number of components */

         if (facility_data->facility_component_data[1].component_type !=
                                                                   UNKNOWN)
         {
            ss_reject.problem_code_tag = GENERAL_PROBLEM;
            ss_reject.problem_code = UNRECOGNISED_COMPONENT;

            MN_put_reject(ss_reject, invoke_id, components);

            MN_put_facility(components, &facility, last_octet_component_pointer);

            MN_put_MNSS_END_REQ(message->connection_id, &facility);
         }

         else
         {
            switch (facility_data->facility_component_data[0].component_type)
            {

            case INVOKE:
               {
                  MN_handle_invoke(facility_data
#if defined(FEATURE_DUAL_SIM )
                     ,ss_transaction_information[message->connection_id].as_id
#endif
                                  );
                  break;
               }

            default:
               {
                  ss_reject.problem_code_tag = GENERAL_PROBLEM;
                  ss_reject.problem_code = UNRECOGNISED_COMPONENT;

                  MN_put_reject(ss_reject, invoke_id, components);

                  MN_put_facility(components, &facility, last_octet_component_pointer);

                  MN_put_MNSS_END_REQ(message->connection_id, &facility);
                  break;
               }
            }
         }
      }

#ifndef FEATURE_MODEM_HEAP
      gs_free( facility_data );
#else
      modem_mem_free(facility_data, MODEM_MEM_CLIENT_NAS);
#endif

   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");

   }
}


/*
 * Function name : MN_fix_ie_sequence_in_MNSS_END_IND
 * -------------
 *
 * Description :
 * -----------
 *   Fix IE sequence in MNSS_END_IND so that 'cause IE' always comes
 *   before 'facility IE'. Fixes potential bug in MN_get_MNSS_END_IND.
 *   There can only be cause IE and facility IE in MNSS_END_IND. See
 *   TS24.080 cl. 2.5
 *   
 * Parameters:
 * ----------  
 *   msg_ptr : MNSS_END_IND message pointer
 *
 * Returns
 * --------
 *  void
 */

static void MN_fix_ie_sequence_in_MNSS_END_IND (MNSS_END_IND_T *msg_ptr)
{
  byte *ptr, buf[sizeof(ie_facility_T)]={0};
  byte *last_octet_ptr;
  unsigned int msg_len, ie_len;

  GET_IMH_LEN (msg_len, msg_ptr);

  ptr = msg_ptr->data;
  last_octet_ptr = &msg_ptr->data[sizeof(msg_ptr->data) -1];

  if (*ptr == IEI_FACILITY)
  {
    ie_len = *(ptr+1); /* get facility ie len */

    if (2/*TL*/ + ie_len/*V*/ + sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type)< msg_len)
    {
      if (*(ptr+2+ie_len) == IEI_CAUSE) /* next ie is cause */
      {
        memscpy (buf, sizeof(buf) ,ptr, ie_len+2);
        memscpy (ptr, (last_octet_ptr - ptr + 1), ptr+2+ie_len, *(ptr+2+ie_len+1)+2); /* move cause ie to front */
        /* dest_size = end_octet_addr - start_addr + 1*/
        memscpy (ptr+*(ptr+1)+2, (last_octet_ptr -ptr -*ptr -1) ,buf, ie_len+2); /* put facility ie next */
      }
    }
  }
}



/*
 * Function name :  MN_get_MNSS_END_IND
 * -------------
 *
 * Description :  End of a Call Independent SS operation
 * -----------
 *
 *
 *
 *
 *
 *
 * Parameters: MNSS_END_IND
 * ----------  
 *
 *
 *
 * Returns
 * --------
 *
 */

VOID_FUNC MN_get_MNSS_END_IND(MNSS_END_IND_T *message)
{
   MN_FACILITY_DATA_T *facility_data;
   MNSS_REJ_IND_T     mnss_rej_ind;
   boolean            success = FALSE;
   byte               invoke_ptr,invoke_id;
   byte               cm_ss_ref = 0xff;
   byte               stored_operation = NULL_SS_OPERATION;
   word               message_length,facility_length;
   byte               *data_pointer;
   byte               *last_octet_data_pointer;
   ie_cm_cc_cause_T   cm_cc_cause;
   facility_component_data_T  *component_data;
   word index = 0;
   suppsvc_l3_data_length = 0;

   MN_fix_ie_sequence_in_MNSS_END_IND (message);

   cm_cc_cause.present = FALSE;

   message_length = message->message_header.message_len_lsb +
                      (message->message_header.message_len_msb * 256);

#ifndef FEATURE_MODEM_HEAP
   facility_data = (MN_FACILITY_DATA_T *) gs_alloc(sizeof(MN_FACILITY_DATA_T));
#else
   facility_data = (MN_FACILITY_DATA_T *) modem_mem_calloc(1, sizeof(MN_FACILITY_DATA_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( facility_data != NULL )
   {
      data_pointer = message->data;
      last_octet_data_pointer = data_pointer + sizeof(message->data) - 1;

      facility_data->connection_id = message->connection_id;

      component_data = &(facility_data->facility_component_data[0]);

      /* Set up pointer for generating components for Return Error or Reject */

      if (message_length > (sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type)))
      {
         if ((data_pointer <= last_octet_data_pointer) && (*data_pointer == IEI_CAUSE))
         {
            index = (word)MN_get_cause_data(data_pointer, &cm_cc_cause, last_octet_data_pointer);

            /* 
             * Find the invoke id corresponding to the given connection id 
             */
            for (invoke_id = 0, success = FALSE; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
            {
               if (ss_invoke_data[invoke_id].connection_id == message->connection_id)
               {
                  stored_operation = MN_retrieve_ss_operation(invoke_id, FALSE);

                  if( stored_operation != NULL_SS_OPERATION )
                  {
                      success = TRUE;
                      break;
                  }
               }
            }

            if ( success == TRUE )
            {
               if (invoke_id < MAXNO_INVOKE_IDS)
               {
                  cm_ss_ref = ss_invoke_data[invoke_id].cm_ss_ref;
                  
                  /* Advance to FACILITY IE */
                  data_pointer += *(data_pointer+1) + 2;  /* 2 = IEI and Length field */

                  if((data_pointer <= last_octet_data_pointer) && (*data_pointer != IEI_FACILITY))
                  {
                     /* stop related timer */
                     MN_stop_ss_timer(invoke_id);

                     /* Sent confimration to UI */
                     MN_select_cc_cause ( cm_ss_ref, 
                                          stored_operation, 
#if defined(FEATURE_DUAL_SIM )
                                          message->as_id,
#endif
                                          cm_cc_cause);
                     
                     (void)MN_retrieve_ss_operation(invoke_id, TRUE); /* clear invoke data */
                  }
                  else if(data_pointer > last_octet_data_pointer)
                  {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error!!! 2 Buffer Overflown...");
                  }
                  else
                  {
                     /* FACILITY IE info override the cause */
                     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= FACILITY info overide cause value %d", cm_ss_ref); 
                  }
               }
               else
               {
               
                  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= invoke id exceeds maximum value permitted %d", invoke_id); 
               }
            }
            else
            {
                MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Unknown TI/Invoke ID = %d, %d", message->connection_id, invoke_id);
            }
 
         } /* CAUSE_IEI */
         else if(data_pointer > last_octet_data_pointer)
         {
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error!!! 1 Buffer Overflown...");
         }

         success = FALSE;  /* reset success flag */

         if ((data_pointer < last_octet_data_pointer) && (*data_pointer == IEI_FACILITY))
         {
            data_pointer++;

            facility_length = *data_pointer;

            if (facility_length > 0)
            {
               data_pointer++;

               MN_get_MNCC_FACILITY_IND(data_pointer,
                                        facility_length,
                                        facility_data,
                                        last_octet_data_pointer);
               suppsvc_l3_data_length = facility_data->facility_component_data[0].parameter_length;
               memscpy(&suppsvc_l3_data[0], sizeof(suppsvc_l3_data) ,facility_data->facility_component_data[0].parameters, suppsvc_l3_data_length);
               invoke_ptr = facility_data->facility_component_data[0].invoke_id;

               component_data = &(facility_data->facility_component_data[0]);

               if (component_data->component_type == INVOKE)
               {
                  if (facility_data->facility_component_data[0].linked_id_present == TRUE)
                  {
                     cm_ss_ref = ss_invoke_data[facility_data->
                             facility_component_data[0].linked_id].cm_ss_ref;
                  }
                  else
                  {
                     cm_ss_ref = 0xff;
                  }

                  /* Allocate and stores the MT invoke data */
                  success = MN_load_invoke_id(message->connection_id,
                                              component_data->invoke_id,
                                              component_data->operational_code,
                                              cm_ss_ref
                                              );

               }
               else if( (component_data->component_type == REJECT) ||
                        (component_data->component_type == RETURN_RESULT) ||
                        (component_data->component_type == RETURN_ERROR) )
               {
                  if ( (invoke_ptr < MAXNO_INVOKE_IDS) &&
                       (facility_data->facility_component_data[0].invoke_id_present == TRUE))
                  {
                     stored_operation = 
                                 MN_retrieve_ss_operation(invoke_ptr, FALSE);

                     if ( (stored_operation != NULL_SS_OPERATION) &&
                          (((component_data->component_type == RETURN_RESULT) &&
                           ((stored_operation == component_data->operational_code)||
                           (component_data->operational_code_present == FALSE))) ||
                           (component_data->component_type != RETURN_RESULT)) )
                     {
                        cm_ss_ref = ss_invoke_data[invoke_ptr].cm_ss_ref;

                        success = TRUE; 
                     }
                     else
                     {
                        success = FALSE;
                     }

                  } /* invoke id present */
                  /*  
                   * message is a reject message where the invoke_id has 
                   * become lost 
                   */
                  else if ((component_data->invoke_id_present == FALSE)&&
                           (component_data->component_type == REJECT))
                  {
                     /* 
                      * Find the invoke id corresponding to the given 
                      * connection id 
                      */
                     for ( invoke_id = 0; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
                     {
                        if (ss_invoke_data[invoke_id].connection_id == message->connection_id)
                        {
                           stored_operation = MN_retrieve_ss_operation(invoke_id, TRUE);

                           if(stored_operation != NULL_SS_OPERATION)
                           {
                               MN_stop_ss_timer(invoke_id);

                               cm_ss_ref = ss_invoke_data[invoke_id].cm_ss_ref;

                               MN_handle_error(facility_data, 
                                               cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                               message->as_id,
#endif
                                               stored_operation);
                             
                               success = FALSE;  /* stop further processing */

                               break;
                           }

                        } /* connection ID */

                     } /* invoke ID */

                  } /* REJECT */
                  else
                  {
                      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Invalid REJECT component = %d", component_data->component_type);
                  }

               } /* component type */
               else
               {
                  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Invalid component type = %d", component_data->component_type);
               }
               
               if (success == TRUE)
               {

                  switch(facility_data->facility_component_data[0].component_type)
                  {
                     case INVOKE:
                     {
                        MN_handle_invoke(facility_data
#if defined(FEATURE_DUAL_SIM )
                       ,message->as_id
#endif
                        );

                        break;
                     }

                     case RETURN_RESULT:
                     {
                        MN_stop_ss_timer(invoke_ptr);

                        success = MN_handle_result(facility_data, 
                                                   cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                                   message->as_id,
#endif
                                                   stored_operation);
                        break;
                     }

                     case RETURN_ERROR:
                     case REJECT:
                     {
                        MN_stop_ss_timer(invoke_ptr);

                        MN_handle_error(facility_data,
                                        cm_ss_ref,
#if defined(FEATURE_DUAL_SIM )
                                        message->as_id,
#endif
                                        stored_operation);
                        break;
                     }

                     default:
                     {
                        MSG_ERROR_DS_3(MN_SUB,"=MNCNM= ERROR: Unknown facility component",
                                          facility_data,
                                          facility_data->facility_component_data[0].component_type,
                                          index); 

                        success = FALSE;

                        break;
                     }

                  } /* switch component type */

               } /* success invoke ID & operation */
               else
               {
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Invalid component IE = %d");

                   /*
                    * Trigger RLF scenario if either invoke_id or operation_code does not 
                    * match or if success is FALSE.
                    * MN should send indication to CM if NAS has received RELEASE COMPLETE
                    * message OTA.
                    */

                   mnss_rej_ind.connection_id = message->connection_id;
                   mnss_rej_ind.mm_cause = ((cm_cc_cause.present == TRUE) ? cm_cc_cause.cause_value : ABORT_MSG_RECEIVED);   
                   mnss_rej_ind.lower_layer_cause = NULL;
#if defined(FEATURE_DUAL_SIM )
                   if(MNCNM_IS_SINGLE_STACK)
                   {
                     mnss_rej_ind.as_id = message->as_id;
                   }
#endif
                   MN_get_MNSS_REJ_IND(&mnss_rej_ind);
 
                   success = TRUE;  /* MT USSD transactions can be process */

               }

               (void)MN_retrieve_ss_operation(invoke_ptr, TRUE); /* clear invoke data */

            } /* facility length */
            else
            { 
                 MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Invalid facility len = %d", facility_length);
            }

         } /* FACILITY IEI */
         else
         {

           if(data_pointer >= last_octet_data_pointer)
           {
             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Error!!! 3 Buffer Overflown...");
           }

             /* MNSS_END_IND w/o any facility IE = LCS ABORT indication */
             mnss_rej_ind.connection_id = message->connection_id;
             mnss_rej_ind.mm_cause = ((cm_cc_cause.present == TRUE) ? cm_cc_cause.cause_value : ABORT_MSG_RECEIVED);   
             mnss_rej_ind.lower_layer_cause = NULL;
#if defined(FEATURE_DUAL_SIM )
             if(MNCNM_IS_SINGLE_STACK)
             {
               mnss_rej_ind.as_id = message->as_id;
             }
#endif
             MN_get_MNSS_REJ_IND(&mnss_rej_ind);
 
             success = TRUE;  /* MT USSD transactions can be process */
         }

      } /* message_length */
      else
      {
         /* MNSS_END_IND w/o any facility IE = LCS ABORT indication */
         mnss_rej_ind.connection_id = message->connection_id;
         mnss_rej_ind.mm_cause = ABORT_MSG_RECEIVED;   
         mnss_rej_ind.lower_layer_cause = NULL;
#if defined(FEATURE_DUAL_SIM )
         if(MNCNM_IS_SINGLE_STACK)
         {
           mnss_rej_ind.as_id = message->as_id;
         }
#endif

         MN_get_MNSS_REJ_IND(&mnss_rej_ind);

         success = TRUE;  /* MT USSD can be process */
      }

      if (success != FALSE) 
      {
        /* special processing for USSD Data and Requests */
        if ( stored_operation == unstructuredSS_Request        ||
             stored_operation == processUnstructuredSS_Request ||
             stored_operation == processUnstructuredSS_Data )
        {
          MN_put_cm_release_unstructured_ss_ind(
#if defined(FEATURE_DUAL_SIM )
                    message->as_id
#endif
            );
        }
      } 
      else
      {
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= SS END indication handling returned failure ");
          /* SS END indication did was not handle properly */
          /* Partially Cleanup invoke data for Timer recovery 
          for (invoke_id = 0; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
          {
              if ((ss_invoke_data[invoke_id].connection_id == message->connection_id) &&
                  (component_data->component_type != INVOKE))
              {

                  memset((ss_invoke_data_T *)&ss_invoke_data[invoke_id].cm_ss_ref, 
                         NULL, 
                         sizeof(ss_invoke_data_T) - (sizeof(byte) + sizeof(connection_id_T)));

              }
          }
          */
      }

      /* Release any MT operations */
      (void)MN_release_ci_invoke_id_end(message->connection_id);

#ifndef FEATURE_MODEM_HEAP
      gs_free( facility_data );
#else
      modem_mem_free(facility_data, MODEM_MEM_CLIENT_NAS);
#endif
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
     
   }
   
   /* If CM_RELEASE_UNSTRUCTURED_SS_IND is not already sent, then send it now */
   if(waiting_for_next_network_ussd_trigger == TRUE)
   {
      MN_put_cm_release_unstructured_ss_ind(
#if defined(FEATURE_DUAL_SIM )
                       message->as_id
#endif
                       );

   }

}



/*
 * Function name :  MN_get_MNSS_FACILITY_IND
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
 * Parameters: MNSS_FACILITY_IND
 * ----------  
 *
 *
 *
 * Returns
 * --------
 *
 */

VOID_FUNC MN_get_MNSS_FACILITY_IND(MNSS_FACILITY_IND_T *message)
{
   MN_FACILITY_DATA_T         *facility_data;
   boolean                    success;
   byte                       invoke_ptr = 0;
   byte                       cm_ss_ref = 00;
   byte                       stored_operation = NULL_SS_OPERATION;
   word                       message_length;
   word                       message_index;
   word                       facility_length;
   byte                       *data_pointer;
   byte                       *last_octet_data_pointer;
   ss_reject_T                ss_reject;
   byte                       *components;
   byte                       *last_octet_component_pointer;
   byte                       return_component[MAX_COMPONENTS_LENGTH];
   ie_facility_T              facility;
   facility_component_data_T  *component_data;
   ie_cm_ss_error_T           cm_ss_error;


   message_length = (word)(message->message_header.message_len_lsb +
       (message->message_header.message_len_msb * 256));

   data_pointer = message->data;
   last_octet_data_pointer = data_pointer + sizeof(ie_facility_T) - 1;

   message_index = 0;

#ifndef FEATURE_MODEM_HEAP
   facility_data = (MN_FACILITY_DATA_T *) gs_alloc(sizeof(MN_FACILITY_DATA_T));
#else
   facility_data = (MN_FACILITY_DATA_T *) modem_mem_calloc(1, sizeof(MN_FACILITY_DATA_T), MODEM_MEM_CLIENT_NAS);
#endif

   if (facility_data != NULL )
   {

      facility_data->connection_id = message->connection_id;

      message_index++;
      message_index = message_index + sizeof(sys_modem_as_id_e_type);


      /* 
       * Set the pointer for generating components for Return Error or Reject 
       * Messages 
       */

      components = &(return_component[0]);
      last_octet_component_pointer = components + sizeof(return_component) -1;

      if (*data_pointer == IEI_FACILITY)
      {
         data_pointer++;
         message_index++;

         facility_length = *data_pointer;

         if (facility_length > 0)
         {
            message_index++;
            data_pointer++;


            MN_get_MNCC_FACILITY_IND(data_pointer,
                                     (word)(message_length - message_index),
                                     facility_data,
                                     last_octet_data_pointer);
         }
      }

      invoke_ptr = facility_data->facility_component_data[0].invoke_id;

      success = TRUE;

      component_data = &(facility_data->facility_component_data[0]);

      if (component_data->component_type == INVOKE)
      {
         if (facility_data->facility_component_data[0].linked_id_present == 
                                                                        TRUE)
         {
              cm_ss_ref = ss_invoke_data[facility_data->
                             facility_component_data[0].linked_id].cm_ss_ref;
         }
         else
         {
            cm_ss_ref = 0xff;
         }

         success = MN_load_invoke_id(message->connection_id,
                                    component_data->invoke_id,
                                    component_data->operational_code,
                                    cm_ss_ref
                                     );

         if (success == FALSE)
         {
            ss_reject.problem_code_tag = INVOKE_PROBLEM;
            ss_reject.problem_code = DUPLICATE_INVOKE_ID;

            MN_put_reject(ss_reject, invoke_ptr, components);

            MN_put_facility(components, &facility, last_octet_component_pointer);

            MN_put_MNSS_END_REQ(message->connection_id, &facility);
            MN_release_invoke_ids(message->connection_id);
         }
      }

      if( (component_data->component_type == REJECT) ||
          (component_data->component_type == RETURN_RESULT) ||
          (component_data->component_type == RETURN_ERROR) )
      {

         if ((invoke_ptr < MAXNO_INVOKE_IDS)&&
             (component_data->invoke_id_present == TRUE))
         {
            stored_operation = MN_retrieve_ss_operation(invoke_ptr, FALSE);

            if ((stored_operation != NULL_SS_OPERATION)&&
               (((component_data->component_type == RETURN_RESULT)&&
               ((stored_operation == component_data->operational_code) ||
               (component_data->operational_code_present == FALSE))) ||
               (component_data->component_type != RETURN_RESULT)))
            {
               cm_ss_ref = ss_invoke_data[invoke_ptr].cm_ss_ref;

               MN_stop_ss_timer(invoke_ptr);
            }
            else
            {
               if(facility_data->facility_component_data[0].component_type ==
                                                                RETURN_RESULT)
               {
                  ss_reject.problem_code_tag = RETURN_RESULT_PROBLEM;
                  ss_reject.problem_code = MISTYPED_PARAMETER;

                  MN_put_reject(ss_reject, invoke_ptr, components);

                  MN_put_facility(components, &facility, last_octet_component_pointer);

                  MN_put_MNSS_END_REQ(message->connection_id, &facility);
               }

               if(facility_data->facility_component_data[0].component_type ==
                                                                   RETURN_ERROR)
               {
                  ss_reject.problem_code_tag = RETURN_ERROR_PROBLEM;
                  ss_reject.problem_code = RE_MISTYPED_PARAMETER;

                  MN_put_reject(ss_reject, invoke_ptr, components);

                  MN_put_facility(components, &facility, last_octet_component_pointer);

                  MN_put_MNSS_END_REQ(message->connection_id, &facility);
               }

               success = FALSE;
            }
         }

         else
         {
            if(facility_data->facility_component_data[0].component_type ==
                                                                RETURN_RESULT)
            {
               ss_reject.problem_code_tag = RETURN_RESULT_PROBLEM;
               ss_reject.problem_code = UNRECOGNISED_INVOKE_ID;

               MN_put_reject(ss_reject, invoke_ptr, components);

               MN_put_facility(components, &facility, last_octet_component_pointer);

               MN_put_MNSS_END_REQ(message->connection_id, &facility);
            }

            if(facility_data->facility_component_data[0].component_type == 
                                                               RETURN_ERROR)
            {
               ss_reject.problem_code_tag = RETURN_ERROR_PROBLEM;
               ss_reject.problem_code = UNRECOGNISED_INVOKE_ID;

               MN_put_reject(ss_reject, invoke_ptr, components);

               MN_put_facility(components, &facility, last_octet_component_pointer);

               MN_put_MNSS_END_REQ(message->connection_id, &facility);
            }

            success = FALSE;
         }

         /*  
          * message is a reject message where the invoke_id has become lost 
          */

         if ((component_data->invoke_id_present == FALSE)&&
             (component_data->component_type == REJECT))
         {
            byte invoke_id;
            /*  
             * Find the invoke id corresponding to the given connection id 
             */

            success = FALSE;

            for (invoke_id = 0; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
            {
               if (ss_invoke_data[invoke_id].connection_id == 
                                                         message->connection_id)
               {
                  stored_operation = 
                              MN_retrieve_ss_operation(invoke_id, TRUE);

                  cm_ss_ref = ss_invoke_data[invoke_id].cm_ss_ref;

                  MN_handle_error(facility_data,
                                  cm_ss_ref,
#if defined(FEATURE_DUAL_SIM )
                                  ss_transaction_information[message->connection_id].as_id,
#endif
                                  stored_operation);
                  break;

               }
            }
         }
      }

      /* Check the number of components */

      if (facility_data->facility_component_data[1].component_type != UNKNOWN)
      {
         ss_reject.problem_code_tag = GENERAL_PROBLEM;
         ss_reject.problem_code = UNRECOGNISED_COMPONENT;

         MN_put_reject(ss_reject, invoke_ptr, components);

         MN_put_facility(components, &facility, last_octet_component_pointer);

         MN_put_MNSS_END_REQ(message->connection_id, &facility);

         success = FALSE;
      }

      if (success == TRUE)
      {

         switch(component_data->component_type)
         {
            case INVOKE:
            {
               MN_handle_invoke(facility_data
#if defined(FEATURE_DUAL_SIM )
               ,ss_transaction_information[message->connection_id].as_id
#endif
               );
               break;
            }

            case RETURN_RESULT:
            {
               success = MN_handle_result(facility_data, 
                                          cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                          ss_transaction_information[message->connection_id].as_id,
#endif
                                          stored_operation);
               if (success == FALSE)
               {
                  ss_reject.problem_code_tag = GENERAL_PROBLEM;
                  ss_reject.problem_code = BADLY_STRUCTURED_COMPONENT;

                  MN_put_reject(ss_reject, invoke_ptr, components);

                  MN_put_facility(components, &facility, last_octet_component_pointer);

                  MN_put_MNSS_END_REQ(message->connection_id, &facility);

                  /* Tell cm about the abnormal condition */

                  cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;
                  cm_ss_error.error_code = MN_NETWORK_NOT_RESPONDING;
                  cm_ss_ref = invoke_ptr;

                  MN_select_ss_error(cm_ss_ref, 
                                     stored_operation, 
#if defined(FEATURE_DUAL_SIM )
                                     ss_transaction_information[message->connection_id].as_id,
#endif
                                     cm_ss_error);
               }

               break;
            }

            case RETURN_ERROR:
            case REJECT:
            {
               MN_handle_error(facility_data,
                               cm_ss_ref,
#if defined(FEATURE_DUAL_SIM )
                               ss_transaction_information[message->connection_id].as_id,
#endif
                               stored_operation);

               break;
            }

         default:
            {
               ss_reject.problem_code_tag = GENERAL_PROBLEM;
               ss_reject.problem_code = UNRECOGNISED_COMPONENT;

               MN_put_reject(ss_reject, invoke_ptr, components);

               MN_put_facility(components, &facility, last_octet_component_pointer);

               MN_put_MNSS_END_REQ(message->connection_id, &facility);
            }
         }
      }


      if(component_data->component_type != INVOKE)
      {
             /* Clear out unused ss_invoke_data entry buffer */
             if(invoke_ptr < MAXNO_INVOKE_IDS)
             {
#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
                 if(ss_invoke_data[invoke_ptr].ss_stored_operation != lcs_MOLR)
#endif
                 {
                     memset(&ss_invoke_data[invoke_ptr], NULL, sizeof(ss_invoke_data_T));
                     ss_invoke_data[invoke_ptr].connection_id = UNKNOWN_CONNECTION_ID;
                     ss_invoke_data[invoke_ptr].ss_stored_operation = NULL_SS_OPERATION;
                 }
             }
      }

#ifndef FEATURE_MODEM_HEAP
      gs_free( facility_data ); /* free heap buffer */
#else
      modem_mem_free(facility_data, MODEM_MEM_CLIENT_NAS);
#endif

   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");
   
   }
}



/*
 * Function name :  MN_get_MNSS_REJ_IND
 * -------------
 *
 * Description :  Releases invoke data
 * -----------
 *
 *
 *
 *
 *
 *
 * Parameters: MNSS_REJ_IND
 * ----------  
 *
 *
 *
 * Returns
 * --------
 *
 */

VOID_FUNC MN_get_MNSS_REJ_IND(MNSS_REJ_IND_T *message)
{
   byte invoke_id;
   byte cm_ss_ref;
   byte ss_operation;
   ie_cm_cc_cause_T cm_cause;

   memset (&cm_cause, 0, sizeof(ie_cm_cc_cause_T));

   /* The connection is down or has not been established -
         cancel the relevant invoke id's */

   for (invoke_id = 0; invoke_id < MAXNO_INVOKE_IDS; invoke_id++)
   {
       if (ss_invoke_data[invoke_id].connection_id == message->connection_id)
       {
          cm_ss_ref = ss_invoke_data[invoke_id].cm_ss_ref;

          /* MO operation */
          ss_operation = MN_retrieve_ss_operation(invoke_id, TRUE);  /* released invoke id */

          if(ss_operation != NULL_SS_OPERATION)
          {
              /* Find out the cause */
              if (message->mm_cause != 0)
              {
                 /* MM cause */
                 cm_cause.present = TRUE;
                 cm_cause.cause_value = message->mm_cause;
              }
              else if (message->lower_layer_cause != 0)
              {
                 /* OTA or lower layer cause */
                 cm_cause.present = TRUE;
                 cm_cause.cause_value = message->lower_layer_cause;
              }
              else
              {
                 cm_cause.present = TRUE;
                 cm_cause.cause_value = UNASSIGNED_CAUSE;
              }

              /* stop related timer */
              MN_stop_ss_timer(invoke_id);  /* MO operation, invoke = cm_ss_ref */

              MN_select_cc_cause(cm_ss_ref, 
                                 ss_operation, 
#if defined(FEATURE_DUAL_SIM )
                                 message->as_id,
#endif
                                 cm_cause);

          } /* MO operation */
       }

       if(ss_invoke_data[invoke_id].mt_invoke_id_valid == TRUE)
       {

          ss_operation=NULL_SS_OPERATION;
          if(ss_invoke_data[invoke_id].mt_connection_id == message->connection_id)
          {
               ss_operation=ss_invoke_data[invoke_id].mt_ss_operation; 
               ss_invoke_data[invoke_id].mt_invoke_id_valid = FALSE;
               ss_invoke_data[invoke_id].mt_ss_operation = NULL_SS_OPERATION;
               ss_invoke_data[invoke_id].mt_connection_id=UNKNOWN_CONNECTION_ID;   
          }

          if (ss_operation == unstructuredSS_Request)
          {
               MN_put_cm_release_unstructured_ss_ind(
#if defined(FEATURE_DUAL_SIM )
                                 message->as_id
#endif
                 );
          }
          
#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
          /* MT LCS notification Abort */
          if(ss_operation == lcs_LocationNotification)
          {
               location_notification_T   location_notification;

               memset(&location_notification, NULL, sizeof(location_notification_T));

               location_notification.present = TRUE;
               location_notification.notification_type = NOTIFY_LOCATION_ABORTED;

               location_notification.aborted.present = TRUE;
               location_notification.aborted.mm_cause = message->mm_cause;
               location_notification.aborted.lower_layer_cause = message->lower_layer_cause;

               MN_put_cm_lcs_location_notification_ind(invoke_id, 
#if defined(FEATURE_DUAL_SIM )
                                            message->as_id,
#endif
                                           &location_notification);
          }
          else
          {
               MSG_HIGH_DS_2(MN_SUB,"=MNCNM= MT SS invoke=%d, operation=%d is aborted", invoke_id, ss_invoke_data[invoke_id].mt_ss_operation);
          }
#endif

       }
       else if(ss_invoke_data[invoke_id].mt_invoke_id_valid == FALSE)
       {
           ss_invoke_data[invoke_id].mt_connection_id=UNKNOWN_CONNECTION_ID;
           ss_invoke_data[invoke_id].mt_ss_operation = NULL_SS_OPERATION;
       }

   } /* for invoke id < MAXNO_INVOKE_IDS */

    /* If CM_RELEASE_UNSTRUCTURED_SS_IND is not already sent, then send it now */
   if(waiting_for_next_network_ussd_trigger == TRUE)
   {
      MN_put_cm_release_unstructured_ss_ind(
#if defined(FEATURE_DUAL_SIM )
                       message->as_id
#endif
                       );

   }

}



/*
 * Function name :  MN_get_element_length
 * -------------
 *
 * Description : returns the length 0f a component 
 * -----------   according to X.209
 *               max_length is a constant for the maximum
 *               length expected in this context
 *
 *
 *
 *
 *
 *
 *
 * Input Parameters: 
 *                   byte *length_indicator   -  pointer to start of length element
 *                   int max_length              -  maximum length expected to be returned by the function.
 *                   int max_octets_to_read -  maximum number of octets which can be read from input buffer.
 *
 * Output:
 *                   return_size_T  element_size - contains increment and length.
 *
 * ----------  
 *
 *
 *
 * Returns length
 * --------
 *
 */

return_size_T MN_get_element_length(byte *length_indicator,int max_length, int max_octets_to_read)
{
   /* Locals */

   byte   no_of_bytes;   /* The number of extra bytes comprising the X.209 */
                         /* length element */
   word           length;
   word           increment;
   int              copy_max_octets_to_read = max_octets_to_read;
   return_size_T  element_size = {0,0}; /* Initialized to 0. This will be returned if max_octets_to_read agrument is zero or lesser*/

   if(max_octets_to_read <= 0)
   {
     return element_size;
   }
   
   length = 0;
   increment = 1;

   if (((*length_indicator)&0x80) != 0 )
   {
      /* fixed compile warning C4244 */
      no_of_bytes = (byte)(*length_indicator &0x7f);

      if (no_of_bytes == 0)
      {
          byte *marker = length_indicator;
          return_size_T  sub_size;
          int            ie_len; // including iei, len, val
          int            tot_len=0;

          while ((max_octets_to_read > 0) && !(*(marker+1)==0x00 && *(marker+2)==0x00))
          {
             byte *sub_len_ind = marker+2;   // point to next length indicator

             if(max_octets_to_read <= 2) /* If octets to read are lesser than what are actually read then break from this procedure */
             {
               max_octets_to_read = 0;
               break;
             }

             sub_size = MN_get_element_length2(sub_len_ind, (max_length-tot_len)-2, max_octets_to_read - 2); /* (marker + 2) is sub_len_ind */

             if (sub_size.length == (max_length-tot_len)-2)
             {
                tot_len = max_length;
                break;
             }

             ie_len = ( 1/*iei*/ + sub_size.increment/*len*/ + sub_size.length/*val*/ );

             if ( *(marker+2) == 0x80 ) // nested indefinite form
                ie_len += 2;  // skip 0x00 0x00. this one is not ours.

             tot_len += ie_len;
             marker  += ie_len;

             max_octets_to_read = max_octets_to_read - ie_len;

             if(max_octets_to_read <= 0)
             {
               max_octets_to_read = 0;
               break;
             }

             if (tot_len >= max_length)
                break;

             if (sub_size.length == 0)
             {
                tot_len = 0;
                break;
             }
          }

          length = (word)tot_len;
      }
      else
      {
         max_octets_to_read--; /* we have come here after reading an octet */

         if(no_of_bytes > max_octets_to_read)
         {
           no_of_bytes = 0;
         }

         for (;(no_of_bytes > 0);no_of_bytes--)
         {
            (length_indicator)++;
            /* fixed compile warnings C4244 */
            length = (word)((length*256) + *length_indicator);
            increment++;
         }
      }
   }
   else
   {
      length = *length_indicator;
   }

   /* limit maximum size to value passed */

   if( length > MIN(max_length, copy_max_octets_to_read)) /* length to be read inside this buffer can't exceed maximum number of octets allowed to read*/
   {
      /* fixed compile warning C4244 */
      length = (word)MIN(max_length, copy_max_octets_to_read);

      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Input ASN.1 construct too long");

   }

   element_size.length = length;
   element_size.increment = increment;
   
   return(element_size);
}



/*
 * Function name :  MN_get_element_length2
 * -------------
 *
 * Description : returns the length 0f a component 
 * -----------   according to X.209
 *               max_length is a constant for the maximum
 *               length expected in this context
 *               Also supports maximum number of recursive call
 *
 *
 *
 *
 *
 *
 * Input Parameters: 
 *                   byte *length_indicator   -  pointer to start of length element
 *                   int max_length              -  maximum length expected to be returned by the function.
 *                   int max_octets_to_read -  maximum number of octets which can be read from input buffer.
 *
 * Output:
 *                   return_size_T  element_size - contains increment and length.
 * ----------  
 *
 *
 *
 * Returns length
 * --------
 *
 */
return_size_T MN_get_element_length2(byte *length_indicator,int max_length, int max_octets_to_read)
{
   /* Locals */

   byte   no_of_bytes;   /* The number of extra bytes comprising the X.209 */
                         /* length element */
   word           length;
   word           increment;
   int              copy_max_octets_to_read = max_octets_to_read;
   return_size_T  element_size = {0,0};

   if(max_octets_to_read <= 0)
   {
     return element_size;
   }

   length = 0;
   increment = 1;

   if (((*length_indicator)&0x80) != 0 )
   {
      /* fixed compile warning C4244 */
      no_of_bytes = (byte)(*length_indicator &0x7f);

      if (no_of_bytes == 0)
      {
          byte *marker = length_indicator;

          return_size_T  sub_size;
          int            ie_len; // including iei, len, val
          int            tot_len=0;

          while ((max_octets_to_read > 0) && (!(*(marker+1)==0x00 && *(marker+2)==0x00)))
          {
             byte *sub_len_ind = marker+2;   // point to next length indicator
             if(max_octets_to_read <= 2) /* If octets to read are lesser than what are actually being read then break from this procedure */
             {
               max_octets_to_read = 0;             
               break;
             }

             sub_size = MN_get_element_length2 (sub_len_ind, (max_length-tot_len)-2, max_octets_to_read - 2);

             ie_len = ( 1/*iei*/ + sub_size.increment/*len*/ + sub_size.length/*val*/ );

             if (sub_size.length == (max_length-tot_len)-2)
             {
                tot_len = max_length;
                break;
             }

             if ( *(marker+2) == 0x80 ) // nested indefinite form
                ie_len += 2;  // skip 0x00 0x00. this one is not ours.

             tot_len += ie_len;
             marker  += ie_len;

             max_octets_to_read = max_octets_to_read - ie_len;
             if(max_octets_to_read <= 0)
             {
               max_octets_to_read = 0;             
               break;
             }


             if (tot_len >= max_length)
                break;
          }

          length = (word)tot_len;
      }
      else
      {
         if(no_of_bytes > max_octets_to_read)
         {
           no_of_bytes = 0;
         }
         for (;no_of_bytes > 0;no_of_bytes--)
         {
            (length_indicator)++;
            /* fixed compile warnings C4244 */
            length = (word)((length*256) + *length_indicator);
            increment++;
         }
      }
   }
   else
   {
      length = *length_indicator;
   }

   /* limit maximum size to value passed */

   if( length > MIN(max_length, copy_max_octets_to_read))
   {
      /* fixed compile warning C4244 */
      length = (word)MIN(max_length, copy_max_octets_to_read);

      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Input ASN.1 construct too long");

   }

   element_size.length = length;
   element_size.increment = increment;
   
   return(element_size);
}

