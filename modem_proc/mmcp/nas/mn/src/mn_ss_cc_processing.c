/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_ss_cc_processing.c_v   1.15   28 Jun 2002 13:24:40   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_ss_cc_processing.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/01   AB      Moved SS call related functions here.

10/12/01   AB      Fixed Lint errors.

11/01/01   AB      Added CNM_clear_dtmf_queue() to clear out remaining
                   DTMF difit in HOLD_ACK for MPTY calls.

11/06/01   AB      Added Checking for valid invoke_id before indexing the
                   ss_invoke_ss[] array.   

12/01/01   AB      Updated to new MN_start_ss_timer() interface changes.

12/19/01   AB      Moved MN_process_MNCC_FACILITY_IND from mn_process_cnm_cc_msgs.c.

01/22/02   AB      Removed redudant MN_put_cm_manage_calls_conf() calls for 
                   both RETURN_ERROR and REJECT components received in 
                   MN_process_MNCC_FACILITY_IND().  
                     
                   For each of the error handling function for HOLD, MPTY,
                   and RETRIEVE such as MN_execute_holdmpty_error(), etc,
                   the function to find the index to Multicall_Statu_index via
                   MN_whereami() had been replaced by MN_whichMultiCallIndex()
                   to avoid updating the MultiCall_Status_index[] table on 
                   error condition.

01/23/02   AB      ReWrited MN_execute_splitmpty_error() as error handling for
                   splitmpty() is all wrong.

01/25/02   CD      Updated Copyright
                   Replaced calls to ERR() by MSG_ERR()
                   Replaced IEI by boolean flag

02/12/02   AB      Updated the aux_hold_state for all connection ID instead
                   of only first connection in the MPTY in MN_retrieve_call().

02/18/02   AB      Fixed the setting of the charge element info that was not
                   update correctly in IEI changes in 
                   MN_execute_forward_charge_advice().  
                    
02/19/02  AB       Updated SS Call independent confirmation routine to 
                   process the new interface message that included 
                   success flag.
                    
02/28/02  AB      Moved SS aux management, MN_manage_CC_auxiliary_state(),
                  here from mn_cc_processing.c

03/06/02  AB      Fixed the double gsm_free() of the same memory block in
                  MN_execute_forward_charge_advice() which cause REX to 
                  abort the CPU.

03/12/02  AB      Added RETURN_RESULT, ERROR, and REJECT SS components 
                  handling for CD in MN_process_MNCC_FACILITY_IND().

                  Added MN_execute_callDeflection_error for CD error 
                  handling.

06/23/02  AB      Updated MN_execute_splitympty_success() per changes
                  in the MN_read_multicall_status() that affected
                  ordering of the spitmpty call session.

06/26/02  AB      Added ERROR & REJECT component for ProcessUSSDRequest
                  operation in MN_process_MNCC_FACILITY_IND().

08/12/02  AB      In MN_execute_callDeflection_error(), change the 
                  ERROR code tag to ERROR_CODE_TAG, to conform with
                  CM interface.

08/16/02  AB      In MN_execute_holdmpty_success, DTMF queue does not need
                  to be clear on MPTYHOLD ACK.

                  In MN_manage_CC_auxiliary_state(), added provision in 
                  SPLIT_REQUEST to return AUX_MPTY_CALL_HELD.

                  in MN_execute_retrievempty_error(), change the HOLD_REJ
                  event to the RETRIEVE_REJECT event in order to restore
                  the correct aux call state.
                  
08/26/02  AB      Removed the debug code in MN_execute_buildmpty_error(), 
                  that left behind when fixing the 
                  MN_execute_retrievempty_error().

09/02/02  AB      In MN_execute_callDeflection_error(), replaced calls to
                  MN_put_cm_manage_calls_conf() with 
                  MN_put_cm_manage_calls_x_conf() to provides UI with 
                  Deflected call ID and Explict call ID instead of 
                  empty list of ACTIVE call ID.

09/18/02  AB      Used the MM_put_cm_manage_calls_conf() instead of x_conf()
                  in callDeflectionr response as CM/UI can't handle the new 
                  interface.

10/12/02  AB      In MN_manage_CC_auxiliary_state(), added BUILD_MPTY_CONF 
                  to IDLE_ACTIVE state.

10/28/02  AB      In MN_execute_buildmpty_error(), restore all possible
                  MPTY request calls instead of just the one related to 
                  connection ID.

12/12/02  AB      In MN_execute_forward_charge_advise(), initilizes charge
                  buffer that are sent to CM. 

12/17/02  AB      In MN_execute_forward_charge_advise(), populate the 
                  ss_invoke_data[] using connection ID not invoke IDs.

04/16/03  AB      Updated the MN_put_cm_mo_call_conf() new interface function
                  call that supports silient redial reject IE that is
                  received from the MM.

11/16/04  AB      Start the CC timer for MN_retrieve_call().

12/22/04  AB      Fixed user data buffer alignment in MN_put_user_data(), 
                      and MN_free_user_data().

05/10/05  HS      Added support for FEATURE_CCBS

06/14/05  HS      Multicall status update to send CM correct active call list.
                  Reject event handling added for splitMPTY in AUX_CALL_IN_MPTY
                  state.

06/05/06  NR      Avoid sending MN_put_cm_manage_calls_conf in 
                  MN_execute_holdmpty_success() when MPTY call is held.

11/02/06  NR      Fixed lint errors

03/21/07  NR      Clearing the pending SS requests in case of sucess or faliures

05/25/11   AM     Putting the error range check for Multicall_Status_index

11/02/11   PM     Changes made for a feature where Voc gets released when
                  call is put on HOLD and Voc gets acquired when call is retrieved.

12/20/11  PM     When alternate procedure is in progress, don't refresh Multicall_status
                 array when a response is awaited from NW on either HOLD or REJECT request.

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
#include "mn_cnmtask.h"
#include "mn_cnmtask_int.h"
#include "cnm.h"
#include "mm.h"

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include "geran_grr_api.h"
#include <stringl/stringl.h>
#include "nasutils.h"



/*
 * Function name :  MN_retrieve_call
 * -------------
 *
 * Description :  Retreives the call indexed in the Multicall_Status array
 * -----------    This may be a single or a mpty call.
 *
 *
 *
 *
 *
 *
 * Parameters: Multicall_Status_index
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_retrieve_call(byte Multicall_Status_index)
{

   /* Locals */

   connection_id_T connection_id;
   byte     invoke_id;
   boolean  return_value = FALSE;
   byte     components[8];
   byte     ss_version[1];
   int      i;

   /*  RETRIEVE THIS HELD CALL  */

   connection_id = Multicall_Status[Multicall_Status_index].
       connection_id[0];

   if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
   {
      for(i=0; i < Multicall_Status[Multicall_Status_index].id_index; i++)
      {
          return_value = MN_manage_CC_auxiliary_state(
                           Multicall_Status[Multicall_Status_index].connection_id[i],
                           CCE_RETRIEVE_MPTY_REQ);
      }

      if(return_value == TRUE)
      {
         /*  RETRIEVE A MPTY CALL  */

         invoke_id = MN_request_invoke_id(retrieveMPTY);
         MN_start_ss_timer(connection_id, invoke_id, 10000); /* 10 sec */

         /* Build the facility component (INVOKE) */
         components[0] = 0xa1;   /* component type tag */
         components[1] = 0x06;   /* component length */
         components[2] = 0x02;   /* invoke id tag */
         components[3] = 0x01;   /* invoke id length */
         components[4] = invoke_id;
         components[5] = 0x02;   /* operational code tag */
         components[6] = 0x01;   /* operational code length */
         components[7] = retrieveMPTY;  /* operational code */

         ss_version[0] = 0x00;

         /* Send facility message to CC */
         MN_put_MNCC_FACILITY_REQ(connection_id,
             components,
             8,
             ss_version,
             1);

      }
      else
      {
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: MPTY call in wrong state for connection_id %d",connection_id);

      }
   }

   else
   {
      /*  IT IS A SINGLE CALL */

      return_value = 
          MN_manage_CC_auxiliary_state(connection_id, CCE_CALL_RETRIEVE_REQ);

      if(return_value == TRUE)
      {
         /* Send retrieve request to CC */

         MN_put_MNCC_CALL_RETRIEVE_REQ(connection_id);

         MN_start_cc_timer(connection_id, CCE_CALL_RETRIEVE_REQ);

      }
      else
      {
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Single call for connection_id %d in wrong state",connection_id); 

      }
   }

}


/*
 *   Function name : MN_put_call_on_hold
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function processes the cm_1_send message that
 *  releases all active calls 
 * MN_handle_MNCC_REL_CNF accepts the held or waiting call 
 *
 *  Parameters:
 *  -----------
 *  IN message
 *
 *  Returns:
 *
 *
 */


boolean   MN_put_call_on_hold(connection_id_T     connection_id)
{

   /* Locals */

   int                  Multicall_Status_index,index;
   boolean              success;
   boolean              s_respond_to_event = FALSE;
   byte                 components[MAX_COMPONENTS_LENGTH];
   byte                 invoke_id;
   byte                 ss_version[MAX_SS_VERSION_IND_LENGTH];
   byte                 CC_auxiliary_state = 0;   /* To check whether hold_req is sent for mpty call*/

   /* Refresh Multicall_Status */

   MN_MULTICALL_LOG(MN_read_multicall_status());

   Multicall_Status_index = (int)MN_whereami(connection_id);

   success = TRUE;
   if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
   }
   else if (Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)

   /* put a mpty call on hold */

   {

      for ( index = 0; 
            index < (Multicall_Status[Multicall_Status_index].id_index);
            index++)
      {

         /*
          * Change the state
          */
	     CC_auxiliary_state = (byte)(cc_transaction_information_p[Multicall_Status[Multicall_Status_index].
                                 connection_id[index]]->mpty_auxiliary_state +
         (4* cc_transaction_information_p[Multicall_Status[Multicall_Status_index].
                                 connection_id[index]]->hold_auxiliary_state));
		 

		 if (CC_auxiliary_state != AUX_HOLD_MPTY_REQUEST)
		 {
            s_respond_to_event = MN_manage_CC_auxiliary_state(
                                 Multicall_Status[Multicall_Status_index].
                                 connection_id[index],
                                 CCE_HOLD_MPTY_REQ);

           if ( s_respond_to_event == FALSE)
           {

              /* State change failed */

              success = FALSE;
           }
		 }
		 
	  }    

      if (CC_auxiliary_state != AUX_HOLD_MPTY_REQUEST && success == TRUE)
      {

         /* get the connection_id */

         connection_id = 
            Multicall_Status[Multicall_Status_index].connection_id[0];

         /* set up the component part of the Facility message */

         /* get an invoke_id */

         invoke_id =  MN_request_invoke_id(holdMPTY);

         /* 
          * Build the facility INVOKE component
          */

         components[0] = INVOKE;    /*0xa1*/
         components[1] = 0x06;      /* length */
         components[2] = 0x02;      /*Invoke ID tag */
         components[3] = 0x01;      /* Invoke ID length */
         components[4] = invoke_id; /* Invoke ID - for hold_MPTY */
         components[5] = 0x02;      /*Operational_code tag */
         components[6] = 0x01;      /*Operational code length */
         components[7] = holdMPTY;

         ss_version[0] = 0x00;

         /* Send facility data to CC */

         MN_put_MNCC_FACILITY_REQ(connection_id,
                                  components,
                                  8,          /* the component length */
                                  ss_version, /* the ss_version */
                                  1);         /* the ss_version_length */

         MN_start_ss_timer(connection_id,invoke_id, 10000);  /* 10 sec */

         success = TRUE;

      }
      else
      {
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: MPTY call for connection_id %d in wrong state",connection_id);
      }
   }
   else
   {

      /* It is a single call */

      connection_id = 
         Multicall_Status[Multicall_Status_index].connection_id[0];

      /*
       * Attempt to change state  
       */

      s_respond_to_event = MN_manage_CC_auxiliary_state(connection_id,
                                                        CCE_CALL_HOLD_REQ);

      if (s_respond_to_event == TRUE)
      {

         /* State changed */

         MN_put_MNCC_CALL_HOLD_REQ(connection_id);

         MN_start_cc_timer(connection_id,CCE_CALL_HOLD_REQ);

         success = TRUE;

      }
      else
      {
          MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Single call for connection_id %d in wrong state", connection_id);

      }
   }
   if(CC_auxiliary_state == AUX_HOLD_MPTY_REQUEST)
   {
      success = TRUE;
   }

   return(success);

}

