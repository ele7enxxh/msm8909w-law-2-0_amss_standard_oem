/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_get_cnm_cc_msgs.c_v   1.10   10 Jul 2002 13:31:48   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_get_cnm_cc_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/3/01      CD     Corrected compile warnings C4244 - conversion from int to byte

5/10/01     CD     Removed unnecessary header file includes

5/21/01     CD     Removed #include for cause.h and ms.h as they were not being used
                   (lint 766)

                   Removed 'null_ptr' in MN_get_MNCC_MODIFY_CNF since it was not 
                   accessed (lint 550)

6/5/01      CD     Removed sequence_length from MN_get_MNCC_FACILITY_IND() since
                   it was not being accessed

7/18/01     CD     Removed duplicate external declaration of mn_call_information

8/10/01     CD     Removed unnecessary queue.h.

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

12/12/01    AB     Added logic in MN_get_MNCC_FACILITY_IND() to handle processing of 
                   optional OP_CODE_TAG in RETURN_RESULT Components.  Plus, check for 
                   SET_TAG in addition to SEQUENCE_TAG.

23-Jan-2002  VT    In MN_get_MNCC_SETUP_IND(), added a MSG_HIGH in case of 
                     IEI_PROGRESS_INDICATOR.

01/25/02    CD     Remove IEI and added boolean flag
                   Updated Copyright
                   Replaced calls to ERR() by MSG_ERROR()
                   Fixed negative indentation Lint errors

05/31/02     CD    Changed calls to MN_get_bearer_capability_data() to pass in
                   the connection_id 

06/25/02     CD    In MN_get_MNCC_REL_IND() corrected processing of the 
                   two cause IE that may come in a RELEASE message

06/28/02     CD    MN_get_MNCC_REL_CNF() and MN_get_MNCC_REJ_IND() corrected 
                   overwriting the OTA cause value with a local cause value. 

07/02/02     CD    Added processing of cause_of_no_CLI to MN_get_MNCC_SETUP_IND()

09/04/02     AB    In MN_get_MNCC_SETUP_IND(), copy the Redirect BCD IE 
                   and/or Redirect Subaddress IE if any, to the 
                   MN_call_infomration[] global structure.

09/13/02     CD    In MN_get_MNCC_REL_CNF(), and MN_get_MNCC_REJ_IND()
                   corrected reading of cause data based on the msg length

02/12/02     AB    Removed lint errors.

03/24/05     HS    Fixed IEI_CONNECTED_SUBADDRESS handling error in
                   MN_get_MNCC_SETUP_CNF()

05/02/05     HS    Fixed RETURN_RESULT case in MN_get_MNCC_FACILITY_IND() to 
                   correctly calulate parameter length

05/10/05     HS    Added support for FEATURE_CCBS

11/04/06     NR    Adding support for LSM CR88759

02/02/07     NR    Removing the incrementing, when Reapeat Indicator is 1

06/18/07     RD    BCIE 2 is included even when repeat indicator is 1
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "memory.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"

#include "nasutils.h"



/*
 * Function name :  MN_get_MNCC_CALL_PROC_IND
 * -------------
 *
 * Description :  Decode message - acceptance of a MO call
 * -----------     setup by the network
 *
 *
 *
 *
 *
 *
 * Parameters: message_ptr
 * ----------  MNCC_CALL_PROC_data_T *
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC MN_get_MNCC_CALL_PROC_IND(MNCC_CALL_PROC_IND_T *message_ptr, 
MNCC_CALL_PROC_data_T *proc_data_ptr)
{
   int         message_position;
   int         message_length;
   int         data_index;
   int         index;
   byte        *data_ptr;
   byte        *last_octet_data_ptr;


   memset(proc_data_ptr, NULL, sizeof(MNCC_CALL_PROC_data_T));
   
   /*  LOAD THE CONNECTION ID  */
   proc_data_ptr->connection_id = message_ptr->connection_id;

   message_length = ( (message_ptr->message_header.message_len_lsb) +
       (256 *(message_ptr->message_header.message_len_msb)) );

   data_index = 0;

   /*  SET MESSAGE POSITION TO START OF VARIABLE DATA  */
   message_position = sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type);

   /*  SEE IF THERE IS ANY MORE DATA  */
   proc_data_ptr->repeat_indication_bc = 0;

   if(message_position < message_length)
   {
      /*  SEE IF ONLY ONE BEARER CAPABILITY IE IS PRESENT  */
      if(message_ptr->data[data_index] == IEI_BEARER_CAPABILITY)
      {

         data_ptr = &(message_ptr->data[data_index]);
         last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);

         index = MN_get_bearer_capability_data(data_ptr, 
                    &proc_data_ptr->bearer_capability_1, 
                    proc_data_ptr->connection_id,
                    last_octet_data_ptr);

         data_index += index;
         message_position += index;

         proc_data_ptr->repeat_indication_bc = 1;

#ifdef  FEATURE_MULTIMEDIA_FALLBACK               
         #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
      
      }

      else
      {
         /*  SEE IF TWO BEARER CAPABILITIES ARE PRESENT  */
         if((message_ptr->data[data_index] & 0xf0) == IEI_REPEAT_INDICATOR)
         {
             /* Corrected compile warning C4244 - conversion from int to byte */
             
            proc_data_ptr->repeat_indication_bc = 
                (byte)(message_ptr->data[data_index] & 0x0f);

            message_position++;
            data_index++;


            /*  GET THE FIRST BEARER CAPABILITY  */
            if(message_ptr->data[data_index] == IEI_BEARER_CAPABILITY)
            {

               data_ptr = &(message_ptr->data[data_index]);
               last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);
               index = MN_get_bearer_capability_data(data_ptr,
                          &proc_data_ptr->bearer_capability_1, 
                          proc_data_ptr->connection_id,
                          last_octet_data_ptr);

               data_index += index;
               message_position += index;

               /*  GET THE SECOND BEARER CAPABILITY  */
               if((message_ptr->data[data_index]) == IEI_BEARER_CAPABILITY)
               {

                  data_ptr = &(message_ptr->data[data_index]);
                  last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);
                  index = MN_get_bearer_capability_data(data_ptr,
                             &proc_data_ptr->bearer_capability_2, 
                             proc_data_ptr->connection_id,
                             last_octet_data_ptr);

                  data_index += index;
                  message_position += index;

                  
                  if(message_position != message_length)
                  {
                     MSG_LOW_DS_0(MN_SUB,"=MNCNM= MNCC_CALL_PROC_IND message_length");
                  }
               }
               else
               {
                   MSG_LOW_DS_0(MN_SUB,"=MNCNM= IEI in MNCC_CALL_PROC_IND message - 2nd BC expected");
               } /* BC 2 */
            }
            else
            {
                MSG_LOW_DS_0(MN_SUB,"=MNCNM= IEI in MNCC_CALL_PROC_IND message - 1st BC expected");
            } /* BC 1 */

#ifdef  FEATURE_MULTIMEDIA_FALLBACK               
            #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

         }
         else
         {
             MSG_LOW_DS_0(MN_SUB,"=MNCNM= IEI in MNCC_CALL_PROC_IND message");
         }
      }
   }
}


