/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_put_cnm_sms_msgs.c_v   1.6   25 Oct 2001 09:27:04   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_put_cnm_sms_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/10/01     CD     Removed unnecessary header file includes

5/24/01     CD     Removed cause.h since it was not used

                   Corrected calling sequence for PUT_IMH_LEN to avoid lint errors

7/18/01     CD     Removed duplicate external declaration of sm_rl_transaction_information
                   Removed duplicate external declaration of mo_rpdu_store
                   Removed external declaration of rl_mo_mr

8/10/01     CD     Removed unnecessary queue.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

08/16/01    AB     Renamed MMI references to CM.

10/12/01    CD     Added logging to MN_put_MNSMS_REL_REQ()

12/15/04    AB     Added supports for AT+CMMS=1, ATCOP multiple MO SMS.

11/02/06    NR     Fixed lint errors
===========================================================================*/
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include <string.h>
#include "ms.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include <stringl/stringl.h>
#include "nasutils.h"


/*
 * Function name :  MN_put_MNSMS_ABORT_REQ
 * -------------
 *
 * Description :    Builds & sends message to SMS entity 
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  cp_cause
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_put_MNSMS_ABORT_REQ(connection_id_T connection_id,
                                 byte            cause_value)
{
   MNSMS_ABORT_REQ_T message;
   gs_status_T       return_value;
   size_t            message_size;

   rl_mo_mr = sm_rl_transaction_information[connection_id].rl_message_ref;

   /* Create the message */

   message.message_header.message_set = MS_MN_SMS;
   message.message_header.message_id  = MNSMS_ABORT_REQ;
   message_size = sizeof(MNSMS_ABORT_REQ_T) - sizeof(IMH_T);

   PUT_IMH_LEN((int)message_size,&message.message_header);

   message.connection_id = connection_id;
   message.cp_cause.Iei  = IEI_CP_CAUSE;
   message.cp_cause.cause_value = cause_value;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   /* Send the message */

   return_value = MN_send_message(GS_QUEUE_MN_CM, &message, TRUE);

   if (return_value != GS_SUCCESS)
   {
      ERR("ERROR: MNSMS_ABORT_REQ message send failed",0,0,0);
  
   }
}



/*
 * Function name :  MN_put_MNSMS_DATA_REQ
 * -------------
 *
 * Description :    Builds & sends message to SMS entity 
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  cp_cause
 *             
 *             
 *
 * Returns:    None
 * --------
 *
 */

VOID_FUNC MN_put_MNSMS_DATA_REQ( connection_id_T connection_id,
                                 byte            *rpdu,
                                 byte            rpdu_length)
{
   MNSMS_DATA_REQ_T message;
   gs_status_T      return_value;
   size_t           message_size;
   int              index;

   /* Create the message */

   message.message_header.message_set = MS_MN_SMS;
   message.message_header.message_id  = MNSMS_DATA_REQ;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   message_size =
      ((sizeof(MNSMS_DATA_REQ_T) -
      sizeof(IMH_T)) -
      sizeof(message.rpdu.data)) +
      rpdu_length;

   PUT_IMH_LEN((int)message_size,&message.message_header)

   message.connection_id = connection_id;

   message.rpdu_len = rpdu_length;

   for (index = 0;
       index < rpdu_length;
       index++)
   {
      message.rpdu.data[index] = rpdu[index];
   }

   /* Send the message */

   return_value = MN_send_message(GS_QUEUE_MN_CM, &message, TRUE);

   if (return_value != GS_SUCCESS)
   {
      ERR("ERROR: MNSMS_DATA_REQ message send failed",0,0,0);
   
   }
}


/*
 * Function name :  MN_put_MNSMS_EST_REQ
 * -------------
 *
 * Description :    Builds & sends message to SMS entity 
 * -----------    
 *
 *
 *
 *
 *
 *
 * Parameters: connection_id
 * ----------  rpdu
 *             rpdu_length
 *             
 *
 * Returns:    rl_message_ref
 * --------
 *
 */