/***************************************************************
*                                                              *
*                   MN_execute_buildmpty_success               *
*                                                              *
****************************************************************/
/*
 * Function name :  MN_execute_buildmpty_success
 * -------------
 *
 * Description :   Successful build multiparty - put all
 * -----------     the calls in the mpty
 *
 *
 *
 *
 *
 *
 * Parameters: Pointer to MN_FACILITY_DATA type
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_execute_buildmpty_success(MN_FACILITY_DATA_T *facility_data,
int component_number)
{
   boolean  success = TRUE;
   int i, Multicall_Status_index;

   /* SUCCESSFUL BUILD MULTIPARTY - PUT ALL CALLS IN THE MPTY  */
   Multicall_Status_index = MN_whichMultiCallIndex(facility_data->connection_id);

   if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
   {
       success = FALSE;
   }
   else
   { 

       /* clearing the SS pending tag*/
       mn_call_information_p[facility_data->connection_id]->ss_request_pending = CM_NONE_PENDING;

       for ( i = 0; i < Multicall_Status[Multicall_Status_index].id_index; i++)
       {
          /*
           * If current call is a Multiparty call that has had a
           * request to be built
           */

           success = MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].connection_id[i], 
                                                  CCE_BUILD_MPTY_CONF);
       }

       /* Held calls session */
       Multicall_Status_index = (byte)((Multicall_Status_index + 1) % 2);

       for ( i = 0; i < Multicall_Status[Multicall_Status_index].id_index; i++)
       {
          /*
           * If current call is a Multiparty call that has had a
           * request to be built
           */

           success = MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].connection_id[i], 
                                                  CCE_BUILD_MPTY_CONF);
       }
   }

   /*
    * If request failed 
    */
   if(success == FALSE)
   {
      if (facility_data->facility_component_data[component_number].component_type == RETURN_ERROR)
      {
         MN_put_crss_reject(RETURN_ERROR_PROBLEM,   /* Problem code tag */
                            UNEXPECTED_ERROR,       /* Problem code */
                            facility_data->connection_id,
                            facility_data->facility_component_data[component_number].
                             invoke_id);
      }

      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= BuildMPTY failed = %d", facility_data->connection_id);

   }

}


/***************************************************************
*                                                              *
*                   MN_execute_buildmpty_error                 *
*                                                              *
****************************************************************/
/*
 * Function name :  MN_execute_buildmpty_error
 * -------------
 *
 * Description :  Failed build multiparty - keep the 
 * -----------    calls separate
 *
 *
 *
 *
 *
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  component_number
 *        component_type
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_execute_buildmpty_error(MN_FACILITY_DATA_T *facility_data, 
int component_number,
byte component_type)
{

   ie_cm_ss_error_T cm_ss_error={0};
   boolean  success = TRUE;
   int Multicall_index;


   /* Restore the AUX state */
   if((Multicall_Status[0].call_is_mpty) || (Multicall_Status[1].call_is_mpty))
   {
       Multicall_index =  ((Multicall_Status[0].call_is_mpty) ? 1 : 0);  /* get non-MPTY session */

       success = MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_index].connection_id[0], 
                                              CCE_BUILD_MPTY_REJ);
   }
   else
   {
       /* both call session are non-MPTY */
       Multicall_index = 0;
       success = MN_manage_CC_auxiliary_state(Multicall_Status[0].connection_id[0], CCE_BUILD_MPTY_REJ);
       success = MN_manage_CC_auxiliary_state(Multicall_Status[1].connection_id[0], CCE_BUILD_MPTY_REJ);
   }

   if(success == TRUE)
   {
      /* clearing the SS pending tag only in successful case*/
      mn_call_information_p[facility_data->connection_id]->ss_request_pending = CM_NONE_PENDING;
  
      /*  TELL THE CM  */
      if(component_type == RETURN_ERROR)
      {
         /*
          * Build an error component
          */
         cm_ss_error.present = TRUE;   /* = ErrorCodeTag  */
         cm_ss_error.error_code_tag = ERROR_CODE_TAG; 
         cm_ss_error.error_code = facility_data->facility_component_data[component_number].error_code;

         MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                    mn_call_information_p[facility_data->connection_id]->as_id, 
#endif 
                                     cm_ss_error);
      }

      if(component_type == REJECT)
      {
         /* 
          * Build a problem component
          */
         cm_ss_error.present = TRUE;
         cm_ss_error.error_code_tag = facility_data->facility_component_data[component_number].problem_tag;
         cm_ss_error.error_code = facility_data->facility_component_data[component_number].problem_code;
         MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                     mn_call_information_p[facility_data->connection_id]->as_id, 
#endif 
                                     cm_ss_error);
      }

   }
   else
   {
      if (component_type == RETURN_ERROR)
      {
         MN_put_crss_reject(RETURN_ERROR_PROBLEM,   /* problem code tag */
                            UNEXPECTED_ERROR,       /* problem code */
                            facility_data->connection_id, 
                            facility_data->facility_component_data[
                            component_number].invoke_id);
      }

      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Build MPTY error incorrectly returned");
   }

}



/*
 * Function name :  MN_execute_forward_charge_advice
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
VOID_FUNC MN_execute_forward_charge_advice (
            MN_FACILITY_DATA_T   *facility_data_ptr, 
            int                  component_number)
{
   boolean                             success;
   int                                 index;
   ie_cm_forward_charge_advice_req_T  *cm_fca_data;
   charge_advice_data_T               *advice;
   byte                                increment;
   byte                                *position;
   byte                                ss_code;
   byte                                length;
   byte                                invoke_id, connection_id;

   /*  DECODE THE PARAMETERS AND SEND DATA ON TO CM  */

   success = TRUE;
   index = 0;
   cm_fca_data = NULL;
   advice = NULL;

   /* 
   ** Load the data in to the global store ss_invoke_data 
   */
   connection_id = facility_data_ptr->connection_id;

   if(connection_id < MAXNO_SS_TRANSACTIONS)
   {
       invoke_id = facility_data_ptr->facility_component_data[component_number].invoke_id;

       /*
        * If this element is not a sequence tag 
        */

       if((facility_data_ptr->facility_component_data[component_number].parameters[index] != SEQUENCE_TAG) &&
          (facility_data_ptr->facility_component_data[component_number].parameters[index] != SEQUENCE_TAG))
       {
          success = FALSE;
       }

       index += 2;

       if(facility_data_ptr->facility_component_data[component_number].parameters[index] != 0x80)      /* Context specific Primitive [0] */
       {
          success = FALSE;
       }

       index += 2;

       /*
    * If this element is NOT Advice Of Charge
    */

       ss_code = facility_data_ptr->facility_component_data[component_number].
         parameters[index];

       if((ss_code != (byte)aocc) && (ss_code != (byte)aoci)) /* AOCC or AOCI identifiers */
       {
          success = FALSE;
       }
       index++;

       /*
        * If this is not a Charging Info Id 
        */

       if(facility_data_ptr->facility_component_data[component_number].
           parameters[index] != 0xa1) /* Context specific Construct [1] */
           /* Charging info identifier */
       {
         success = FALSE;
       }

       /* Get some memory for the output message */

       if ( success == TRUE )  
       {
#ifndef FEATURE_MODEM_HEAP
         cm_fca_data = (ie_cm_forward_charge_advice_req_T *)gs_alloc(sizeof(ie_cm_forward_charge_advice_req_T));
#else
         cm_fca_data = (ie_cm_forward_charge_advice_req_T *)modem_mem_calloc(1, sizeof(ie_cm_forward_charge_advice_req_T), MODEM_MEM_CLIENT_NAS);
#endif

         if(cm_fca_data != NULL)
         {
           memset(cm_fca_data, NULL, sizeof(ie_cm_forward_charge_advice_req_T));  /* initialize buffer */
           
           success = MN_load_invoke_id(facility_data_ptr->connection_id,
                                       invoke_id,
                                       ss_code,
                                       0xFF);
         }
         else 
         {
           success = FALSE;
         }
       }

       index += 2;
   }
   else
   {
       success = FALSE;
   }

   if(success == TRUE)
   {
      /** 
      *** GET THE ADVICE OF CHARGE INFORMATION
      **/

      length = facility_data_ptr->
                  facility_component_data[component_number].parameter_length;

      while(index < length)
      {
         switch(facility_data_ptr->facility_component_data[component_number].parameters[index])
         {

           /*
            * Load the data in to CM friendly structures 
            */

         case 0x81:  /* e1 - Context specific Primitive [1] */

            {
               position = &(facility_data_ptr->facility_component_data[component_number].parameters[index]);

               if(cm_fca_data != NULL)
               {                       
                   advice = &(cm_fca_data->e1);

                   advice->present = TRUE;
                  
                   increment = MN_put_charge_advice(position,advice);
                   
                   index+= increment;
               }
 
               break;
            }

         case 0x82:  /* e2 */
            {
               position = &( facility_data_ptr->facility_component_data[component_number].parameters[index]);

               if(cm_fca_data != NULL) 
               {

                   advice = &( cm_fca_data->e2);
                   advice->present = TRUE;
                   increment = MN_put_charge_advice(position,advice);
                   index+= increment;
               }

               break;
            }

         case 0x83:  /* e3 */
            {
               position = &( facility_data_ptr->facility_component_data[component_number].parameters[index]);

               if(cm_fca_data != NULL)
               {
                   advice = &( cm_fca_data->e3);
                   advice->present = TRUE;
                   increment = MN_put_charge_advice(position,advice);
                   index+= increment;
               }

               break;
            }

         case 0x84:  /* e4 */
            {
               position = &( facility_data_ptr->facility_component_data[component_number].parameters[index]);

               if(cm_fca_data != NULL)
               {
                   advice = &( cm_fca_data->e4);
                   advice->present = TRUE;
                   increment = MN_put_charge_advice(position,advice);
                   index+= increment;
               }

               break;
            }

         case 0x85:  /* e5 */
            {
               position = &( facility_data_ptr->facility_component_data[component_number].parameters[index]);

               if(cm_fca_data != NULL) 
               {
                   advice = &( cm_fca_data->e5);
                   advice->present = TRUE;
                   increment = MN_put_charge_advice(position,advice);
                   index+= increment;
               }

               break;
            }

         case 0x86:  /* e6 */
            {
               position = &( facility_data_ptr->facility_component_data[component_number].parameters[index]);

               if(cm_fca_data != NULL)
               {
                   advice = &( cm_fca_data->e6);
                   advice->present = TRUE;
                   increment = MN_put_charge_advice(position,advice);
                   index+= increment;
               }

               break;
            }

         case 0x87:  /* e7 */
            {
               position = &( facility_data_ptr->facility_component_data[component_number].parameters[index]);

               if(cm_fca_data != NULL) 
               {
                   advice = &( cm_fca_data->e7);
                   advice->present = TRUE;
                   increment = MN_put_charge_advice(position,advice);
                   index+= increment;
               }
               break;
            }

         case NULL:  /* End-of sequence marker */

            {

                index++;

                if (facility_data_ptr->
                   facility_component_data[component_number].parameters[index] 
                      != NULL )                    /* should be null length */

                {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect length of NULL element");

                }
                else
                {

                   index++;
                   length = (byte)index;
                }
                break;
             }

         default:
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected component in advice of charge data");

               index =
                   facility_data_ptr->
                   facility_component_data[component_number].parameter_length;
               break;
            }
         }
      }

      /*  SEND THE MESSAGE  */

      MN_put_cm_mt_call_ss_ind(facility_data_ptr->connection_id,
                                  (byte)CM_FWD_AOC_IND,
                                  NULL,
                                  cm_fca_data);

   }

   else
   {

      /* 
      ** The data was unreadable by MN
      */

      if (facility_data_ptr->
            facility_component_data[component_number].component_type ==
          RETURN_ERROR)
      {
         MN_put_crss_reject(RETURN_ERROR_PROBLEM,
                            UNEXPECTED_ERROR,
                            facility_data_ptr->connection_id,
                            facility_data_ptr->
                              facility_component_data[component_number].
                            invoke_id);
      }

      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected component in advice of charge data");


   }

   if ((cm_fca_data != NULL) || (advice != NULL))
   {
#ifndef FEATURE_MODEM_HEAP
      gs_free(cm_fca_data);
#else
      modem_mem_free(cm_fca_data, MODEM_MEM_CLIENT_NAS);
#endif
   }
}