#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
/*
 * Function name :  MN_get_MNCC_MODIFY_CONF
 * -------------
 *
 * Description :  Decode message - network notification of a
 * -----------     MO modify request
 *
 *
 *
 *
 *
 *
 * Parameters: message_ptr
 * ----------  modify_data_pointer
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_get_MNCC_MODIFY_CNF(MNCC_MODIFY_CNF_T *message_ptr, 
MNCC_MODIFY_data_T *modify_data_ptr)
{
   int message_position;
   int message_length;
   byte *data_ptr;
   byte * last_octet_data_ptr;

   /* Initialise the output data area */

   memset((VOID_DATA  *)modify_data_ptr,'\0',sizeof(MNCC_MODIFY_data_T));

   /*  DECODE MNCC_MODIFY_CNF - NETWORK NOTIFICATION OF A MO MODIFY REQUEST  */

   message_length = message_ptr->message_header.message_len_lsb +
       (256 *(message_ptr->message_header.message_len_msb));

   message_position = sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type);  /*  Size of connection ID + as_id  */

   modify_data_ptr->connection_id = message_ptr->connection_id;


   data_ptr = &(message_ptr->data[0]);
   last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);

   MN_get_MNCC_MODIFY_data(message_position, message_length,
       data_ptr, modify_data_ptr, last_octet_data_ptr);

   /*  THIS RETURNS A COMPLETE MNCC_MODIFY_data STRUCTURE  */

}
#endif /* FEATURE_MULTIMEDIA_FALLBACK */


/*
 * Function name :  MN_get_MNCC_REJ_IND
 * -------------
 *
 * Description :  Decode message - indication of a rejection
 * -----------     of an MO call request
 *
 *
 *
 * Parameters: message_ptr
 * ----------  cause_data_ptr
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_get_MNCC_REJ_IND(MNCC_REJ_IND_T    *message_ptr, 
                              ie_cm_cc_cause_T *cause_data_ptr)
{
   int message_length;


   /* DECODE MNCC_REJ_IND - INDICATION OF A REJECTION IF AN MO CALL REQUEST  */

   message_length = message_ptr->message_header.message_len_lsb +
       (256 * message_ptr->message_header.message_len_msb);

   /* 
   * If the message length is greater than 2,  the cause data is included
   * in this message 
   */
   if(message_length > 2)
   {
      /*  Load the OTA cause into cm_cause data; if absent, only
           flushing to nulls occurs */

       (void)MN_get_cause_data(&message_ptr->data[0], cause_data_ptr, (&message_ptr->data[0] + sizeof(message_ptr->data) - 1));

   }

   else
   {
      cause_data_ptr->present = FALSE;
   }

}


/*
 * Function name :  MN_get_MNCC_REL_CNF
 * -------------
 *
 * Description : Confirms the releasse of a call connection to CC 
 * -----------     
 *
 *
 *
 * Parameters: message_ptr
 * ----------  cm_cc_cause_ptr
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_get_MNCC_REL_CNF(MNCC_REL_CNF_T *message_ptr, 
                              ie_cm_cc_cause_T *cm_cc_cause_ptr)
{
   int message_length;


   message_length = message_ptr->message_header.message_len_lsb +
       (256 * message_ptr->message_header.message_len_msb);

   /* 
   * If the message length is greater than 2,  the cause data is included
   * in this message 
   */
   if(message_length > 2)
   {

      /*  Load the OTA cause into cm_cause data; if absent, only
           flushing to nulls occurs */

      (void)MN_get_cause_data(&message_ptr->data[0], cm_cc_cause_ptr, (&message_ptr->data[0] + sizeof(message_ptr->data) -1));

   }
   else
   {
      cm_cc_cause_ptr->present = FALSE;
   }

}

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
/*
 * Function name :MN_get_MNCC_MODIFY_data  
 * -------------
 *
 * Description :  Gets data from message data array and put in 
 * -----------     to relevant data structure
 *
 *
 *
 *
 *
 *
 * Parameters: message_position
 * ----------  message_length
 *             data_ptr   -    to message_data_array
 *             MNCC_MODIFY_data_T *
 *
 * Returns: index   -   to message data array
 * --------
 *
 */

VOID_FUNC MN_get_MNCC_MODIFY_data(int message_position, 
int message_length,
byte *data_ptr, 
MNCC_MODIFY_data_T *modify_data_ptr,
byte * last_octet_data_ptr)
{
   int index;


   while(message_position < message_length)
   {
      switch(*data_ptr)
      {
      case IEI_CAUSE:
         {
            modify_data_ptr->cause_present = TRUE;
            index = 
                MN_get_cause_data(data_ptr, &(modify_data_ptr->cm_cc_cause), last_octet_data_ptr);

            data_ptr += (index);
            message_position += (index);
            break;
         }

      case IEI_BEARER_CAPABILITY:
         {
            index = MN_get_bearer_capability_data(data_ptr, 
                &(modify_data_ptr->cm_bearer_capability), 
                modify_data_ptr->connection_id,
                last_octet_data_ptr);

            data_ptr += (index);
            message_position += (index);
            break;
         }

      case IEI_LOW_LAYER_COMPATIB:
         {
            index = MN_get_llc_data(data_ptr, 
                &(modify_data_ptr->cm_low_layer_compatibility),
                last_octet_data_ptr);

            data_ptr += (index);
            message_position += (index);
            break;
         }

      case IEI_HIGH_LAYER_COMPATIB:
         {
            index = MN_get_hlc_data(data_ptr, 
                &(modify_data_ptr->cm_high_layer_compatibility),
                last_octet_data_ptr);

            data_ptr += (index);
            message_position += (index);
            break;
         }

      case IEI_REVERSE_CALL_SETUP_DIR:
         {
            if(modify_data_ptr->reverse_call_direction == TRUE)
            {
               modify_data_ptr->reverse_call_direction = FALSE;
            }

            else
            {
               modify_data_ptr->reverse_call_direction = TRUE;
            }

            data_ptr++;
            message_position += 1;
            break;
         }

      case IEI_IMMEDIDATE_MOD_IND:
         {

            modify_data_ptr->immediate_modify_ind = TRUE;

            data_ptr++;
            message_position += 1;
            break;
         }

      case IEI_NW_INITIATED_SVC_UPGRADE_IND:
         {

            modify_data_ptr->nw_initiated_ind = TRUE;

            data_ptr++;
            message_position += 1;
            break;
         }

      default:
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected IEI in MNCC_MODIFY_data message"); 

            message_position = message_length;
            break;
         }
      }
   }

}
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

/***************************************************************
*                           *
*                    MN_get_MNCC_REL_IND            *
*                           *
****************************************************************/
/*
 * Function name :  MN_get_MNCC_REL_IND
 * -------------
 *
 * Description :  Decode message - network is releasing the connection
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: message_ptr
 * ----------  cause_1
 *             cause_2
 *
 *
 * Returns: no_of_causes  (int)
 * --------
 *
 */