VOID_FUNC MN_put_MNSMS_EST_REQ(void)
{
   MNSMS_EST_REQ_T message;
   gs_status_T     return_value;
   size_t          message_size;

   /* Get a new rl_mr to use with this message */

   rl_mo_mr = MN_get_rl_mo_mr();

   /* Store it for comparing with return ACK's/ERROR's */

   sm_rl_transaction_information[mo_rpdu_store.connection_id].
       rl_message_ref = rl_mo_mr;

   mo_rpdu_store.rpdu[1] = rl_mo_mr; /* Always the 2nd octet */

   /* Create the message */

   message.message_header.message_set = MS_MN_SMS;

   message.message_header.message_id  = MNSMS_EST_REQ;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   message_size =
      ((sizeof(MNSMS_EST_REQ_T) -
      sizeof(IMH_T)) -
      sizeof(message.rpdu.data)) +
      mo_rpdu_store.rpdu_length;

   PUT_IMH_LEN((int)message_size,&message.message_header);

   message.connection_id = mo_rpdu_store.connection_id;

   message.rpdu_len = mo_rpdu_store.rpdu_length;

   memscpy(message.rpdu.data,
          sizeof(message.rpdu.data),
          mo_rpdu_store.rpdu,
          mo_rpdu_store.rpdu_length);

   /* Send the message */

   return_value = MN_send_message(GS_QUEUE_MN_CM, &message, TRUE);

   if (return_value != GS_SUCCESS)
   {
      ERR("ERROR: MNSMS_EST_REQ message send failed",0,0,0);

   }
}


/*
 * Function name :  MN_put_MNSMS_REL_REQ
 * -------------
 *
 * Description :    Builds & sends message to SMS entity 
 * -----------    
 *
 * Parameters: connection_id
 * ----------  cause_value
 *          
 * Returns:    rl_message_ref
 * --------
 */

VOID_FUNC MN_put_MNSMS_REL_REQ(connection_id_T connection_id,
                               byte            cause_value)
{
   MNSMS_REL_REQ_T message;
   gs_status_T     return_value;
   size_t          message_size;

   if((sms_link_control.mode) && 
      (sms_link_control.status == SMS_LINK_CONTROL_INACTIVE))  /* TLC1M timeout */
   {
       int conn_id;
       int sms_exists = 0;
      
       for(conn_id = 0;conn_id <= MAX_MO_CONNECTION_ID; conn_id++)
       {
          if((conn_id != connection_id) && (sms_transaction_information[conn_id].sms_state !=SMS_IDLE))
          {
             sms_exists = 1;
             break;
          }
       }
       
       if(sms_exists == 0) /*if there is no SMS segement in the process*/
       {
          /* Update the link status */
          sms_link_control.status = SMS_LINK_CONTROL_ACTIVE;

          MN_start_sms_timer(connection_id, TLC1M);
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Starting the TLC1M timer");
       }
   }
   
   /* Create the message */
   message.message_header.message_set = MS_MN_SMS;
   message.message_header.message_id  = MNSMS_REL_REQ;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   message_size = sizeof(MNSMS_REL_REQ_T) - sizeof(IMH_T);

   PUT_IMH_LEN((int)message_size,&message.message_header)

   message.connection_id = connection_id;
   message.cp_cause.Iei  = IEI_CP_CAUSE;
   message.cp_cause.cause_value = cause_value;
   
   MSG_LOW_DS_2(MN_SUB,"=MNCNM= MNSMS_REL_REQ connId %d cause %d",
            connection_id,
            cause_value);

   /* Send the SMS release request message */
   return_value = MN_send_message(GS_QUEUE_MN_CM, &message, TRUE);

   if (return_value != GS_SUCCESS)
   {
       ERR("ERROR: MNSMS_REL_REQ message send failed",0,0,0);
   }
}