/*
 * Function name :  MN_put_charge_advice
 * -------------
 *
 * Description :  Decodes the charge advice IE
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: position - the pointter to the place in the input message stream
 * ----------  advice - the pointer to the IE in the output cm message
 *
 *
 *
 * Returns: increment - the number of bytes moved along the input message
 * --------
 *
 */
byte MN_put_charge_advice( byte                 *position, 
                           charge_advice_data_T *advice)

{

   /* Locals */

   byte  string_length,increment;

   /* Start of code */

   increment = 1;
   position++;

   /* get the number of bytes in the integer */

   string_length =  *position;

   increment++;
   position++;

   /* set up the output MSB */

   if (string_length == 1)
   {
      advice->msb = 0;

   }
   else
   {
      advice->msb = *position;
      increment++;
      position++;

   }

   /* Set up the output LSB */

   advice->lsb = *position;

   increment++;
   position++;

   /* position now points at the next tag in the input message stream */

   return(increment);

}
/***************************************************************
*                                                              *
*                   MN_execute_holdmpty_success                *
*                                                              *
****************************************************************/
/*
 * Function name :  MN_execute_holdmpty_success
 * -------------
 *
 * Description :  Successful hold mpty
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_execute_holdmpty_success(MN_FACILITY_DATA_T *facility_data,
int component_number)
{
   byte Multicall_Status_index;
   boolean success;
   boolean ret;
   int index;
   /*ie_cm_ss_error_T   ss_error;*/
   cm_connected_subaddress_T   null_subaddress = {0};
   byte no_auto_answer_on_hold = mn_get_no_auto_answer_on_hold();

   boolean                   s_respond_to_event;
   connection_id_T           connection_id;
   ie_cm_ss_error_T          cm_ss_error={0};   

  
   success = TRUE;
   //MN_MULTICALL_LOG(MN_read_multicall_status());
   /*ss_error.present= FALSE;*/

   /* clearing the SS pending tag in the beginning for sucess case*/
   if(mn_call_information_p[facility_data->connection_id] != NULL)
   {
     mn_call_information_p[facility_data->connection_id]->ss_request_pending = CM_NONE_PENDING;
     mn_call_information_p[facility_data->connection_id]->hold_request = FALSE;
   }
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= mn_execute_holdmpty_success: mn_call_information_p[%d] is NULL",
           facility_data->connection_id);
   }
   
   //Multicall_Status_index = MN_whereami(facility_data->connection_id);


   Multicall_Status_index = MN_whichMultiCallIndex(facility_data->connection_id);

   if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
   {
     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
   }
   else if (mn_alternate_proc_status.is_alternate_procedure == TRUE)
   {
     if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED)
     {
       mn_alternate_proc_status.status = MN_SS_ALT_RETRIEVE_ACK_AWAITED;
     }
     else if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_AWAITED)
     {
       mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;
       mn_alternate_proc_status.is_alternate_procedure = FALSE;

       if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
       {
         for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
         {
           success = MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].connection_id[index], CCE_HOLD_MPTY_CONF);

           /*  FLUSHING THE DTMF REQs OF EACH PARTY OF THE MPTY CALL  */         
           MN_flush_queued_dtmf_requests(Multicall_Status[Multicall_Status_index].connection_id[index]);
         }
       }
       else
       {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: MPTY CALL HOLD SUCC message received for wrong Call");       
       }

       if(success == TRUE)
       {
         Multicall_Status_index = (Multicall_Status_index + 1) % 2;
            connection_id = Multicall_Status[Multicall_Status_index].connection_id[0];

         s_respond_to_event = MN_manage_CC_auxiliary_state (connection_id,
                                                             CCE_CALL_RETRIEVE_CONF);/*c_call_event */
         if (s_respond_to_event == TRUE)
         {
            /* clear the ss pending requests */   
            mn_call_information_p[connection_id]->ss_request_pending= CM_NONE_PENDING;
            MN_stop_cc_timer(connection_id);
         }
         else
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: MNCC_CALL_RETRIEVE_CNF message received in wrong state");
         }
         cm_ss_error.present = FALSE;            
         MN_put_cm_manage_calls_conf(TRUE,
#if defined(FEATURE_DUAL_SIM )
                 mn_call_information_p[facility_data->connection_id]->as_id,
#endif
                 cm_ss_error);                

       } 
     }
     else if (mn_alternate_proc_status.status == MN_SS_ALT_NACKED)
     {
       mn_alternate_proc_status.is_alternate_procedure = FALSE;
       mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;
     
       if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
       {
         /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */

         for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
         {
           success = MN_manage_CC_auxiliary_state(
               Multicall_Status[Multicall_Status_index].connection_id[index], 
               CCE_HOLD_MPTY_REJ);
         }
       }

       if(success == TRUE)
       {
         /* clearing the SS pending tag in the successful case only*/
         mn_call_information_p[facility_data->connection_id]->ss_request_pending = CM_NONE_PENDING;
         mn_call_information_p[facility_data->connection_id]->hold_request = FALSE;
         /*  TELL THE CM  */

         cm_ss_error.error_code_tag = ERROR_CODE_TAG; /* = ErrorCodeTag */
         cm_ss_error.error_code = MN_CALL_RETRIEVE_REJ;
         (void)MN_read_multicall_status();
         MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
          mn_call_information_p[facility_data->connection_id]->as_id,
#endif
               cm_ss_error);
       }
       else
       {
         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= ALT Proc is invoked in Wrong Status ");
       }
     }
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= ALT Proc is on ,status = %d",mn_alternate_proc_status.status);      
   }
   else if( (Multicall_Status_index = MN_whereami(facility_data->connection_id)) < MULTICALL_STATUS_SIZE)
   {
     if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
     {
        for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
        {
           success = MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].connection_id[index], CCE_HOLD_MPTY_CONF);

           /*  FLUSHING THE DTMF REQs OF EACH PARTY OF THE MPTY CALL  */         
           MN_flush_queued_dtmf_requests(Multicall_Status[Multicall_Status_index].connection_id[index]);
        }
     }

     /*  NOW RETRIEVE ANY OTHER HELD CALLS  */

     if(success == TRUE)
     {
        /*  POINT AT THE OTHER CALL  */
        /* fixed compile warning C4244 */
        Multicall_Status_index = (byte)((Multicall_Status_index + 1) % 2);
        if(Multicall_Status[Multicall_Status_index].call_activity == CALL_HELD)
        {
           /*  RETRIEVE THIS HELD CALL*/

           if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
           {
              /*  THERE MUST BE TWO MPTY CALLS - IMPOSSIBLE  */
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Multiple MPTY calls");

           }
           else
           {
              /*  IT IS A SINGLE CALL  */
              ret = MN_manage_CC_auxiliary_state(
                  Multicall_Status[Multicall_Status_index].connection_id[0], 
                  CCE_CALL_RETRIEVE_CONF);

              if(ret ==TRUE)
              {
                 MN_put_MNCC_CALL_RETRIEVE_REQ(
                     Multicall_Status[Multicall_Status_index].connection_id[0]);

                 /* for marking the pending requests in order to respond to ATCOP commands later 
                 putting 2_SEND request in case of not able to find the request */
  
                 mn_call_information_p[Multicall_Status[Multicall_Status_index].connection_id[0]]->
                        ss_request_pending = CM_2_SEND_PENDING;

                 MSG_HIGH_DS_0(MN_SUB,"=MNCNM= ss request CM_2_SEND_PENDING pending");  
              }

              else
              {
                  MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Single call for connection_id %d in wrong state",
                      Multicall_Status[Multicall_Status_index].connection_id[0]);
  
              }

           }
        }

        else
        {
           if (Multicall_Status[Multicall_Status_index].call_activity == MO_CALL_PENDING)
           /* RESUME THE MO CALL SETUP */

           {
              facility_data->connection_id = Multicall_Status[Multicall_Status_index].
                  connection_id[0];
            
              /* Override the use of UL DTX between call setup and connect/connect_ack */
#ifdef FEATURE_GSM
#ifdef FEATURE_DUAL_SIM
              if(MNCNM_IS_SINGLE_STACK)
              {
                geran_grr_dtx_override_on_per_subs(mn_as_id);
              }
#ifdef FEATURE_DUAL_ACTIVE
              else
              {
                 geran_grr_dtx_override_on_per_subs((sys_modem_as_id_e_type)mn_dsda_as_id);
              }
#endif
#else

              geran_grr_dtx_override_on();
#endif

#endif
              /* RESET THE MO CALL PENDING FLAG */
              mn_call_information_p[facility_data->connection_id]->mo_setup_pending = FALSE;
              MN_put_MNCC_SETUP_REQ(facility_data->connection_id);

           }

           else

           {
              /*  GET THE WAITING CALL  */
              if((no_auto_answer_on_hold == FALSE) && (Multicall_Status[2].call_activity == CALL_WAITING))
              {
                 mn_call_information_p[Multicall_Status[2].connection_id[0]]->
                                       mt_call_accept_cause = CM_2_SEND_REQ;

                 /*  RESUME THE MT CALL SETUP  */

                 MN_put_MNCC_SETUP_RES(Multicall_Status[2].connection_id[0], 
                                      &(null_subaddress));
              }
              else
              {

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD

                MN_disconnect_TCH(MN_VOCODER_SWITCH); /* Releasing vocoder when no active call is there to support simultaneous CS/PS call */
     
                /* There was no other call */

#endif /* FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD */

                /* Send success back to CM */
                /* commented out on request from ATCOP to stop sending this message
               twice CR 91948 */
                /* MN_put_cm_manage_calls_conf(TRUE,
                     ss_error);*/
              }

           }
        }
     }

     if(success == FALSE)
     {
        if (facility_data->facility_component_data[component_number].component_type ==
            RETURN_ERROR)
        {
           MN_put_crss_reject(RETURN_ERROR_PROBLEM,   /* Problem code tag */
           UNEXPECTED_ERROR,       /* Problem code */
           facility_data->connection_id,
               facility_data->facility_component_data[component_number].
               invoke_id);
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Hold MPTY operation incorrectly returned");

        }
     }
   }
}