int
MN_get_MNCC_REL_IND(MNCC_REL_IND_T *message_ptr, 
ie_cm_cc_cause_T *cause_data_ptr_1,   
ie_cm_cc_cause_T *cause_data_ptr_2)
{
   int message_length;
   int message_position;
   int no_of_causes;
   int data_index;
   int index;
   ie_cm_cc_cause_T *cause_ptr;

   /*  DECODE AN MNCC_REL_IND MESSAGE - THE NETWORK IS RELEASING THE CONNECTION  */

   no_of_causes = 0;

   if (message_ptr->connection_id >= MAXNO_CC_TRANSACTIONS)
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Connection id invalid for MNCC_REL_IND");
   }

   else
   {
      GET_IMH_LEN(message_length, &(message_ptr->message_header));

      data_index = 0;

      /*  SET MESSAGE POSITION TO START OF VARIABLE DATA  */

      message_position = sizeof(message_ptr->connection_id) +
          sizeof(message_ptr->cause) + sizeof(sys_modem_as_id_e_type);

      /*  SEE IF THERE ARE OPTIONAL CAUSE IEs  */
      /*  IF THE MNCC_REL_IND IS THE RESULT OF */
      /*  A RELEASE MSG,  THERE COULD BE 2     */
      /*  CAUSE IEs                            */

      while((message_position < message_length) && (no_of_causes < 2))
      {
          /* Klocwork fix*/ 
          if(data_index >=0 && data_index < (2*GSM_CAUSE_SIZE))
          {
              if (message_ptr->data[data_index] == IEI_CAUSE)
              {
                  no_of_causes++;
                  if (no_of_causes == 1)
                  {
                      cause_ptr = cause_data_ptr_1;
                  }
                  else
                  {
                      cause_ptr = cause_data_ptr_2;
                  }

                  index = MN_get_cause_data(&(message_ptr->data[data_index]), cause_ptr, 
                                         &(message_ptr->data[sizeof(message_ptr->data) -1]));

                  data_index += index;
                  message_position += index;
              }
              else
              {
                  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Unexpected cause IEI %#x in MNCC_REL_IND message",
                                    message_ptr->data[data_index]);
    
                  message_position = message_length;
              }
          }
          else
          {
              ERR("ERROR: Buffer Overflow",0,0,0);
          }
      }
   }

   return (no_of_causes);
}


/***************************************************************
*                           *
*                    MN_get_MNCC_SETUP_CNF             *
*                           *
****************************************************************/
/*
 * Function name :  MN_get_MNCC_SETUP_CNF
 * -------------
 *
 * Description :  Network has confirmed setup of connection - 
 * -----------    get the setup data 
 *
 *
 *
 *
 *
 *
 * Parameters: message_ptr
 * ----------  
 *
 *
 *
 * Returns:    connection_id
 * --------
 *
 */
connection_id_T
MN_get_MNCC_SETUP_CNF(MNCC_SETUP_CNF_T *message_ptr)
{
   int data_index;
   int index;
   mn_call_information_T *call_data_ptr;
   connection_id_T connection_id;
   int message_length;
   int message_position;


   message_length = message_ptr->message_header.message_len_lsb +
       (256 * message_ptr->message_header.message_len_msb);
   data_index = 0;

   /*  SET MESSAGE POSITION TO START OF VARIABLE DATA  */

   message_position = sizeof(message_ptr->connection_id) + sizeof(sys_modem_as_id_e_type);

   connection_id = message_ptr->connection_id;

   /*  LOCATE THE CALL STORAGE  */
   if(mn_call_information_p[connection_id] != NULL)
   {
     call_data_ptr = mn_call_information_p[connection_id];
   
     call_data_ptr->connected_number_present = FALSE;
     call_data_ptr->connected_subaddress_present = FALSE;

     /*  SEE IF THERE IS MORE DATA  */

     if(message_position < message_length)
     {
        /*  GENERATE IEI  */

        if(message_ptr->data[data_index] == IEI_CONNECTED_NUMBER)
        {
           call_data_ptr->connected_number_present = TRUE;
           data_index++;
           message_position += message_ptr->data[data_index] + 2;
           call_data_ptr->connected_number.length = message_ptr->data[data_index++];
           if((call_data_ptr->connected_number.length > (byte)CM_CONNECTED_NO_LENGTH))
           {
             MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
           }
           else
           {
             for(index = 0; index < call_data_ptr->connected_number.length; index++)
             {
               call_data_ptr->connected_number.data[index] =
                   message_ptr->data[data_index++];
             }
           }
        }

        if(message_position < message_length)
        {
           /*  GET LAST POSSIBLE IE  */
           if(message_ptr->data[data_index] == IEI_CONNECTED_SUBADDRESS)
           {
             call_data_ptr->connected_subaddress_present = TRUE;
             data_index++;
             message_position += message_ptr->data[data_index] + 2;
             call_data_ptr->connected_subaddress.length = message_ptr->data[data_index++];
             for(index = 0; index < call_data_ptr->connected_subaddress.length; index++)
             {
               call_data_ptr->connected_subaddress.data[index] =
                   message_ptr->data[data_index++];
             }
           }
        }
        if(message_position != message_length)
        {
          MSG_MED_DS_0(MN_SUB,"=MNCNM= MNCC_SETUP_CNF message");
          message_position = message_length;
        }
     }
   }
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= cannot process MNCC_SETUP_CNF as mn_call_information_p[%d] is NULL",
                   connection_id);
   }
   return connection_id;
}

/*
 * Function name :MN_get_MNCC_SETUP_IND  
 * -------------
 *
 * Description : Get call setup data and send indication to CC 
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: message_ptr
 * ----------  
 *
 *
 *
 * Returns:    connection_id
 * --------
 *
 */
