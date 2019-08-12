/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_handle_test.c_v   1.3   17 Aug 2001 15:19:16   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_handle_test.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/10/01     CD     Removed unnecessary header file includes

5/22/01     CD     Removed #include for cause.h and ms.h as they are not used

                   Removed redundant declaration of cc_transaction_information

                   Removed declaration of extern mn_call_information since it was 
                   not referenced

                   Changed calculation of message_length to use GET_IMH_LEN.  
                   This avoids lint error 701

8/10/01     CD     Removed unnecessary queue.h

08/16/01    AB     Renamed MMI references to CM.

===========================================================================*/


/* Includes */

#include "customer.h"
#include "rex.h"

#include "environ.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "nasutils.h"




#define MN_TEST_CHANGE_CC_STATE  0


/*
 * Function name :  MN_handle_test(IMH_T   *message)
 * -------------
 *
 * Description :  Decodes special test messages that change the state
 * -----------    of MN 
 *                
 *
 *
 * Uses :        
 * ----
 *
 *
 * Parameters:  message
 * ----------
 *
 *
 *
 * Returns:
 * --------
 *
 */

VOID_FUNC  MN_handle_test(IMH_T   *message)

{

   /* Locals */

   int    message_length,i;
   byte    *pointer,connection_id;
   /*
   message_length = (int)message->message_len_msb;
   message_length = message_length <<8 ;
   message_length += (int)message->message_len_lsb;
   */
   GET_IMH_LEN(message_length, message);

   pointer = (byte *)message;

   pointer+=4;  /* point at first byte of message data */

   switch(message->message_id)

   {

   case MN_TEST_CHANGE_CC_STATE:

      /*  Format of MN_test_change_cc_transaction_state message is:-
                  
                         IMH_T  header
                         
                         byte  connection_id
                         byte  cc_state
                         byte  hold_auxiliary_state
                         byte  mpty_auxiliary_state
                  
                  HOLD_IDLE               0
                   HOLD_REQUEST            1
                   HOLD_CALL_HELD          2
                   HOLD_RETRIEVE_REQUEST   3
                  
                   MPTY_IDLE               0
                   MPTY_REQUEST            1
                   MPTY_CALL_IN_MPTY       2
                   MPTY_SPLIT_REQUEST      3
                  
                  
                  */

      for (i = 0; i< message_length;)

      {

         connection_id = *pointer;
         i++;
         pointer++;
         
         if(cc_transaction_information_p[connection_id] != NULL)
         {
           cc_transaction_information_p[connection_id]->cc_state = *pointer;
         }
         else
         {
           MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot update cc_state as cc_transaction_information_p[%d] is NULL",
                              connection_id);
         }
         i++;
         pointer++;
         if(cc_transaction_information_p[connection_id] != NULL)
         {
           cc_transaction_information_p[connection_id]->hold_auxiliary_state = *pointer;
         }
         else
         {
           MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot update hold_auxiliary_state as cc_transaction_information_p[%d] is NULL",
                            connection_id);
         }
         i++;
         pointer++;
         if(cc_transaction_information_p[connection_id] != NULL)
         {
           cc_transaction_information_p[connection_id]->mpty_auxiliary_state = *pointer;
         }
         else
         {
           MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot update mpty_auxiliary_state as cc_transaction_information_p[%d] is NULL",
                            connection_id);
         }
         i++;
         pointer++;
         
      }

      break;

   default:

      break;

   }

}