/***************************************************************
*                           *
*                   MN_execute_holdmpty_error                *
*                           *
****************************************************************/
/*
 * Function name :  MN_execute_holdmpty_error
 * -------------
 *
 * Description :  Unsuccessful hold MPTY 
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  component_number
 *        component_type
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_execute_holdmpty_error(MN_FACILITY_DATA_T *facility_data_ptr, int component_number,
byte component_type)
{
   boolean success;
   int index;
   ie_cm_ss_error_T cm_ss_error={0};
   ie_cm_cc_cause_T       cm_cc_cause;
   cm_bearer_capability_T bearer_capability;
   int                       Multicall_Status_index;
   connection_id_T           connection_id;

   boolean                   s_respond_to_event;

   success = TRUE;

   /*  UNSUCCESSFUL HOLD MPTY  */
   Multicall_Status_index = MN_whichMultiCallIndex(facility_data_ptr->connection_id);
   if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
   {
     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
   }
   else if (mn_alternate_proc_status.is_alternate_procedure == TRUE)
   {
      /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */

     if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
     {
        /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
        for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
        {
           success = MN_manage_CC_auxiliary_state(
               Multicall_Status[Multicall_Status_index].connection_id[index], 
               CCE_HOLD_MPTY_REJ);
        }
     }
     if(success == TRUE)
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
            Multicall_Status_index = (Multicall_Status_index + 1) % 2;            
            connection_id = Multicall_Status[Multicall_Status_index].connection_id[0];

            s_respond_to_event = MN_manage_CC_auxiliary_state (connection_id,
                                                             CCE_CALL_RETRIEVE_REJ);/*c_call_event */
            if (s_respond_to_event == TRUE)
            {
               MN_stop_cc_timer(connection_id);
            }
            else
            {
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR:ALT Proc on,CC Aux state of %d call in wrong state",connection_id);
            }
          }
          mn_alternate_proc_status.is_alternate_procedure = FALSE;
          mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;
        
          /* clearing the SS pending tag in the successful case only*/
          mn_call_information_p[facility_data_ptr->connection_id]->ss_request_pending = CM_NONE_PENDING;
          mn_call_information_p[facility_data_ptr->connection_id]->hold_request = FALSE;
          /*  TELL THE CM  */
          if(component_type == RETURN_ERROR)
          {
             cm_ss_error.error_code_tag = ERROR_CODE_TAG; /* = ErrorCodeTag */
             cm_ss_error.error_code = facility_data_ptr->facility_component_data
                 [component_number].error_code;
             MN_MULTICALL_LOG(MN_read_multicall_status());
             MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                     mn_call_information_p[facility_data_ptr->connection_id]->as_id, 
#endif 
                                      cm_ss_error);
          }

          if(component_type == REJECT)
          {
             cm_ss_error.error_code_tag = facility_data_ptr->facility_component_data
                 [component_number].problem_tag;
             cm_ss_error.error_code = facility_data_ptr->facility_component_data
                 [component_number].problem_code;
             MN_MULTICALL_LOG(MN_read_multicall_status());
             MN_put_cm_manage_calls_conf(FALSE,
#if defined(FEATURE_DUAL_SIM )
                                 mn_call_information_p[facility_data_ptr->connection_id]->as_id, 
#endif 
                                         cm_ss_error);
          }
        }
     }
     
   }
   else
   {
     if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
     {
        /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
        for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
        {
           success = MN_manage_CC_auxiliary_state(
               Multicall_Status[Multicall_Status_index].connection_id[index], 
               CCE_HOLD_MPTY_REJ);
        }
     }

     if(success == TRUE)
     {
        /* clearing the SS pending tag in the successful case only*/
        mn_call_information_p[facility_data_ptr->connection_id]->ss_request_pending = CM_NONE_PENDING;
        mn_call_information_p[facility_data_ptr->connection_id]->hold_request = FALSE;
        /*  TELL THE CM  */
        if(component_type == RETURN_ERROR)
        {
           cm_ss_error.error_code_tag = ERROR_CODE_TAG; /* = ErrorCodeTag */
           cm_ss_error.error_code = facility_data_ptr->facility_component_data
               [component_number].error_code;
           MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                     mn_call_information_p[facility_data_ptr->connection_id]->as_id, 
#endif 
               cm_ss_error);
        }

        if(component_type == REJECT)
        {
           cm_ss_error.error_code_tag = facility_data_ptr->facility_component_data
               [component_number].problem_tag;
           cm_ss_error.error_code = facility_data_ptr->facility_component_data
               [component_number].problem_code;
           MN_put_cm_manage_calls_conf(FALSE,
#if defined(FEATURE_DUAL_SIM )
                                  mn_call_information_p[facility_data_ptr->connection_id]->as_id, 
#endif 
                                       cm_ss_error);
        }
     }
     else
     {
        if (component_type == RETURN_ERROR)
        {
           MN_put_crss_reject(RETURN_ERROR_PROBLEM,   /* Problem code tag */
           UNEXPECTED_ERROR,       /* Probem code */
           facility_data_ptr->connection_id,
               facility_data_ptr->facility_component_data[component_number].
               invoke_id);
        }

        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Hold MPTY operation incorrectly returned"); 
     }

     /* Find out which call the MNCC_Call_Hold_REJ was about */
     /* Refresh the Multicall_Status array */

     MN_MULTICALL_LOG(MN_read_multicall_status());

     Multicall_Status_index = (int)MN_whereami(facility_data_ptr->connection_id);

     /*check if the other call is a MO call pending */
     /*Point at the other call */

     Multicall_Status_index = (Multicall_Status_index + 1) % 2;

     if (Multicall_Status[Multicall_Status_index].call_activity == 
          MO_CALL_PENDING)
     /* reject the mo call setup */
     {

        connection_id = Multicall_Status[Multicall_Status_index].connection_id[0];

        /* reset the mo call pending flag */

        mn_call_information_p[connection_id]->mo_setup_pending = FALSE;

        /* Set up null data structures for the message */

        memset(&(cm_cc_cause),'\0',sizeof(ie_cm_cc_cause_T));
        memset(&(bearer_capability),'\0',sizeof(cm_bearer_capability_T));

        MN_put_cm_mo_call_conf(connection_id,
                                 FALSE,      /* cnm_service_success */
                                 bearer_capability,
                                 bearer_capability,
                                 cm_cc_cause,
#if defined(FEATURE_DUAL_SIM )
                                 mn_call_information_p[connection_id]->as_id,
#endif
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
                                 NULL,
                                 NULL,
                                 NULL
#else
                                 NULL
#endif
                                );

        MN_clear_call_data(connection_id);

     }

    
   }

}

/***************************************************************
*                                                              *
*                   MN_execute_retrievempty_success            *
*                                                              *
****************************************************************/
/*
 * Function name :  MN_execute_retrievempty_success
 * -------------
 *
 * Description :  Successful retrieve MPTY
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_execute_retrievempty_success(MN_FACILITY_DATA_T *facility_data,
int component_number)
{
   byte Multicall_Status_index;
   boolean success;
   boolean ret;
   int index;

   ie_cm_ss_error_T        cm_ss_error={0};
   connection_id_T         connection_id;
   boolean                 s_respond_to_event;

   success = TRUE;

   //MN_MULTICALL_LOG(MN_read_multicall_status());
   //Multicall_Status_index = MN_whereami(facility_data->connection_id);

   Multicall_Status_index = MN_whichMultiCallIndex(facility_data->connection_id);

   /* clearing the SS pending tag*/
   mn_call_information_p[facility_data->connection_id]->ss_request_pending = CM_NONE_PENDING;
   if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
   }
   else if (mn_alternate_proc_status.is_alternate_procedure == TRUE)
   {
      if (mn_alternate_proc_status.status == MN_SS_ALT_HOLD_ACK_RETRIEVE_ACK_AWAITED)
      {
        mn_alternate_proc_status.status = MN_SS_ALT_HOLD_ACK_AWAITED;
      }
      else if (mn_alternate_proc_status.status == MN_SS_ALT_RETRIEVE_ACK_AWAITED)
      {
        mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;
        mn_alternate_proc_status.is_alternate_procedure = FALSE;

        mn_call_information_p[facility_data->connection_id]->ss_request_pending = CM_NONE_PENDING;
        if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
        {
          /*  COMPLETE THE HOLD OPERATION FOR THE MPTY CALL  */

          for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
          {
            ret = MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].
                   connection_id[index], CCE_RETRIEVE_MPTY_CONF);
            if(ret == FALSE)
            {
              success = FALSE;
            }
          }
        }
        if(success == FALSE)
        {
          if (facility_data->facility_component_data[component_number].component_type ==
              RETURN_ERROR)
          {
             MN_put_crss_reject(RETURN_ERROR_PROBLEM,
                              UNEXPECTED_ERROR,
                              facility_data->connection_id,
                              facility_data->facility_component_data[component_number].
                              invoke_id);
          }
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Retrieve MPTY operation incorrectly returned");
        }
        else
        {
          /*Point at the other call */
          Multicall_Status_index = (Multicall_Status_index + 1) % 2;

          connection_id = Multicall_Status[Multicall_Status_index].connection_id[0];
          /* Clear out any remaining DTMF requests and send the DTMF CNF to CM  */
          MN_flush_queued_dtmf_requests(connection_id);
          /* Update CC auxiliary state for this connection */
          s_respond_to_event = MN_manage_CC_auxiliary_state(connection_id,
                                                CCE_CALL_HOLD_CONF);/* c_call_event value */
          if (s_respond_to_event == TRUE)
          {
            /* clear the ss pending requests */   
            mn_call_information_p[connection_id]->ss_request_pending= CM_NONE_PENDING;
            mn_call_information_p[connection_id]->hold_request = FALSE;

            MN_stop_cc_timer(connection_id);
          }
          else
          {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: MNCC_CALL_HOLD_CNF message received in wrong state, ALT proc on");
          }
          
          cm_ss_error.present = FALSE;

          MN_put_cm_manage_calls_conf(TRUE,     /* call_ss_success */
#if defined(FEATURE_DUAL_SIM )
                     mn_call_information_p[facility_data->connection_id]->as_id, 
#endif 
                                        cm_ss_error);
        
        }
      
      }
      else if (mn_alternate_proc_status.status == MN_SS_ALT_NACKED)
      { /* The alternate proc has failed already, so reject it */
         /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
        mn_alternate_proc_status.is_alternate_procedure = FALSE;
        mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;

        for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
        {
           ret = MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].
                                                               connection_id[index], CCE_RETRIEVE_MPTY_REJ);
           if(ret == FALSE)
           {
              success = FALSE;
           }
        }

        if(success == TRUE)
        {

          /* clearing the SS pending tag only in successful case*/
          mn_call_information_p[facility_data->connection_id]->ss_request_pending = CM_NONE_PENDING;

          /*  TELL THE CM  */
          cm_ss_error.error_code_tag = ERROR_CODE_TAG; /* = ErrorCodeTag */
          cm_ss_error.error_code = MN_CALL_HOLD_REJ;
          (void)MN_read_multicall_status();
          /* keep multicall status up to date or it will fail
            * to provide CM with correct active call list */
          MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                     mn_call_information_p[facility_data->connection_id]->as_id, 
#endif 
                                      cm_ss_error);
        }
        else
        {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Retrieve MPTY error incorrectly returned, ALT Proc on");
        } /* end of else block of success */
     
      }
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= ALT Proc is on ,status = %d",mn_alternate_proc_status.status);            
      
   }
   else if( ( Multicall_Status_index = MN_whereami(facility_data->connection_id) ) < MULTICALL_STATUS_SIZE)
   {

     mn_call_information_p[facility_data->connection_id]->ss_request_pending = CM_NONE_PENDING;   

     if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
     {
        /*  COMPLETE THE HOLD OPERATION FOR THE MPTY CALL  */

        for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
        {
           ret = MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].
               connection_id[index], CCE_RETRIEVE_MPTY_CONF);
           if(ret == FALSE)
           {
              success = FALSE;
           }
        }
     }
     if(success == FALSE)
     {
        if (facility_data->facility_component_data[component_number].component_type ==
            RETURN_ERROR)
        {
           MN_put_crss_reject(RETURN_ERROR_PROBLEM,
                              UNEXPECTED_ERROR,
                              facility_data->connection_id,
                              facility_data->facility_component_data[component_number].
                              invoke_id);
        }
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Retrieve MPTY operation incorrectly returned");
     }

#ifdef FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD 
     MN_MULTICALL_LOG(MN_read_multicall_status()); /* update the call_activity of Multicall_Status */
     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Retrive Operation succeded - Acquiring Vocoder");

     if (tch_user_connections_store[DEFAULT_RAB_ID].OTA_channel_connected == TRUE)
     {
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= BC.ITC  : %d\n ",mn_call_information_p[facility_data->connection_id]->bearer_capability_1.information_transfer_capability );
  
        /* Connect the appropriate traffic channel */
  
        MN_connect_TCH(mn_call_information_p[facility_data->connection_id]->bearer_capability_1.
               information_transfer_capability); /* Vocoder was released by NAS at the time of Call Hold */
     }     

#endif /*FEATURE_NAS_VOC_REL_AFTER_CALL_HOLD  */

   }/* else of alternate proc stat */

}