connection_id_T
MN_get_MNCC_SETUP_IND(MNCC_SETUP_IND_T *message_ptr)
{
   mn_call_information_T *call_data_ptr;
   int index, data_index;
   int message_position;
   int message_length,ie_length;
   byte *data_ptr;
   byte *last_octet_data_ptr;
   cm_bearer_capability_T cm_bearer_capability;
   cm_low_layer_compatibility_T cm_low_layer_compatibility;
   cm_high_layer_compatibility_T cm_high_layer_compatibility;


   /*  DECODE AN MNCC_SETUP_IND MESSAGE  */

   message_length = message_ptr->message_header.message_len_lsb +
       (256 * message_ptr->message_header.message_len_msb);
   data_index = 0;

   /*  SET MESSAGE POSITION TO START OF VARIABLE DATA  */

   message_position = (int)sizeof(message_ptr->connection_id) + sizeof(sys_modem_as_id_e_type);
   if(mn_call_information_p[message_ptr->connection_id] != NULL)
   {
     call_data_ptr = mn_call_information_p[message_ptr->connection_id];
  
     /*  SET ALL `DATA PRESENT' INDICATORS TO ZERO  */
  
     call_data_ptr->repeat_indication_llc = 0;
     call_data_ptr->repeat_indication_hlc = 0;
     call_data_ptr->repeat_indicator_bc = 0;
     call_data_ptr->calling_party_bcd_number_present = FALSE;
     call_data_ptr->calling_party_subaddress_present = FALSE;
     call_data_ptr->called_party_subaddress_present = FALSE;
     call_data_ptr->called_party_bcd_number_present = FALSE;
     call_data_ptr->signal_present = FALSE;
     call_data_ptr->progress_indicator_present = FALSE;
     call_data_ptr->cause_of_no_CLI.present = FALSE;
  
     /*  SEE IF THERE IS MORE DATA  */
  
     while(message_position < message_length)
     {
        switch(message_ptr->data[data_index])
        {
        case IEI_BEARER_CAPABILITY:
           {
              /*  ONE BEARER CAPABILITY  */
              data_ptr = &(message_ptr->data[data_index]);
              last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);
              index = MN_get_bearer_capability_data(data_ptr, &cm_bearer_capability,
                                                    message_ptr->connection_id,
                                                    last_octet_data_ptr);
  
              call_data_ptr->bearer_capability_1 = cm_bearer_capability;
              data_index += index;
              message_position += index;
              call_data_ptr->repeat_indicator_bc = 1;
              break;
           }
  
        case IEI_PROGRESS_INDICATOR:
           {
              /*  ONE PROGRESS INDICATOR PRESENT  */
              data_index++;
              message_position += message_ptr->data[data_index++] + 2;
  
              /* corrected compile warning C4244 conversion from int to byte */
              call_data_ptr->progress_indicator.coding_standard = 
                  (byte)((message_ptr->data[data_index] & 0x60) >> 5);
              call_data_ptr->progress_indicator.location = 
                  (byte)(message_ptr->data[data_index++] & 0x0f);
  
              call_data_ptr->progress_indicator.progress_description = 
                  message_ptr->data[data_index++];
              call_data_ptr->progress_indicator_present = TRUE;
              MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Progress Indicator received in MNCC_SETUP_IND from CNM");
              break;
           }
  
        case IEI_SIGNAL:
           {
              /*  ONE SIGNAL VALUE PRESENT  */
              data_index++;
              message_position += 2;
              call_data_ptr->SignalValue = message_ptr->data[data_index++];
              call_data_ptr->signal_present = TRUE;
              break;
           }
  
        case IEI_CALLING_PARTY_BCD_NO:
           {
              /*  ONE CALLING PARTY BCD NUMBER PRESENT  */
  
              data_index++;
  
              ie_length = message_ptr->data[data_index++];
  
              call_data_ptr->calling_party_bcd_number.length = (byte)ie_length;
  
              message_position += (ie_length + 2);
  
              for(index = 0; index < ie_length;index++)
  
              {
                 call_data_ptr->calling_party_bcd_number.data[index] = 
                     message_ptr->data[data_index++];
              }
              call_data_ptr->calling_party_bcd_number_present = TRUE;
              break;
           }
  
        case IEI_CALLING_PARTY_SUBAD:
           {
  
              /*  ONE CALLING PARTY SUBADDRESS PRESENT  */
  
              data_index++;
  
              ie_length =  message_ptr->data[data_index++];
  
              /* fixed compile warning C4244 conversion from int to byte */
              /*
              call_data_ptr->calling_party_subaddress.length = (int)ie_length;
              */
              call_data_ptr->calling_party_subaddress.length = (byte)ie_length;
  
  
              message_position += (ie_length + 2);
  
              for(index = 0; index < ie_length;index++)
  
              {
                 call_data_ptr->calling_party_subaddress.data[index] =
                     message_ptr->data[data_index++];
              }
              call_data_ptr->calling_party_subaddress_present = TRUE;
              break;
           }
  
        case IEI_REDIR_PARTY_BCD_NO:
           {
              /*  ONE CALLING PARTY BCD NUMBER PRESENT  */
  
              data_index++;
  
              ie_length = message_ptr->data[data_index++];
              if(ie_length > CM_CALLING_PARTY_BCD_NO_LENGTH)
              {
                call_data_ptr->redirect_party_bcd_number.length = CM_CALLING_PARTY_BCD_NO_LENGTH;
              }
              else
              {
                call_data_ptr->redirect_party_bcd_number.length = (byte)ie_length;
              }
              message_position += (ie_length + 2);
  
              for(index = 0; index < ie_length;index++,data_index++)
              {
                if(index < CM_CALLING_PARTY_BCD_NO_LENGTH)
                {
                  call_data_ptr->redirect_party_bcd_number.data[index] = message_ptr->data[data_index];
                }
              }
  
              call_data_ptr->redirect_party_bcd_number_present = TRUE;
  
              break;
           }
  
        case IEI_REDIR_PARTY_SUBADDR:
           {
  
              /*  ONE CALLING PARTY SUBADDRESS PRESENT  */
  
              data_index++;
  
              ie_length =  message_ptr->data[data_index++];
  
              /* fixed compile warning C4244 conversion from int to byte */
              /*
              call_data_ptr->calling_party_subaddress.length = (int)ie_length;
              */
              call_data_ptr->redirect_party_subaddress.length = (byte)ie_length;
  
              message_position += (ie_length + 2);
  
              for(index = 0; index < ie_length;index++)
  
              {
                 call_data_ptr->redirect_party_subaddress.data[index] =
                     message_ptr->data[data_index++];
              }
  
              call_data_ptr->redirect_party_subaddress_present = TRUE;
  
              break;
           }
  
        case IEI_CALLED_PARTY_BCD_NO:
  
           {
              /*  ONE CALLED PARTY BCD NUMBER PRESENT  */
  
              data_index++;
  
              ie_length =  message_ptr->data[data_index++];
  
              call_data_ptr->called_party_bcd_number.length = (byte)ie_length;
  
              message_position += (ie_length + 2);
  
              for(index = 0; index < ie_length;index++)
  
              {
                 call_data_ptr->called_party_bcd_number.data[index] = 
                     message_ptr->data[data_index++];
              }
  
              call_data_ptr->called_party_bcd_number_present = TRUE;
              break;
  
           }
  
        case IEI_CALLED_PARTY_SUBAD:
           {
  
              /*  ONE CALLED PARTY SUBADDRESS PRESENT  */
  
              data_index++;
  
              ie_length =  message_ptr->data[data_index++];
  
              /* fixed compile warning C4244 conversion from int to byte */
              /*
              call_data_ptr->called_party_subaddress.length = (int)ie_length;
              */
              call_data_ptr->called_party_subaddress.length = (byte)ie_length;
  
  
              message_position += (ie_length + 2);
  
              for(index = 0; index < ie_length;index++)
              {
                 call_data_ptr->called_party_subaddress.data[index] =
                     message_ptr->data[data_index++];
              }
              call_data_ptr->called_party_subaddress_present = TRUE;
              break;
           }
  
        case IEI_LOW_LAYER_COMPATIB:
  
           {
              /*  ONE LOW LAYER COMPATIBILITY PRESENT  */
  
              call_data_ptr->repeat_indication_llc = 1;
  
              data_ptr = &(message_ptr->data[data_index]);
              last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);

              index = MN_get_llc_data(data_ptr, &cm_low_layer_compatibility, last_octet_data_ptr);
  
  
              call_data_ptr->low_layer_compatibility_1 = cm_low_layer_compatibility;
  
              data_index += index;
  
              message_position += index;
  
              break;
           }
  
        case IEI_HIGH_LAYER_COMPATIB:
           {
  
              /*  ONE HIGH LAYER COMPATIBILITY PRESENT  */
  
              call_data_ptr->repeat_indication_hlc = 1;
  
              data_ptr = &(message_ptr->data[data_index]);
              last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);
  
              index = MN_get_hlc_data(data_ptr, &cm_high_layer_compatibility, last_octet_data_ptr);
  
              call_data_ptr->high_layer_compatibility_1 = cm_high_layer_compatibility;
  
              data_index += index;
  
              message_position += index;
  
              break;
           }
  
        case (IEI_REPEAT_INDICATOR | REPEAT_CIRCULAR):
        case (IEI_REPEAT_INDICATOR | REPEAT_SUPPORTS_FALLBACK):
        case (IEI_REPEAT_INDICATOR | REPEAT_SERVICE_CHANGE_FALLBACK):
           {
              
              message_position++;
              call_data_ptr->repeat_indicator_bc = message_ptr->data[data_index++] & 0x0f;
  
              /*  TWO CONSEQUTIVE DATA ELEMENTS OF THE SAME TYPE ARE PRESENT  */
  
              switch(message_ptr->data[data_index])
  
              {
              case IEI_BEARER_CAPABILITY:
  
                 {
                    /*  TWO BEARER CAPABILITIES PRESENT  */
                    /* fixed compile warning C4244 conversion from int to byte */
                    
                    call_data_ptr->repeat_indicator_bc = 
                        (byte)(message_ptr->data[data_index-1] & 0x0f);
  
                    data_ptr = &(message_ptr->data[data_index]);
                    last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);
                    index = MN_get_bearer_capability_data(data_ptr, &cm_bearer_capability, message_ptr->connection_id, last_octet_data_ptr);
                    call_data_ptr->bearer_capability_1 = cm_bearer_capability;
                    data_index += index;
                    message_position += index;
  
                    data_ptr = &(message_ptr->data[data_index]);
                    index = MN_get_bearer_capability_data(data_ptr, &cm_bearer_capability, message_ptr->connection_id, last_octet_data_ptr);
                    call_data_ptr->bearer_capability_2 = cm_bearer_capability;
                    data_index += index;
                    message_position += index;
                    break;
                 }
  
              case IEI_LOW_LAYER_COMPATIB:
  
                 {
                    /*  TWO LOW LAYER COMPATIBILITIES  */
                    /* fixed compile warning C4244 conversion from int to byte */
                     call_data_ptr->repeat_indication_llc = 
                         (byte)(message_ptr->data[data_index - 1] & 0x0f);
  
                    if(call_data_ptr->repeat_indication_llc == 1)
                    {
                       call_data_ptr->repeat_indication_llc++;
                    }
  
                    data_ptr = &(message_ptr->data[data_index]);
                    last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);
  
                    index = MN_get_llc_data(data_ptr, &cm_low_layer_compatibility, last_octet_data_ptr);
  
                    call_data_ptr->low_layer_compatibility_1 = cm_low_layer_compatibility;
  
                    data_index += index;
  
                    message_position += index;
  
                    data_ptr = &(message_ptr->data[data_index]);
  
                    index = MN_get_llc_data(data_ptr, &cm_low_layer_compatibility, last_octet_data_ptr);
  
                    call_data_ptr->low_layer_compatibility_2 = cm_low_layer_compatibility;
                    data_index += index;
                    message_position += index;
                    break;
  
                 }
  
              case IEI_HIGH_LAYER_COMPATIB:
  
                 {
                    /*  TWO HIGH LAYER COMPATIBILITIES  */
  
                    /* fixed compile warning C4244 conversion from int to byte */
                    call_data_ptr->repeat_indication_hlc = 
                        (byte)(message_ptr->data[data_index - 1] & 0x0f);
  
  
                    if(call_data_ptr->repeat_indication_hlc == 1)
  
                    {
                       call_data_ptr->repeat_indication_hlc++;
                    }
  
                    data_ptr = &(message_ptr->data[data_index]);
                    last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);
  
                    index = MN_get_hlc_data(data_ptr, &cm_high_layer_compatibility, last_octet_data_ptr);
  
                    call_data_ptr->high_layer_compatibility_1 = cm_high_layer_compatibility;
  
                    data_index += index;
  
                    message_position += index;
                    if(data_index < sizeof(message_ptr->data))
                    {
                       data_ptr = &(message_ptr->data[data_index]);
                       index = MN_get_hlc_data(data_ptr, &cm_high_layer_compatibility, last_octet_data_ptr);
                       call_data_ptr->high_layer_compatibility_2 = cm_high_layer_compatibility;
                       data_index += index;
                       message_position += index;
                    }
                    else
                    {
                       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Buffer Overflow happened in MNCC_SETUP_IND message");
                    }
                    break;
                 }
  
              default:
                 {
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Error in MNCC_SETUP_IND message");
  
                    /* Break out of the loop */
  
                    message_position = message_length;
  
                    break;
                 }
                    /*  END THE EMBEDDED SWITCH  */
              }
  
                 /* end of the  case (IEI_REPEAT_INDICATOR + 1):
                             case (IEI_REPEAT_INDICATOR + 3) :
                 */
              break;
           }
  
        case IEI_CAUSE_OF_NO_CLI:
           {
              /*  advance the index by two in order to skip the length byte which is set 
               *  to 1
               */
              data_index += 2;
              message_position += (int)sizeof(ie_cause_of_no_CLI_T);
              call_data_ptr->cause_of_no_CLI.cause_value = message_ptr->data[data_index++];
              call_data_ptr->cause_of_no_CLI.present = TRUE;
              break;
           }

        case IEI_ALERTING_PATTERN:
           {
              /*  advance the index by two in order to skip the length byte which is set 
               *  to 1
               */
              data_index += 2;
              message_position += (int)sizeof(ie_alerting_pattern_T);
              call_data_ptr->alerting_pattern.alerting_pattern_value = message_ptr->data[data_index++];
              call_data_ptr->alerting_pattern.present = TRUE;
              break;
           }
  
  #if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
        case IEI_NETWORK_CC_CAPABILITY: 
            {
              data_index += 2;
              message_position += (int)sizeof(ie_network_CC_CAP_T);
              call_data_ptr->network_cc_capability.capability = message_ptr->data[data_index++];
              call_data_ptr->network_cc_capability.length = 1;
              call_data_ptr->network_cc_capability.present = TRUE;
              break;  
            }
  
        case IEI_BACKUP_BEARER_CAPABILITY:
            {
              data_ptr = &(message_ptr->data[data_index]);
              last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);
              index = MN_get_bearer_capability_data(data_ptr, &cm_bearer_capability, message_ptr->connection_id, last_octet_data_ptr);
              call_data_ptr->backup_bearer_capability = cm_bearer_capability;
              data_index += index;
              message_position += index;
  
              break;
            }
            
  #endif      
  
        default:
  
           {       
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect IEI in MNCC_SETUP_IND");
             
              /* In error case, break out of the main while loop */
  
              message_position = message_length;
  
              break;
           }
  
           /*  END THE MAIN SWITCH  */
        }
     }
   }
   else
   {
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot process MNCC_SET_IND as mn_call_information_p[%d] is NULL",
                           message_ptr->connection_id);
   }
   return (message_ptr->connection_id);

}

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
/*
 * Function name : MN_get_MNCC_MODIFY_IND 
 * -------------
 *
 * Description :  Network wishes to initiate a modification
 * -----------     
 *
 *
 *
 *
 *
 *
 * Parameters: message_ptr
 * ----------  MNCC_MODIFY_data_T *
 *
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_get_MNCC_MODIFY_IND(MNCC_MODIFY_IND_T *message_ptr, 
MNCC_MODIFY_data_T *modify_data_ptr)
{
   int message_position;
   int message_length;

   /*  DECODE AN MNCC_SETUP_IND MESSAGE  */

   message_length = message_ptr->message_header.message_len_lsb +
       (256 * message_ptr->message_header.message_len_msb);

   /*  SET MESSAGE POSITION TO START OF VARIABLE DATA  */

   message_position = sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type);

   /*  SEE IF THERE IS MORE DATA  */

   modify_data_ptr->cause_present = FALSE;
   modify_data_ptr->repeat_indication_llc = 0;
   modify_data_ptr->repeat_indication_hlc = 0;
   modify_data_ptr->reverse_call_direction = FALSE;
   modify_data_ptr->cm_bearer_capability.present = FALSE;
   modify_data_ptr->cm_high_layer_compatibility.present = FALSE;
   modify_data_ptr->cm_low_layer_compatibility.present = FALSE;
   modify_data_ptr->cm_cc_cause.present = FALSE;
   modify_data_ptr->connection_id = message_ptr->connection_id;

   MN_get_MNCC_MODIFY_data(message_position, message_length,
       &message_ptr->data[0], modify_data_ptr, (&message_ptr->data[0] + sizeof(message_ptr->data) -1));

   /* MODIFY DATA STRUCT RETURNED TO MODIFY DATA POINTER  */

}