/***************************************************************
*                           *
*                   MN_execute_retrievempty_error         *
*                           *
****************************************************************/
/*
 * Function name :  MN_execute_retrievempty_error
 * -------------
 *
 * Description :  Unsuccessful retrieve MPTY
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  component_number
 *        component_type
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_execute_retrievempty_error(MN_FACILITY_DATA_T *facility_data_ptr, 
int component_number,
byte component_type)
{
   /*  LOCALS  */
   boolean success;
   boolean ret;
   byte Multicall_Status_index;
   int index;
   ie_cm_ss_error_T cm_ss_error={0};

   boolean                   s_respond_to_event;
   connection_id_T              connection_id;


   success = TRUE;

   //MN_MULTICALL_LOG(MN_read_multicall_status());
   //Multicall_Status_index = MN_whereami(facility_data_ptr->connection_id);

   Multicall_Status_index = MN_whichMultiCallIndex(facility_data_ptr->connection_id);
   
   if(Multicall_Status_index >= MULTICALL_STATUS_SIZE)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
   }
   else if (mn_alternate_proc_status.is_alternate_procedure == TRUE)  
   {
      if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
      {
         /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
         for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
         {
            ret = MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].
                connection_id[index], CCE_RETRIEVE_MPTY_REJ);

            if(ret == FALSE)
            {
               success = FALSE;
            }
         }
      }

      if(success == TRUE)
      {
         /* clearing the SS pending tag only in successful case*/
         mn_call_information_p[facility_data_ptr->connection_id]->ss_request_pending = CM_NONE_PENDING;

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
               Multicall_Status_index =  (Multicall_Status_index + 1) % 2;

               connection_id = Multicall_Status[Multicall_Status_index].connection_id[0];

               s_respond_to_event = MN_manage_CC_auxiliary_state (connection_id,
                                                             CCE_CALL_HOLD_REJ);/*c_call_event */
               if (s_respond_to_event == TRUE)
               {
                  MN_stop_cc_timer(connection_id);   
               }
               else
               {
                  MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR:ALT Proc on,CC Aux state of %d call in wrong state",connection_id);
               }
           }
           mn_alternate_proc_status.is_alternate_procedure = FALSE;
           mn_alternate_proc_status.status = MN_SS_ALT_NONE_AWAITED;
                 
           /*  TELL THE CM  */
           if(component_type == RETURN_ERROR)
           {
              cm_ss_error.error_code_tag = ERROR_CODE_TAG; /* = ErrorCodeTag */
              cm_ss_error.error_code = facility_data_ptr->facility_component_data
                  [component_number].error_code;
              (void)MN_read_multicall_status();
                /* keep multicall status up to date or it will fail
                  * to provide CM with correct active call list */
              MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                              mn_call_information_p[facility_data_ptr->connection_id]->as_id, 
#endif 
                                          cm_ss_error);
           }

           if(component_type == REJECT)
           {
              cm_ss_error.error_code_tag = facility_data_ptr->facility_component_data
                  [component_number].problem_tag;
              cm_ss_error.error_code = facility_data_ptr->facility_component_data
                  [component_number].problem_code;
              (void)MN_read_multicall_status();
                /* keep multicall status up to date or it will fail
                 * to provide CM with correct active call list */
              MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
       mn_call_information_p[facility_data_ptr->connection_id]->as_id,
#endif
                                          cm_ss_error);
           }
         }
         else
         {
           if (facility_data_ptr->facility_component_data[component_number].component_type ==
             RETURN_ERROR)
           {
              MN_put_crss_reject(RETURN_ERROR_PROBLEM,
                  UNEXPECTED_ERROR,
                  facility_data_ptr->connection_id,
                  facility_data_ptr->facility_component_data[component_number].
                  invoke_id);
           }
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Retrieve MPTY error incorrectly returned");
         }
      }
   
   }
   else if( ( Multicall_Status_index = MN_whereami(facility_data_ptr->connection_id) ) < MULTICALL_STATUS_SIZE)
   {
      if(Multicall_Status[Multicall_Status_index].call_is_mpty == TRUE)
      {
         /*  REVERSE THE HOLD CALL OPERATION FOR THE MPTY CALL  */
         for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
         {
            ret = MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].
                connection_id[index], CCE_RETRIEVE_MPTY_REJ);

            if(ret == FALSE)
            {
               success = FALSE;
            }
         }
      }

      if(success == TRUE)
      {

         /* clearing the SS pending tag only in successful case*/
         mn_call_information_p[facility_data_ptr->connection_id]->ss_request_pending = CM_NONE_PENDING;

         /*  TELL THE CM  */
         if(component_type == RETURN_ERROR)
         {
            cm_ss_error.error_code_tag = ERROR_CODE_TAG; /* = ErrorCodeTag */
            cm_ss_error.error_code = facility_data_ptr->facility_component_data
                [component_number].error_code;
            (void)MN_read_multicall_status();
              /* keep multicall status up to date or it will fail
               * to provide CM with correct active call list */
            MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
        mn_call_information_p[facility_data_ptr->connection_id]->as_id,
#endif
                                        cm_ss_error);
         }

         if(component_type == REJECT)
         {
            cm_ss_error.error_code_tag = facility_data_ptr->facility_component_data
                [component_number].problem_tag;
            cm_ss_error.error_code = facility_data_ptr->facility_component_data
                [component_number].problem_code;
            (void)MN_read_multicall_status();
              /* keep multicall status up to date or it will fail
               * to provide CM with correct active call list */
            MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
        mn_call_information_p[facility_data_ptr->connection_id]->as_id,
#endif
                                        cm_ss_error);
         }
      }
      else
      {
         if (facility_data_ptr->facility_component_data[component_number].component_type ==
             RETURN_ERROR)
         {
            MN_put_crss_reject(RETURN_ERROR_PROBLEM,
                UNEXPECTED_ERROR,
                facility_data_ptr->connection_id,
                facility_data_ptr->facility_component_data[component_number].
                invoke_id);
         }

         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Retrieve MPTY error incorrectly returned");

      }
   }

}



/***************************************************************
*                                                              *
*                   MN_execute_callDeflection_error            *
*                                                              *
****************************************************************/
/*
 * Function name :  MN_execute_callDeflection_error
 * -------------
 *
 * Description :  Unsuccessful retrieve MPTY
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  component_number
 *        component_type
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_execute_callDeflection_error(MN_FACILITY_DATA_T *facility_data_ptr, 
int component_number,
byte component_type)
{
    ie_cm_ss_error_T cm_ss_error={0};
 
    /*  TELL THE CM  */
    if(component_type == RETURN_ERROR)
    {
         cm_ss_error.error_code_tag = ERROR_CODE_TAG; /* = ErrorCodeTag */
         cm_ss_error.error_code = facility_data_ptr->facility_component_data
             [component_number].error_code;
         MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                     mn_call_information_p[facility_data_ptr->connection_id]->as_id, 
#endif 
                                     cm_ss_error);
    }
    else if(component_type == REJECT)
    {
         cm_ss_error.error_code_tag = facility_data_ptr->facility_component_data
             [component_number].problem_tag;
         cm_ss_error.error_code = facility_data_ptr->facility_component_data
             [component_number].problem_code;
         MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                     mn_call_information_p[facility_data_ptr->connection_id]->as_id, 
#endif 
                                     cm_ss_error);
    }
    else
    {
       if (facility_data_ptr->facility_component_data[component_number].component_type ==
          RETURN_ERROR)
       {
           MN_put_crss_reject(RETURN_ERROR_PROBLEM,
             UNEXPECTED_ERROR,
             facility_data_ptr->connection_id,
             facility_data_ptr->facility_component_data[component_number].
             invoke_id);
       }

       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Call Deflection error incorrectly returned");
    }
}


/***************************************************************
*                           *
*                   MN_execute_splitmpty_success          *
*                           *
****************************************************************/
/*
 * Function name :  MN_execute_splitmpty_success
 * -------------
 *
 * Description :  Successful split MPTY
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_execute_splitmpty_success(MN_FACILITY_DATA_T *facility_data,
int component_number)
{
   int index, Multicall_Status_index;

   /* clearing the SS pending tag*/
   if(mn_call_information_p[facility_data->connection_id] != NULL)
   {
     mn_call_information_p[facility_data->connection_id]->ss_request_pending = CM_NONE_PENDING;
   } 
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN_execute_splitmpty_success: mn_call_information_p[%d] is NULL",
           facility_data->connection_id);
   }

   /* Update connection ID state.  Make this connection ACTIVE */
   if(MN_manage_CC_auxiliary_state(facility_data->connection_id, CCE_SPLIT_MPTY_CONF))
   {
       /* Refresh the Multicall Session from above state changes */

       Multicall_Status_index = MN_whereami(facility_data->connection_id);

       /*  DEAL WITH THE OTHER CALL WHICH SHOULD BE HELD  */
     
       Multicall_Status_index = (byte)((Multicall_Status_index + 1) % 2);

       /*  DEAL WITH A HELD MPTY CALL  */
       for(index = 0; index < Multicall_Status[Multicall_Status_index].id_index; index++)
       {
              (void)MN_manage_CC_auxiliary_state(Multicall_Status[Multicall_Status_index].
                            connection_id[index], CCE_HOLD_MPTY_CONF);

       }
   }
   else 
   {
      if (facility_data->facility_component_data[component_number].component_type ==
          RETURN_ERROR)
      {
         MN_put_crss_reject(RETURN_ERROR_PROBLEM,
             UNEXPECTED_ERROR,
             facility_data->connection_id,
             facility_data->facility_component_data[component_number].
             invoke_id);
      }

      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Split MPTY operation incorrectly returned");

   }
}


/***************************************************************
*                           *
*                   MN_execute_splitmpty_error            *
*                           *
****************************************************************/
/*
 * Function name :  MN_execute_splitmpty_error
 * -------------
 *
 * Description :  Unsuccessful splitMPTY
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  component_number
 *        component_type
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_execute_splitmpty_error(MN_FACILITY_DATA_T *facility_data_ptr, 
int component_number,
byte component_type)
{
    boolean success;
    ie_cm_ss_error_T cm_ss_error={0};

    success = MN_manage_CC_auxiliary_state(facility_data_ptr->connection_id, CCE_SPLIT_MPTY_REJ);

    if(success == TRUE)
    {
      /* clearing the SS pending tag in sucessful case only*/
      mn_call_information_p[facility_data_ptr->connection_id]->ss_request_pending = CM_NONE_PENDING;    
      /*  TELL THE CM  */

      if(component_type == RETURN_ERROR)
      {
         cm_ss_error.error_code_tag = ERROR_CODE_TAG; /* = ErrorCodeTag */
         cm_ss_error.error_code = facility_data_ptr->facility_component_data
             [component_number].error_code;
         MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                     mn_call_information_p[facility_data_ptr->connection_id]->as_id, 
#endif 
                                     cm_ss_error);
      }

      if(component_type == REJECT)
      {
         cm_ss_error.error_code_tag = facility_data_ptr->
             facility_component_data[component_number].problem_tag;
         cm_ss_error.error_code = facility_data_ptr->
             facility_component_data[component_number].problem_code;
         MN_put_cm_manage_calls_conf(FALSE, 
#if defined(FEATURE_DUAL_SIM )
                                     mn_call_information_p[facility_data_ptr->connection_id]->as_id, 
#endif 
                                     cm_ss_error);
      }
    }
    else
    {
      /* Reject the split MPTY */

      if (facility_data_ptr->facility_component_data[component_number].
          component_type == RETURN_ERROR)
      {
         MN_put_crss_reject(RETURN_ERROR_PROBLEM,
             UNEXPECTED_ERROR,
             facility_data_ptr->connection_id,
             facility_data_ptr->facility_component_data[component_number].
             invoke_id);
      }

      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Hold MPTY operation incorrectly returned");
    }
}

#ifdef FEATURE_UUS
/***************************************************************
*                                                              *
*                   MN_execute_user_data_success               *
*                                                              *
****************************************************************/
/*
 * Function name :  MN_execute_user_data_success
 * -------------
 *
 * Description :  Successful activation of userUserService.
 * -----------     
 *
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  
 *
 * Returns:  None.
 * --------
 * 
 */