#endif

/*
 * Function name : MN_get_MNCC_DISC_IND 
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
 * Parameters: message_ptr
 * ----------  cm_cc_cause_ptr
 *             cm_progress_indicator_ptr
 *
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_get_MNCC_DISC_IND(  MNCC_DISC_IND_T *message_ptr, 
ie_cm_cc_cause_T *cm_cc_cause_ptr,
cm_progress_indicator_T *cm_prog_ind_ptr
#ifdef FEATURE_CCBS
,ie_cm_allowed_actions_T *allowed_actions_ptr
#endif /* FEATURE_CCBS */
)
{
   /*  VARIABLE DECLARATIONS  */
   int            message_length;
   int            message_position;
   int            data_index, max_data_index, index;
   byte           *data_ptr;
   byte           *last_octet_data_ptr;

   /*  SET UP VARIABLES  */

   message_length = (message_ptr->message_header.message_len_lsb) +
       (256 * (message_ptr->message_header.message_len_msb));

   data_index = 0;

   message_position = sizeof(message_ptr->connection_id) + sizeof(message_ptr->as_id);

   /* Klocwork fix*/ 
   max_data_index = GSM_CAUSE_SIZE +
#ifdef FEATURE_CCBS
         sizeof(ie_allowed_actions_T) +
#endif /* FEATURE_CCBS */
         sizeof(ie_progress_indicator_T); 

   cm_cc_cause_ptr->present = FALSE;
   cm_prog_ind_ptr->present = FALSE;

   while(message_position < message_length)
   {
      /* Klocwork fix*/ 
      if(data_index >=0 && data_index < max_data_index)
      {
          switch(message_ptr->data[data_index])
          {
          case IEI_CAUSE:
             {
                data_ptr = &(message_ptr->data[data_index]);
                last_octet_data_ptr = &(message_ptr->data[sizeof(message_ptr->data) -1]);
                index = MN_get_cause_data(data_ptr, cm_cc_cause_ptr, last_octet_data_ptr);
    
                data_index = data_index + index;
                message_position += index;
    
                break;
             }
    
          case IEI_PROGRESS_INDICATOR:
             {
                cm_prog_ind_ptr->present = TRUE;

                /* Klocwork fix*/ 
                if(data_index < (max_data_index - 1))
                {
                  data_index++;
                }
                
                /*  OCTET 2 - LENGTH  */
                message_position += (message_ptr->data[data_index] + 2);

                /* Klocwork fix*/  
                if(data_index < (max_data_index - 1))
                {
                  data_index++;
                }
    
                /*  DECODE OCTET 3  */
                /*fixed compile warning C4244 conversion from int to byte */
                cm_prog_ind_ptr->location = 
                    (byte)(message_ptr->data[data_index] & 0x0f);
                cm_prog_ind_ptr->coding_standard = 
                    (byte)(message_ptr->data[data_index] & 0x60);

                /* Klocwork fix*/ 
                if(data_index < (max_data_index - 1))
                {
                  data_index++;
                }
                
                /*  DECODE OCTET 4  */
                cm_prog_ind_ptr->progress_description = 
                    (byte)(message_ptr->data[data_index] & 0x7f);

                /* Klocwork fix*/ 
                if(data_index < (max_data_index - 1))
                {
                  data_index++;
                }
    
                break;
                /*  POINT AT NEXT IEI  */
    
             }
    
    #ifdef FEATURE_CCBS
          case IEI_ALLOWED_ACTIONS:
             {
                allowed_actions_ptr->present = TRUE;

                /* Klocwork fix*/ 
                if(data_index < (max_data_index - 1))
                {
                  data_index++;
                }
                
                /*  OCTET 2 - LENGTH  */
                message_position += (message_ptr->data[data_index] + 2);

                /* Klocwork fix*/ 
                if(data_index < (max_data_index - 1))
                {
                  data_index++;
                }
    
                /*  DECODE OCTET 3  */
                allowed_actions_ptr->ccbs_possible = 
                    message_ptr->data[data_index] & 0x80 ? TRUE : FALSE;

                /* Klocwork fix*/ 
                if(data_index < (max_data_index - 1))
                {
                  data_index++;
                }
                
                break;
                /*  POINT AT NEXT IEI  */
             }
    #endif /* FEATURE_CCBS */
             
          default:
             {
                /* Break the loop to prevent looping */
    
                message_position = message_length;
    
                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected IE in MNCC_DISC_IND");
    
                break;
    
             }
          } /* switch */
      } 
      else
      {
          ERR("ERROR: Buffer Overflow",0,0,0); 
      }
   }
}