VOID_FUNC MN_execute_user_data_success(MN_FACILITY_DATA_T *facility_data, int component_number)
{
   connection_id_T          connection_id; 
   
   /* Update user user service status in MN call information */
   if((connection_id = facility_data->connection_id) <= MAX_MO_CONNECTION_ID)
   {
       if(mn_call_information_p[connection_id] != NULL)
       {
         /* set the status */
         mn_call_information_p[connection_id]->user_user_data.activated = TRUE;

         /* send the user data */
         (void)MN_put_USER_USER_DATA_REQ(connection_id);
       } 
       else
       {
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN_execute_user_data_success:mn_call_information_p[%d] is NULL",
                connection_id);
       }
   }
   else
   {
       MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid conn = %d, comp = %d", connection_id, component_number);
   }
}

/***************************************************************
*                                                              *
*                   MN_execute_user_data_success               *
*                                                              *
****************************************************************/
/*
 * Function name :  MN_execute_user_data_error
 * -------------
 *
 * Description :  Successful activation of userUserService.
 * -----------     
 *
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  
 *
 * Returns:  None.
 * --------
 * 
 */

VOID_FUNC MN_execute_user_data_error(MN_FACILITY_DATA_T *facility_data, int component_number)
{
   connection_id_T          connection_id; 
   
   /* Update user user service status in MN call information */
   if((connection_id = facility_data->connection_id) <= MAX_MO_CONNECTION_ID)
   {
       if(mn_call_information_p[connection_id] != NULL)
       {
         /* set the status */
         mn_call_information_p[connection_id]->user_user_data.activated = TRUE;

         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= User-User Activation Failed");
       }
       else
   {
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MN_execute_user_data_error:mn_call_information_p[%d] is NULL",
                connection_id);
      }

   }
   else
   {
       MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid conn = %d, comp = %d", connection_id, component_number);
   }
}
#endif

#ifdef FEATURE_CCBS
/***************************************************************
*                                                              *
*           MN_execute_accessRegisterCCEntry_success           *
*                                                              *
****************************************************************/
/*
 * Function name :  MN_execute_accessRegisterCCEntry_success
 * -------------
 *
 * Description :  Successful activation of accessRegisterCCEntry
 * -----------     
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  
 *
 * Returns:  None.
 * --------
 */
VOID_FUNC MN_execute_accessRegisterCCEntry_success(
    MN_FACILITY_DATA_T *facility_data,
    int                 component_number
)
{
   connection_id_T       cid=facility_data->connection_id;
   int                   dummy=0;
   ie_ccbs_indicator_T   info;
   boolean               success;
   byte                 *ptr;
   byte                 *last_octet_data_pointer;

   memset (&info, 0x00, sizeof(info));
   ptr = facility_data->facility_component_data[component_number].parameters;
   last_octet_data_pointer = ptr + MAX_SS_PARAMETER_LENGTH - 1;

   if (cid < 0x07) /* accessRegisterCCEntry only valid for MO calls */
   {
      success = MN_get_ccbs_feature (ptr+1, &dummy, &info, last_octet_data_pointer);
        /* have 1st arg point to len field */

      if (success)
      {
         MN_put_cm_5_send_conf (cid, 
                                CCBS_RET_RES, 
                                &info, 
#if defined(FEATURE_DUAL_SIM )
                                mn_call_information_p[cid]->as_id,
#endif
                                NULL);
      }
      else
      {
         ie_cm_ss_error_T      error;

         error.present        = TRUE;
         error.error_code_tag = MN_SS_ERROR_TAG;
         error.error_code     = MN_INSUFFICIENT_RESOURCES;
         error.error_type     = UNKNOWN_ERR;

         MN_put_cm_5_send_conf (cid, 
                                CCBS_RET_ERR, 
                                NULL, 
#if defined(FEATURE_DUAL_SIM )
                                mn_call_information_p[cid]->as_id,
#endif
                                &error);
      }
   }
   else
   {
      MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid connId=%d, comp=%d", cid, component_number);
   }
}

/***************************************************************
*                                                              *
*           MN_execute_accessRegisterCCEntry_error             *
*                                                              *
****************************************************************/
/*
 * Function name :  MN_execute_accessRegisterCCEntry_error
 * -------------
 *
 * Description :  Unsuccessful activation of accessRegisterCCEntry
 * -----------     
 *
 * Parameters: MN_FACILITY_DATA_T *
 * ----------  
 *
 * Returns:  None.
 * --------
 */
VOID_FUNC MN_execute_accessRegisterCCEntry_error(
      MN_FACILITY_DATA_T *facility_data,
      int                 component_number,
      ie_cm_ss_error_T   *cm_ss_error
      )
{
   connection_id_T        cid=facility_data->connection_id;
   cm_5_send_conf_e_type  type=CCBS_RET_ERR;

   if (cid < 0x07) /* accessRegisterCCEntry only valid for MO calls */
   {
      if (cm_ss_error->error_code_tag != MN_SS_ERROR_TAG)
      {
         type = CCBS_REJ;
      }

      MN_put_cm_5_send_conf (cid, 
                             type, 
                             NULL, 
#if defined(FEATURE_DUAL_SIM )
                             mn_call_information_p[cid]->as_id,
#endif
                             cm_ss_error);
   }
   else
   {
      MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid connId=%d, comp=%d", cid, component_number);
   }
}
#endif /* FEATURE_CCBS */


/*
 *   Function name : MN_process_MNCC_FACILITY_IND
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function decodes facility messages from the network
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


VOID_FUNC   MN_process_MNCC_FACILITY_IND(IMH_T    *message)

{

   /* Locals */

   MNCC_FACILITY_IND_T   *message_ptr;
   MN_FACILITY_DATA_T    *MN_FACILITY_DATA_pointer;
   int                   component_number;
   byte                  invoke_id;
   byte                  component_type;
   byte                  *message_position;
   int                   message_index;
   int                   message_length;
   byte                *last_octet_data_pointer;
   ie_cm_ss_error_T      dummy_ss_error = { 0 }; 
   ie_cm_ss_error_T      cm_ss_error;
   return_size_T         size;

#ifdef FEATURE_CCBS
   memset (&cm_ss_error, 0x00, sizeof(cm_ss_error));
#endif /* FEATURE_CCBS */

   /* align the pointers */

   message_ptr = (MNCC_FACILITY_IND_T *)message;
   message_index = 0;

   /* initialise all data elments in MN_FACILITY_DATA */

#ifndef FEATURE_MODEM_HEAP
      MN_FACILITY_DATA_pointer = 
               (MN_FACILITY_DATA_T *) gs_alloc(sizeof(MN_FACILITY_DATA_T));
#else
      MN_FACILITY_DATA_pointer = 
               (MN_FACILITY_DATA_T *) modem_mem_calloc(1, sizeof(MN_FACILITY_DATA_T), MODEM_MEM_CLIENT_NAS);
#endif

   if ( MN_FACILITY_DATA_pointer != NULL )
   {

      memset( (void *)MN_FACILITY_DATA_pointer, 
              0xff, 
              sizeof(MN_FACILITY_DATA_T));

      message_length = message_ptr->message_header.message_len_lsb +
                            (message_ptr->message_header.message_len_msb * 256);

      /* set the fixed data elements before retrieving component(s) */

      MN_FACILITY_DATA_pointer->connection_id = message_ptr->connection_id;
      message_index++;
      message_index += sizeof(sys_modem_as_id_e_type);
      message_position = message_ptr->data;
      last_octet_data_pointer = message_position + sizeof(ie_facility_T) - 1;

      if (*message_position == IEI_FACILITY)
      {
         message_position++;
         message_index++;

         if  ( *message_position != 0)
         {

            message_index++;
            message_position++;

            /* 
             * retrieve the components from the message into regular structures
             */

            MN_get_MNCC_FACILITY_IND(message_position, 
                                    (word) (message_length-message_index),
                                     MN_FACILITY_DATA_pointer,
                                     last_octet_data_pointer);

         }
      }

      /* analyse them individually */

      for (component_number = 0; 
           component_number < MAX_COMPONENTS; 
           component_number++)

      {

    byte operation = NULL;

         switch(MN_FACILITY_DATA_pointer->
                  facility_component_data[component_number].component_type)
         {

         case INVOKE:

            switch (MN_FACILITY_DATA_pointer->
                     facility_component_data[component_number].operational_code)

            {

            case forwardChargeAdvice:

               MN_execute_forward_charge_advice(MN_FACILITY_DATA_pointer,
                                                 component_number);

               break;


            case notifySS:

               MN_execute_ss_notify(MN_FACILITY_DATA_pointer,component_number);
               break;

            default:

               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected invoke component received in call dependent ss");

               break;

            }

            break;



         case RETURN_RESULT:

            /* This represent a successful MO ss operation outcome */

            /* Stop the timer */

            invoke_id = 
                MN_FACILITY_DATA_pointer->
                        facility_component_data[component_number].invoke_id;

            MN_stop_ss_timer(invoke_id);


            /* MPTY operations do not return any parameters (GSM 04.84 ) */

            switch (operation = MN_retrieve_ss_operation(invoke_id, TRUE))  /* release the invoke_id */

            {

            case buildMPTY:

               /* Tell CM that MO SS operation was successful */

               MN_execute_buildmpty_success(MN_FACILITY_DATA_pointer, 
                                             component_number);

               MN_put_cm_manage_calls_conf(TRUE,
#if defined(FEATURE_DUAL_SIM )
                                            mn_call_information_p[MN_FACILITY_DATA_pointer->connection_id]->as_id, 
#endif 
                                            dummy_ss_error);


               break;

            case splitMPTY:

               /* Tell CM that MO SS operation was successful */

               MN_execute_splitmpty_success(MN_FACILITY_DATA_pointer, 
                                             component_number);

               MN_put_cm_manage_calls_conf(TRUE,
#if defined(FEATURE_DUAL_SIM )
                                            mn_call_information_p[MN_FACILITY_DATA_pointer->connection_id]->as_id, 
#endif 
                                            dummy_ss_error);

               break;

            case holdMPTY:

               /* Tell CM that MO SS operation was successful */
               if (mn_alternate_proc_status.is_alternate_procedure == TRUE)
               {
                 MN_execute_holdmpty_success(MN_FACILITY_DATA_pointer, 
                                           component_number);
               }
               else
               {

                 MN_execute_holdmpty_success(MN_FACILITY_DATA_pointer, 
                                           component_number);
                 MN_put_cm_manage_calls_conf(TRUE,
#if defined(FEATURE_DUAL_SIM )
                                            mn_call_information_p[MN_FACILITY_DATA_pointer->connection_id]->as_id, 
#endif 
                                              dummy_ss_error);
               }

               break;


            case retrieveMPTY:

               /* Tell CM that MO SS operation was successful */
               if (mn_alternate_proc_status.is_alternate_procedure == TRUE)
               {
                 MN_execute_retrievempty_success(MN_FACILITY_DATA_pointer, 
                                                component_number);
               }
               else
               {
                 MN_execute_retrievempty_success(MN_FACILITY_DATA_pointer, 
                                                component_number);

                 MN_put_cm_manage_calls_conf(TRUE,
#if defined(FEATURE_DUAL_SIM )
                                            mn_call_information_p[MN_FACILITY_DATA_pointer->connection_id]->as_id, 
#endif 
                                            dummy_ss_error);
               }

               break;

            case processUnstructuredSS_Data:
               {
                  
                  int                  index = 0;
                  ss_UserData_info_T   ss_UserData_info;
                  byte                 *parameters;
                  byte                 *ptr;
                  byte                 cm_ss_ref;
                  byte                 *last_octet_data_pointer; /* scope of this variable is this case only. */

                  parameters =
                      (byte *)MN_FACILITY_DATA_pointer->
                        facility_component_data[0].parameters;
                  last_octet_data_pointer = parameters + MAX_SS_PARAMETER_LENGTH -1;

                  if (parameters[index++] == IA5_STRING)
                  {

                     memset(&ss_UserData_info,'\0',sizeof(ss_UserData_info_T));

                     ss_UserData_info.data_type = USS_DATA_INFO;
                     ss_UserData_info.data.ss_User_Data.present = TRUE;

                     ptr = &(parameters[index++]);

                     size = MN_get_element_length(ptr,SS_MAX_USS_DATA_LENGTH, (last_octet_data_pointer - ptr + 1));

                     index+= size.increment;

                     ss_UserData_info.data.ss_User_Data.size = (byte)size.length;

                     memscpy(ss_UserData_info.data.ss_User_Data.ss_UserData,
                         sizeof(ss_UserData_info.data.ss_User_Data.ss_UserData),
                         &(parameters[index]),
                         (unsigned int)size.length);

                     cm_ss_ref = ss_invoke_data[invoke_id].cm_ss_ref;

                     MN_put_cm_process_unstructured_ss_data_conf(TRUE, 
                                                                 cm_ss_ref, 
#if defined(FEATURE_DUAL_SIM )
                                         mn_call_information_p[MN_FACILITY_DATA_pointer->connection_id]->as_id,
#endif
                                                                 &ss_UserData_info);
                  }
                  else
                  {
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected contents in SS Process Unstructured");
                  }

                  break;
               }

            case callDeflection:
            case explicitCT:
                {
                    MN_stop_cc_timer(message_ptr->connection_id);

                    MN_put_cm_manage_calls_conf(TRUE, 
#if defined(FEATURE_DUAL_SIM )
                                            mn_call_information_p[message_ptr->connection_id]->as_id, 
#endif 
                                            dummy_ss_error);

                    break;
                }

#ifdef FEATURE_UUS
            case userUserService:
                {
                    MN_execute_user_data_success(MN_FACILITY_DATA_pointer, 
                                             component_number);
                    break;
                }
#endif

#ifdef FEATURE_CCBS
            case accessRegisterCCEntry:
                MN_execute_accessRegisterCCEntry_success ( MN_FACILITY_DATA_pointer,
                                                           component_number);
                break;
#endif /* FEATURE_CCBS */

            default:
                {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected ss operation in Return Result Component");

                    break;
                }
            }

            break;

         case RETURN_ERROR: /*This represent a failed MO ss operation outcome*/

            /* The error parameters are given in (GSM 04.84 section 4.5) */

            /* Stop the timer */

            invoke_id = 
                  MN_FACILITY_DATA_pointer->
                     facility_component_data[component_number].invoke_id;

            component_type = MN_FACILITY_DATA_pointer->facility_component_data[
                                              component_number].component_type;


            MN_stop_ss_timer(invoke_id);

            /* Inform CM of error */

            cm_ss_error.present = TRUE;
            cm_ss_error.parameter.cm_system_failure_error.present = TRUE;

            cm_ss_error.error_code_tag = MN_SS_ERROR_TAG;

            cm_ss_error.error_code = 
                MN_FACILITY_DATA_pointer->
                  facility_component_data[component_number].error_code;

            MN_decode_error_parameters(&cm_ss_error, MN_FACILITY_DATA_pointer);


            switch(MN_retrieve_ss_operation(invoke_id, TRUE))

            /* release invoke id */

            {

            case  buildMPTY:

               MN_execute_buildmpty_error(MN_FACILITY_DATA_pointer,
                   component_number,
                   component_type);
               break;


            case splitMPTY:

               MN_execute_splitmpty_error(MN_FACILITY_DATA_pointer,
                   component_number,
                   component_type);
               break;


            case holdMPTY:

               MN_execute_holdmpty_error(MN_FACILITY_DATA_pointer,
                   component_number,
                   component_type);
               break;


            case retrieveMPTY:

               MN_execute_retrievempty_error(MN_FACILITY_DATA_pointer,
                   component_number,
                   component_type);
               break;

            case processUnstructuredSS_Data:

               MN_select_ss_error(ss_invoke_data[invoke_id].cm_ss_ref,
                   processUnstructuredSS_Data,
#if defined(FEATURE_DUAL_SIM )
                    mn_call_information_p[MN_FACILITY_DATA_pointer->connection_id]->as_id,
#endif
                   cm_ss_error);
               break;

            case processUnstructuredSS_Request:
               MN_select_ss_error(ss_invoke_data[invoke_id].cm_ss_ref,
                                  processUnstructuredSS_Request,
#if defined(FEATURE_DUAL_SIM )
                                  mn_call_information_p[MN_FACILITY_DATA_pointer->connection_id]->as_id,
#endif
                                  cm_ss_error);
               break;

            case callDeflection:
            case explicitCT:

               MN_stop_cc_timer(message_ptr->connection_id);

               MN_execute_callDeflection_error(MN_FACILITY_DATA_pointer,
                   component_number,
                   component_type);

               break;

#ifdef FEATURE_UUS
            case userUserService:
                {
                    MN_execute_user_data_error(MN_FACILITY_DATA_pointer, 
                                             component_number);
                    break;
                }
#endif

#ifdef FEATURE_CCBS
            case accessRegisterCCEntry:
               MN_execute_accessRegisterCCEntry_error( MN_FACILITY_DATA_pointer,
                                                       component_number,
                                                      &cm_ss_error);
               break;
#endif /* FEATURE_CCBS */

            default:

               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected ss operation in Return Result Component");

               break;

            }

            break;


         case REJECT:   /* This represent a failed MO ss operation outcome */

            /* The problem parameters are given in (GSM 04.80 table 3.14) */

            /* Stop the timer */

            if (MN_FACILITY_DATA_pointer->
               facility_component_data[component_number].invoke_id_present 
                                                                     == TRUE)
            {
               invoke_id = 
                   MN_FACILITY_DATA_pointer->
                     facility_component_data[component_number].invoke_id;

            }
            else
            {
               /* Find the invoke id corresponding to the given connection id */

               int success = FALSE;

               for ( invoke_id = 0; 
                     (invoke_id < MAXNO_INVOKE_IDS)&&(success==FALSE); 
                     invoke_id++)
               {
                  if (ss_invoke_data[invoke_id].connection_id == 
                                                   message_ptr->connection_id)
                  {

                     success = TRUE;
                  }
               }
               /* decrement invoke_id if MAX_INVOKE_IDS is reached to avoid
               out-of-bounds errors later */
               if (invoke_id == MAXNO_INVOKE_IDS)
               {
                   invoke_id--;
               }

            }

            component_type = MN_FACILITY_DATA_pointer->facility_component_data[
                                              component_number].component_type;

            MN_stop_ss_timer(invoke_id);

            /* Inform CM of problem */

            cm_ss_error.error_code_tag = 
                MN_FACILITY_DATA_pointer->
                        facility_component_data[component_number].problem_tag;
            cm_ss_error.error_code = 
                MN_FACILITY_DATA_pointer->
                        facility_component_data[component_number].problem_code;

            /* Null the IEI of the error union */
            cm_ss_error.present = TRUE;
            cm_ss_error.parameter.cm_system_failure_error.present = TRUE;


            /* release invoke id for further use */

            switch(MN_retrieve_ss_operation(invoke_id, TRUE))   
                                            

            {

            case buildMPTY:

               MN_execute_buildmpty_error( MN_FACILITY_DATA_pointer,
                                           component_number,
                                           component_type);
               break;

            case splitMPTY:

               MN_execute_splitmpty_error(MN_FACILITY_DATA_pointer,
                                           component_number,
                                           component_type);
               break;

            case holdMPTY:

               MN_execute_holdmpty_error(MN_FACILITY_DATA_pointer,
                                           component_number,
                                           component_type);
               break;


            case retrieveMPTY:

               MN_execute_retrievempty_error(MN_FACILITY_DATA_pointer,
                                              component_number,
                                              component_type);
               break;

            case processUnstructuredSS_Data:
               MN_select_ss_error(ss_invoke_data[invoke_id].cm_ss_ref,
                                  processUnstructuredSS_Data,
#if defined(FEATURE_DUAL_SIM )
                                  mn_call_information_p[MN_FACILITY_DATA_pointer->connection_id]->as_id,
#endif
                                  cm_ss_error);
               break;

            case processUnstructuredSS_Request:
               MN_select_ss_error(ss_invoke_data[invoke_id].cm_ss_ref,
                                  processUnstructuredSS_Request,
#if defined(FEATURE_DUAL_SIM )
                                  mn_call_information_p[MN_FACILITY_DATA_pointer->connection_id]->as_id,
#endif
                                  cm_ss_error);
               break;
              
            case callDeflection:
            case explicitCT:

               MN_stop_cc_timer(message_ptr->connection_id);

               MN_execute_callDeflection_error(MN_FACILITY_DATA_pointer,
                   component_number,
                   component_type);
         
               break;

#ifdef FEATURE_UUS
            case userUserService:
                {
                    MN_execute_user_data_error(MN_FACILITY_DATA_pointer, 
                                             component_number);
                    break;
                }
#endif

#ifdef FEATURE_CCBS
            case accessRegisterCCEntry:
               MN_execute_accessRegisterCCEntry_error( MN_FACILITY_DATA_pointer,
                                                       component_number,
                                                      &cm_ss_error);
               break;
#endif /* FEATURE_CCBS */
            default:

                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected ss operation in Return Result Component");

                break;

            }   /* Switch(operation()) */

            break;


         case UNKNOWN:   /* MN_get_MNCC_FACILITY_IND found no more components */

            break;

         default:

             MSG_ERROR_DS_1(MN_SUB,"=MNCNM= ERROR: Unexpected ss component type for op = %d ", operation); 

            break;

         } /* Switch (component) */

      }
      
#ifndef FEATURE_MODEM_HEAP
      gs_free(MN_FACILITY_DATA_pointer);
#else
      modem_mem_free(MN_FACILITY_DATA_pointer, MODEM_MEM_CLIENT_NAS);
#endif

   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unable to allocate memory");

   }
}


/*
 *   Function name : MN_manage_CC_auxiliary_state
 *   ---------------
 *
 *   Description:
 *   ------------
 * This function controls the transitions between the
 * allowed combinations of the Hold Auxiliary State
 * and the MPTY Auxiliary State
 *
 *  Parameters:
 *  -----------
 *  IN 
 *
 *  Returns:
 *  s_respond_to_event
 *
 */