/*
 * Function name : MN_get_MNCC_FACILITY_IND 
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
 * Parameters: message_ptr
 * ----------  MN_FACILITY_DATA_T *
 *             message_length  This is the length of the message measured after
 *                             facility IEI and length
 *
 * Returns:
 * --------
 *
 */
VOID_FUNC MN_get_MNCC_FACILITY_IND(byte *data_ptr,
word message_length,
MN_FACILITY_DATA_T *facility_data,
byte *last_octet_data_pointer)
{
   word component_length;
   byte component_number;
   word message_position;
   word parameter_length;
   word index = 0;
   connection_id_T  connection_id;
   return_size_T    size;


   /*  NULL THE OUTPUT DATA  - but save the connection_id */

   connection_id = facility_data->connection_id;

   memset(facility_data, 0, sizeof(*facility_data));

   facility_data->connection_id = connection_id;

   for( component_number = 0; 
       component_number < MAX_COMPONENTS; 
       component_number++)
   {
      facility_data->facility_component_data[component_number].component_type 
          = UNKNOWN;
   }

   component_number = 0;

   /*  SCAN THE MESSAGE  */

   message_position = 0;    /* size of conn_id and facility IEI-length */

   while(message_position < message_length && component_number < MAX_COMPONENTS)
   {
      switch(*data_ptr)
      {
      case INVOKE:
         {
            data_ptr++;
            /*  DECODE THE COMPONENT TYPE  */
            facility_data->facility_component_data[component_number].
                component_type = INVOKE;
            size = MN_get_element_length(data_ptr,SS_MAX_INVOKE_COMPONENT_LENGTH,(last_octet_data_pointer - data_ptr + 1));
            data_ptr+=size.increment;
            component_length = size.length;
            message_position += (size.increment+1);

            if((message_position + component_length) > message_length)
            {
              message_position = message_length;            
              break; 

              /* Before storing the data in our buffer, check read overflow won't occur */
            }

            if(component_length < 3)
            {
              /*
               * INVOKE component contains two mandatory TAGS: Invoke ID and Operation Code
               * So, component_length can't be less than 6 (length is 3 for each of the above)
               */
              message_position = message_length;
              break;
            }

            component_length--;
            /*  DECODE THE INVOKE ID  */
            if(*data_ptr == INVOKE_ID_TAG)

            /* Invoke_Id_tag */
            {
               data_ptr++;
               component_length--;
               parameter_length = *(data_ptr++);
               component_length--;
               parameter_length--;

               /* Set invoke id */
               facility_data->facility_component_data[component_number].
                   invoke_id = *(data_ptr++);
               facility_data->facility_component_data[component_number].
                   invoke_id_present = TRUE;
               message_position += 3;

               /* Check parameter was correct length */
               if(parameter_length != 0)
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invoke ID wrong length");
              
                  message_position = message_length;
                  break;
               }
            }

            else
            {
                MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invoke ID missing");

                message_position = message_length;
                break;
            }

            /*
             * Component length should be minimum of 3 here
             * Operation Code Tag is mandatory.
             */

            if(component_length >= 3)
            {
               /*  CARRY ON - DECODE THE LINKED ID TAG AND PUT IN PARAMETERS  */

               if(*data_ptr == LINKED_ID_TAG)   /* linked ID tag */
               {
                  data_ptr++;
                  component_length--;

                  /* Set linked id present flag */
                  facility_data->facility_component_data[component_number].
                      linked_id_present = TRUE;
                  parameter_length = *(data_ptr++);
                  message_position += 2;
                  component_length--;

                  /* Load the Linked ID */
                  facility_data->facility_component_data[component_number].
                      linked_id = *(data_ptr++);
                  component_length--;
                  message_position++;
                  parameter_length--;

                  /* Check the parameter was correct length */
                  if (parameter_length != 0)
                  {
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Linked id incorrect length");

                     message_position = message_length;
                     break;
                  }
               }

               if(component_length < 3)
               {
                 /*
                  * component_length should be atleast 3 as INVOKE
                  * component contains OPERATION CODE mandatorily.
                  */
                 message_position = message_length;
                 break;
               }

               /*  DECODE THE OPERATION CODE TAG  */
               if(*data_ptr == OP_CODE_TAG)
               /*  Operation code tag */
               {
                  data_ptr++;
                  component_length--;

                  /* Get parameter length */
                  parameter_length = *(data_ptr++);
                  component_length--;
                  parameter_length--;

                  /* Load the operation code */
                  facility_data->facility_component_data[component_number].
                      operational_code = *(data_ptr++);
                  facility_data->facility_component_data[component_number].
                      operational_code_present = TRUE;
                  component_length--;

                  message_position += 3;

                  /* Check the parameter was correct length */
                  if(parameter_length != 0)
                  {
                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Operation code wrong length");

                      message_position = message_length;
                      break;
                  }
               }

               else
               {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Operation code is missing");

                  facility_data->facility_component_data[component_number].
                      operational_code_present = FALSE;
               }
               /*  THE REST OF THE MESSAGE_MUST BE PARAMETERS  */

               facility_data->facility_component_data[component_number].
                   parameter_length = (byte)component_length;
               for(index = 0; index < component_length; index++)
               {
                  message_position++;
                  facility_data->facility_component_data[component_number].
                      parameters[index] = *(data_ptr++);
               }
            }


            component_number++;
            break;
         }

      case RETURN_RESULT:
         {
            /* By default there is no linked ID in a Return Result component */
            facility_data->facility_component_data[component_number].

            linked_id_present = FALSE;

            /*  DECODE THE COMPONENT TYPE  */

            data_ptr++;
            facility_data->facility_component_data[component_number].
                component_type = RETURN_RESULT;
            size = MN_get_element_length(data_ptr,SS_MAX_RETURN_RESULT_LENGTH, last_octet_data_pointer - data_ptr + 1);
            component_length = size.length;
            data_ptr += size.increment;
            message_position += (size.increment + 1);

            if((message_position + component_length) > message_length)
            {
              message_position = message_length;            
              /* Before storing the data in our buffer, check read overflow won't occur */
              break;
            }

            if(component_length < 3)
            {
              /*
               * If processing Invoke ID Tag, component length is 3.
               */
              message_position = message_length;
              break;
            }

            /*  DECODE THE INVOKE ID  */
            if(*data_ptr == INVOKE_ID_TAG)
            /* Invoke ID tag */
            {
               data_ptr++;
               component_length--;
               /* Move to next octet */

               /* Get parameter length */
               parameter_length = *(data_ptr++);
               component_length--;
               /* Move to next octet */
               facility_data->facility_component_data[component_number].
                   invoke_id = *(data_ptr++);
               parameter_length--;
               component_length--;
               /* Move to next octet */
               facility_data->facility_component_data[component_number].
                   invoke_id_present = TRUE;

               message_position += 3;

               /* Check the parameter was correct length */
               if(parameter_length != 0)
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invoke ID is wrong length");
                  message_position = message_length;
                  break;
               }
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invoke ID is missing");
               message_position = message_length;
               break;
            } /* INVOKE_ID_TAG */

            if (component_length > 0)
            {
               if ((*data_ptr == SEQUENCE_TAG) || (*data_ptr == SET_TAG))        /* Only if parameters are present */
               {
                  boolean  is_indefinite=FALSE;

                  data_ptr++;
                  component_length--;

                  if (*data_ptr == 0x80)
                  {
                     is_indefinite = TRUE;
                  }

                  /* Get the element length */
                  size = MN_get_element_length(data_ptr,SS_MAX_PARAMETER_LENGTH, last_octet_data_pointer - data_ptr + 1);
                  data_ptr += size.increment;

                  component_length = size.length;
                  message_position += (size.increment + 1);

                  if((message_position + component_length) > message_length)
                  {
                    message_position = message_length;            
                    /* Before storing the data in our buffer, check read overflow won't occur */
                    break;
                  }

                  if(component_length < 3)
                  {
                    /*
                     * If Operation Code TAG is present, component_length >= 3
                     */
                    message_position = message_length;
                    break;
                  }


                  if (*data_ptr == OP_CODE_TAG)
                  {
                     data_ptr++;
                     component_length--;

                     /* Get parameter length */
                     parameter_length = *(data_ptr++);
                     component_length--;

                     /* Load the operation code */
                     facility_data->facility_component_data[component_number].
                         operational_code = *(data_ptr++);
                     facility_data->facility_component_data[component_number].
                         operational_code_present = TRUE;
                     component_length--;
                     parameter_length--;
                     message_position += 3;

                     /* Check the parameter was correct length */
                     if (parameter_length != 0)
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect operational code length"); 
                     }
                  }
                  else
                  {
                     facility_data->facility_component_data[component_number].
                         operational_code_present = FALSE;
                  } /* OP_CODE_TAG */

                  /*  THE REST OF THE MESSAGE MUST BE PARAMETERS  */

                  /* Get the parameter length */
                  facility_data->facility_component_data[component_number].
                      parameter_length = (byte)component_length;

                  if (component_length > MAX_SS_PARAMETER_LENGTH )
                  {
                     component_length = MAX_SS_PARAMETER_LENGTH;
 
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Max SS Parameter length exceeded");
                  }

                  for(index = 0; index < component_length; index++)
                  {
                     message_position++;
                     facility_data->facility_component_data[component_number].parameters[index]
                         =*(data_ptr++);
                  }
                  
                  if (is_indefinite)
                  {
                     data_ptr         += 2;
                     message_position += 2;
                  }
               } /* != SET_TAG */
               else if(*data_ptr == OP_CODE_TAG) /* could have op_code w/o parameters */
               {

                     if(component_length < 3)
                     {
                       message_position = message_length;
                       break;
                     }

                     data_ptr++;
                     component_length--;

                     /* Get parameter length */
                     parameter_length = *(data_ptr++);
                     component_length--;

                     /* Load the operation code */
                     facility_data->facility_component_data[component_number].
                         operational_code = *(data_ptr++);
                     facility_data->facility_component_data[component_number].
                         operational_code_present = TRUE;
                     component_length--;
                     parameter_length--;
                     message_position += 3;

                     /* Check the parameter was correct length */
                     if (parameter_length != 0)
                     {
                        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Incorrect operational code length"); 
                        message_position = message_length;
                        break;
                     }

                     /* CR88759 adding this section */
                     /* Get the parameter length */
                     facility_data->facility_component_data[component_number].
                        parameter_length =(byte) component_length; 

                     if (component_length > MAX_SS_PARAMETER_LENGTH )
                     {
                       component_length = MAX_SS_PARAMETER_LENGTH;
   
                       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Max SS Parameter length exceeded");
                     }
                     for(index = 0; index < component_length; index++)
                     {
                       message_position++;
                       facility_data->facility_component_data[component_number].parameters[index]
                           =*(data_ptr++);
                     }  
                     /* ending section for CR88759 */ 

               } /* OP_CODE_TAG */
               else
               {
                   continue;
               }
            }

            /*  END OF COMPONENT  */
            component_number++;

            break;
         }

      case RETURN_ERROR:
         {
            facility_data->facility_component_data[component_number].
                component_type = RETURN_ERROR;
            data_ptr++;
            size = MN_get_element_length(data_ptr,SS_MAX_RETURN_ERROR_LENGTH, last_octet_data_pointer - data_ptr + 1);
            component_length = size.length;
            data_ptr += size.increment;
            message_position += (size.increment + 1);

            if((message_position + component_length) > message_length)
            {
              message_position = message_length;            
              /* Before storing the data in our buffer, check read overflow won't occur */
              break;
            }

            if(component_length < 3)
            {
              message_position = message_length;
              break;
            }


            /*  DECODE THE INVOKE ID  */
            if(*data_ptr == INVOKE_ID_TAG)
            {
               data_ptr++;
               component_length--;

               /* Get parameter length */
               parameter_length = *(data_ptr++);
               component_length--;

               /* Load invoke id */
               facility_data->facility_component_data[component_number].
                  invoke_id = *(data_ptr++);
               facility_data->facility_component_data[component_number].
                  invoke_id_present = TRUE;
               parameter_length--;
               component_length--;
               message_position += 3;

               /* Check the parameter was correct length */
               if(parameter_length != 0)
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invoke ID is the wrong length");
                  message_position = message_length;
                  break;
               }
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invoke ID is missing");
               message_position = message_length;
               break;
            }

            if(component_length < 3)
            {
              message_position = message_length;
              break;
            }

            /*  DECODE THE ERROR CODE TAG  */
            if(*data_ptr == ERROR_CODE_TAG)  /* Error Code tag */
            {
               component_length--;
               data_ptr++;

               /* Get parameter length */
               parameter_length = *(data_ptr++);
               component_length--;

               /* Load the error code */
               facility_data->facility_component_data[component_number].
                  error_code = *(data_ptr++);
               parameter_length--;
               component_length--;
               message_position += 3;

               /* Check the parameter was correct length */
               if(parameter_length != 0)
               {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Error code is the wrong length");
                   message_position = message_length;
                   break;
               }
            }

            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Error code is missing");
               message_position = message_length;
               break;
            }

            /*  THE REST OF THE MESSAGE MUST BE PARAMETERS  */

            facility_data->facility_component_data[component_number].
                parameter_length =(byte) component_length;
            parameter_length = component_length;

            for(index = 0; index < parameter_length; index++)
            {
               message_position++;
               facility_data->facility_component_data[component_number].parameters[index] =
                   *(data_ptr++);
               component_length--;
            }
            /*  END OF COMPONENT  */

            if (component_length != 0)
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Component is incorrect length");
               message_position = message_length;
               break;
            }

            component_number++;
            break;
         }

      case REJECT:
         {
            /*  DECODE THE COMPONENT TYPE  */

            facility_data->facility_component_data[component_number].
                component_type = REJECT;

            data_ptr++;
            size = MN_get_element_length(data_ptr,SS_MAX_REJECT_LENGTH, last_octet_data_pointer - data_ptr + 1);
            component_length = size.length;
            message_position += (size.increment + 1);
            data_ptr += size.increment;

            if((message_position + component_length) > message_length)
            {
              /* Before storing the data in our buffer, check read overflow won't occur */
              message_position = message_length;
              break;
            }


            /*  DECODE THE INVOKE ID  */
            if(*data_ptr == INVOKE_ID_TAG)
            {

              if(component_length < 3)
              {
                message_position = message_length;
                break;
              }
            
               data_ptr++;
               component_length--;
               parameter_length = *(data_ptr++);
               component_length--;
               facility_data->facility_component_data[component_number].invoke_id =
                   *(data_ptr++);
               parameter_length--;
               facility_data->facility_component_data[component_number].
                   invoke_id_present = TRUE;
               component_length--;
               message_position += 3;
               if(parameter_length != 0)
               {
                   MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invoke ID is wrong length");
                   message_position = message_length;
                   break;
               }
            }
            else
            {
               if(*data_ptr == NULL_TAG)  /* Universal Null tag */
               {

                 if(component_length < 3)
                 {
                   message_position = message_length;
                   break;
                 }
               
                  /*   IT COULD BE A NULL COMPONENT  */
                  component_length--;
                  data_ptr++;
                  message_position++;

                  if(*data_ptr != 0)

                  {
                     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Universal Null wrong length");
                     message_position = message_length;
                     break;
                  }
                  else
                  {

                     facility_data->facility_component_data[component_number].
                         invoke_id_present = FALSE;
                  }
                  component_length--;
                  data_ptr++;
                  message_position++;
               }
               else
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Invoke ID is missing");
                  message_position = message_length;
                  break;
               }
            }
            /*  DECODE THE PROBLEM CODE TAG  */
            if( (*data_ptr >= GENERAL_PROBLEM) &&
                (*data_ptr <= RETURN_ERROR_PROBLEM) )
            {

              if(component_length < 3)
              {
                message_position = message_length;
                break;
              }

               facility_data->facility_component_data[component_number].problem_tag =
                   *(data_ptr++);
               component_length--;
               parameter_length = *(data_ptr++);
               component_length--;
               facility_data->facility_component_data[component_number].problem_code =
                   *(data_ptr++);
               parameter_length--;
               component_length--;
               message_position += 3;

               if(parameter_length != 0)
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Error code is wrong length");

                  message_position = message_length;
                  break;
               }

               if(component_length != 0)
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Reject component is wrong length");
                  message_position = message_length;
                  break;
               }
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Problem code ID is missing");
               message_position = message_length;
               break;
            }
            /*  END OF COMPONENT  */
            component_number++;
            break;
         }

      case UNKNOWN:
         component_number++;
         message_position = message_length;
         break;

      default:
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unknown component type");

            message_position = message_length;
            break;
         }

      } /* switch */

   } /* while message_position */

}