boolean   MN_manage_CC_auxiliary_state(connection_id_T  connection_id,int  c_call_event)
{

   /* locals */

  unsigned int       CC_auxiliary_state, Multicall_index;
  boolean   s_respond_to_event;


  /* Multicall_Status_index[] may yeild connection_id = 255 */
  if(connection_id < MAXNO_CC_TRANSACTIONS)
  {
   if(cc_transaction_information_p[connection_id] != NULL)
   {
     /* Builds CC_auxiliary_state from mpty_auxiliary */
     CC_auxiliary_state = (byte)(cc_transaction_information_p[connection_id]->mpty_auxiliary_state +
         (4* cc_transaction_information_p[connection_id]->hold_auxiliary_state));

     /* Control the cc auxiliary states according to state diagram 2.3.1.7-s1 */

     switch(CC_auxiliary_state)

     {

     case AUX_IDLE_HOLD_AND_MPTY:
        {
           switch(c_call_event)
           {

           case CCE_BUILD_MPTY_REQ:
              {
                 CC_auxiliary_state = AUX_BUILD_REQUEST_FROM_CALL_ACTIVE;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_BUILD_MPTY_CONF:
              {
                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_HOLD_REQ:
              {
                 CC_auxiliary_state = AUX_HOLD_REQUEST;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_ENDED:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           default:
              {
                 s_respond_to_event = FALSE;

                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unacceptable cmd received %d in AUX_IDLE_HOLD_AND_MPTY state for connection_id %d",
                     c_call_event,connection_id);

              }

           }
           break;

        }

     case AUX_BUILD_REQUEST_FROM_CALL_ACTIVE:
        {

           switch(c_call_event)
           {

           case CCE_BUILD_MPTY_CONF:
              {
                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_BUILD_MPTY_REJ:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_ENDED:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           default:
              {

                 s_respond_to_event = FALSE;

                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unacceptable cmd received %d in AUX_BUILD_REQUEST_FROM_CALL_ACTIVE state for connection_id %d",
                     c_call_event,connection_id);
              }

           }
           break;

        }

     case AUX_CALL_IN_MPTY:
        {
           switch(c_call_event)
           {

           case CCE_BUILD_MPTY_REQ:
              {
                 CC_auxiliary_state = AUX_BUILD_REQUEST_FROM_CALL_ACTIVE;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_BUILD_MPTY_CONF:
           case CCE_BUILD_MPTY_REJ:
              {
                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_HOLD_MPTY_REQ:
              {
                 CC_auxiliary_state = AUX_HOLD_MPTY_REQUEST;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_HOLD_MPTY_CONF:
              {
                 CC_auxiliary_state = AUX_MPTY_CALL_HELD;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_SPLIT_MPTY_REQ:
              {
                 CC_auxiliary_state = AUX_SPLIT_REQUEST;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_HOLD_REQ:
              {
                 CC_auxiliary_state = AUX_HOLD_REQUEST;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_ENDED:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_SPLIT_MPTY_REJ:
            /* Event handling added because split reject event can also
             * be passed on to calls within MPTY that are not requesting
             * splitMPTY. For those, just stay in the state where they were
             * instead of going to default: statement and printing error */
              {
                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           default:
              {
                 s_respond_to_event = FALSE;

                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unacceptable cmd received %d in AUX_CALL_IN_MPTY state for connection_id %d",
                   c_call_event,connection_id);
              }
           }
           break;
        }

     case AUX_SPLIT_REQUEST:
        {
           switch(c_call_event)
           {

           case CCE_SPLIT_MPTY_REJ:
              {
                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_SPLIT_MPTY_CONF:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_HOLD_CONF:
              {

                 Multicall_index = MN_whereami(connection_id);
                 if(Multicall_index >= MULTICALL_STATUS_SIZE)
                 {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
                 }
                 else if(Multicall_Status[Multicall_index].call_is_mpty == TRUE)
                 {
                    CC_auxiliary_state = AUX_MPTY_CALL_HELD;
                 }
                 else
                 {
                    CC_auxiliary_state = AUX_CALL_HELD;
                 }

                 s_respond_to_event=TRUE;

                 break;
              }

           case CCE_CALL_ENDED:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           default:
              {
                 s_respond_to_event = FALSE;

                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unacceptable cmd received %d in AUX_SPLIT_REQUEST state for connection_id %d",
                   c_call_event,connection_id);
              }

           }

           break;
        }

     case AUX_HOLD_REQUEST:
        {
           switch(c_call_event)
           {

           case CCE_CALL_HOLD_CONF:
              {
                 CC_auxiliary_state = AUX_CALL_HELD;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_HOLD_REJ:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_ENDED:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_SPLIT_MPTY_CONF:
              {
                 CC_auxiliary_state = AUX_CALL_HELD;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_SPLIT_MPTY_REJ:
              {
                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }
           default:
              {
                 s_respond_to_event = FALSE;

                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unacceptable cmd received %d in AUX_HOLD_REQUEST state for connection_id %d",
                     c_call_event,connection_id);
              }

           }

           break;

        }

     case AUX_HOLD_MPTY_REQUEST:
        {
           switch(c_call_event)
           {
           case CCE_HOLD_MPTY_REJ:
              {
                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_HOLD_MPTY_CONF:
              {
                 CC_auxiliary_state = AUX_MPTY_CALL_HELD;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_ENDED:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_SPLIT_MPTY_CONF:
              {
                 CC_auxiliary_state = AUX_MPTY_CALL_HELD;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_SPLIT_MPTY_REJ:
              {
                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }
           default:
              {
                 s_respond_to_event = FALSE;

                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unacceptable cmd received %d in AUX_HOLD_MPTY_REQUEST state for connection_id %d",
                     c_call_event,connection_id);
              }

           }
           break;

        }

     case AUX_CALL_HELD:
        {
           switch(c_call_event)
           {

           case CCE_BUILD_MPTY_REQ:
              {
                 CC_auxiliary_state = AUX_BUILD_REQUEST_FROM_CALL_HELD;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_BUILD_MPTY_CONF:
              {
                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_RETRIEVE_REQ:
              {
                 CC_auxiliary_state = AUX_RETRIEVE_REQUEST;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_ENDED:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }


           default:
              {
                 s_respond_to_event = FALSE;

                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unacceptable cmd received %d in AUX_CALL_HELD state for connection_id %d",
                     c_call_event,connection_id);
              }

           }

           break;

        }

     case AUX_BUILD_REQUEST_FROM_CALL_HELD:
        {
           switch(c_call_event)
           {
           case CCE_BUILD_MPTY_CONF:
              {

                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_BUILD_MPTY_REJ:
              {
                   /* Activate calls session */
                 Multicall_index = MN_whereami(connection_id);
                 if(Multicall_index >= MULTICALL_STATUS_SIZE)
                 {
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
                 }
                 else if(Multicall_Status[Multicall_index].call_is_mpty == TRUE)
                 {
                     CC_auxiliary_state = AUX_MPTY_CALL_HELD;
                 }
                 else
                 {
                     CC_auxiliary_state = AUX_CALL_HELD;
                 }

                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_ENDED:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           default:
              {

                 s_respond_to_event = FALSE;

                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unacceptable cmd received %d in AUX_BUILD_REQUEST_FROM_CALL_HELD state for connection_id %d",
                     c_call_event,connection_id);

              }

           }
           break;

        }

     case AUX_MPTY_CALL_HELD:
        {
           switch(c_call_event)
           {

           case CCE_RETRIEVE_MPTY_REQ:
              {
                 CC_auxiliary_state = AUX_RETRIEVE_MPTY_REQUEST;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_ENDED:
              {
                 CC_auxiliary_state = AUX_CALL_HELD;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_BUILD_MPTY_REQ:
              {
                 CC_auxiliary_state = AUX_BUILD_REQUEST_FROM_CALL_HELD;
                 s_respond_to_event = TRUE;
                 break;
              }

           case CCE_BUILD_MPTY_CONF:
              {
                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           default:
              {
                 s_respond_to_event = FALSE;

                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unacceptable cmd received %d in AUX_MPTY_CALL_HELD state for connection_id %d",
                     c_call_event,connection_id);
              }
           }
           break;
        }

     case AUX_RETRIEVE_REQUEST:
        {

           switch(c_call_event)
           {

           case CCE_CALL_RETRIEVE_REJ:
              {
                 CC_auxiliary_state = AUX_CALL_HELD;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_RETRIEVE_CONF:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_ENDED:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }


           default:
              {

                 s_respond_to_event = FALSE;

                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unacceptable cmd received %d in AUX_RETRIEVE_REQUEST state for connection_id %d",
                     c_call_event,connection_id);
              }

           }

           break;

        }

     case AUX_RETRIEVE_MPTY_REQUEST:
        {

           switch(c_call_event)
           {

           case CCE_RETRIEVE_MPTY_REJ:
              {
                 CC_auxiliary_state = AUX_MPTY_CALL_HELD;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_RETRIEVE_MPTY_CONF:
              {
                 CC_auxiliary_state = AUX_CALL_IN_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }

           case CCE_CALL_ENDED:
              {
                 CC_auxiliary_state = AUX_IDLE_HOLD_AND_MPTY;
                 s_respond_to_event=TRUE;
                 break;
              }


           default:
              {
                 s_respond_to_event = FALSE;

                 MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unacceptable cmd received %d in AUX_RETRIEVE_MPTY_REQUEST state for connection_id %d",
                     c_call_event,connection_id);
              }
           }
           break;

        }

     default:
        {
           s_respond_to_event = FALSE;
          
           MSG_ERROR_DS_2(MN_SUB,"=MNCNM= ERROR: Unexpected CC auxilliary state %d for connection_id %d",
             CC_auxiliary_state,connection_id);

        }

     }

     /* End of switch function */

     /* re-constitute the CC auxiliary states */

     /* corrected compile warnings C4244 - conversion from int to byte */
   
     cc_transaction_information_p[connection_id]->mpty_auxiliary_state =
         (byte)(CC_auxiliary_state & 3);
     cc_transaction_information_p[connection_id]->hold_auxiliary_state =
         (byte)((CC_auxiliary_state>>2)&3);
   }
   else
   {
     s_respond_to_event = FALSE;
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cc_transaction_information_p[%d] is NULL",connection_id);  

   }
  }
  else /* MAXNO_CC_TRANSACTIONS */
  {
       s_respond_to_event = FALSE;
          
       ERR("ERROR: Invalid connection_id %d",connection_id, 0, 0);
  }

  return(s_respond_to_event);

}

#ifdef FEATURE_UUS

/*
 *   Function name : MN_alloc_user_data()
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Allocate the user to user data request buffer.
 *
 *   Parameters:
 *   -----------
 *  
 *
 *   Returns:
 *   -----------
 *    
 *
 */
cm_mo_user_data_req_T * MN_alloc_user_data()
{
   return (cm_mo_user_data_req_T *) MN_q_get((MN_q_T*) &mn_user_user_data_q[USER_USER_DATA_Q_ID]); 
}

/*
 *   Function name : MN_free_user_data()
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Put the user to user request back to available pool.
 *
 *   Parameters:
 *   -----------
 *   cm_mo_user_data_req_T *buffer.
 *
 *   Returns:
 *   -----------
 *   None. 
 *
 */
void MN_free_user_data(cm_mo_user_data_req_T *user_data)
{
    int index;

    /* check for valid user user data pool */    
    if( (user_data >= &mn_user_user_data_buffers[0]) &&
        (user_data < &mn_user_user_data_buffers[MN_MAXNO_USER_USER_DATA+1]) )
    {
        /* get index to the user data buffer pool */
        index = user_data - mn_user_user_data_buffers;

        if( (index <= MN_MAXNO_USER_USER_DATA) &&
            (user_data == &mn_user_user_data_buffers[index]) )   /* buffer alignment */
        {
            /* clean the buffer */
            memset(&mn_user_user_data_buffers[index], NULL, sizeof(cm_mo_user_data_req_T));
 
            /* Always link before put item back to Q */
            (void)MN_q_link(&mn_user_user_data_buffers[index], &mn_user_user_data_pool[index]);

            /* put it back to user data pool */
            MN_q_put(&mn_user_user_data_q[USER_USER_DATA_Q_ID],  &mn_user_user_data_pool[index]);
        }
        else
        {
            MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid User-User Data = %x, Index = %d", user_data, index);
        }
    } 
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid User-User Data = %x", user_data);
    }
}


/*
 *   Function name : MN_get_user_data()
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Get the user to user data request for a given connection ID.
 *
 *   Parameters:
 *   -----------
 *   connection_id_T   connection_id 
 *
 *   Returns:
 *   -----------
 *   cm_mo_user_data_req_T *user_user_data
 *
 */
cm_mo_user_data_req_T  *MN_get_user_data(connection_id_T connection_id)
{
    cm_mo_user_data_req_T *user_data;

    if(connection_id <= MAX_MO_CONNECTION_ID)
    {
        user_data = (cm_mo_user_data_req_T*) MN_q_get((MN_q_T*) &mn_user_user_data_q[connection_id]);
    }
    else
    {
        user_data = NULL;
    }

    return user_data;
}

/*
 *   Function name : MN_put_user_data()
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Queue the user to user data request for a given connection id.
 *
 *   Parameters:
 *   -----------
 *  
 *
 *   Returns:
 *   -----------
 *    
 *
 */
void  MN_put_user_data(connection_id_T connection_id, cm_mo_user_data_req_T *user_data)
{
    int index;

    /* check for valid user user data pool */    
    if( (user_data >= &mn_user_user_data_buffers[0]) &&
        (user_data < &mn_user_user_data_buffers[MN_MAXNO_USER_USER_DATA+1]) )
    {
        index = user_data - mn_user_user_data_buffers;

        if( (connection_id <= MAX_MO_CONNECTION_ID) &&
        (index <= MN_MAXNO_USER_USER_DATA) &&
            (user_data == &mn_user_user_data_buffers[index]) )   /* buffer alignment */
        {
            /* Always link before put item back to Q */
            (void)MN_q_link(&mn_user_user_data_buffers[index], &mn_user_user_data_pool[index]);

            /* Queue item for given conneciton ID */
            MN_q_put(&mn_user_user_data_q[connection_id], &mn_user_user_data_pool[index]);
        }
        else
        {
            MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid conn id = %d, or user data = %x", connection_id, user_data);
        }
    }
    else
    {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid User-User Data = %x", user_data);
    }
}

#endif

