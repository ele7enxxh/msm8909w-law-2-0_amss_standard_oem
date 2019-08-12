/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_sms_send_msgs.c_v   1.6   12 Mar 2002 16:39:16   vtawker  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_sms_send_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
7/17/01     CD     Added log message whenever the mo/mt retransmit counter value changes
                   Included msg.h

8/1/01      CD     Fixed log message in CNM_send_cp_data

08/07/01    CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

09/14/01    CD     Modified call to MSG_HIGH to avoid using %s on target

12-Mar-2002  VT    Removed Lint errors.

8/7/2002    RG     Initialized CNM_mt_cp_data_retransmit_counter to 
                   CP_DATA_RETRANSMIT_COUNT+1 instead of CP_DATA_RETRANSMIT_COUNT. 
                   This is to ensure that the CP-DATA(RP-ACK)
                   is retransmitted twice during an MT-SMS transaction.

02/14/03    AB     Set prefer radio link domain for SMS transaction in
                   CNM_send_cp_ack(), CNM_send_cp_data, and CNM_send_cp_error.
 
                   Removed lint errors.

02/24/03    AB     In CNM_send_rp_message(), check rpdu_len?

07/13/05    HS     CNM_mt_cp_message and CNM_mt_cp_data_retransmit_counter
                   exanded to save CP data for all call ids
                   
04/09/07    RD     SMS optimization changes are incorporated

10/12/11    PM     Ignoring call to function CNM_send_cp_error() if connection
                   id = 15.

10/17/11   PM      Adding lower_layer_cause as new argument to function 
                   CNM_send_error_indication_to_rp_layer(). This will determine
                   the error_status to be sent to WMS by MN while handling
                   MNSMS_ERROR_IND.
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "com_iei_v.h"
#include "ms.h"
#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"

#include "cmlib.h"
#include "cnm.h"

#include "nasutils.h"
#include "cm_sms.h"
#include <stringl/stringl.h>


/**************** Begin of   CNM_send_rp_message **********************************
 *
 *  Function name: CNM_send_rp_message ()
 *  ------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles requestd from MN to send an SMS message. This is
 *  expected to be an RP_ACK message. The message is stored and the retry
 *  counter is initialised
 *
 *
 *  Parameters:
 *  -----------
 *  message: IN - input message
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

byte CNM_send_rp_message ( IMH_T * message,
                          connection_id_T connection_id )
{
   byte              return_value;
   sms_state_T       current_state;
   boolean           allowed_state;
   cp_data_msg_T     *p_cp_data = NULL;
   word              rpdu_len;

   allowed_state = FALSE;

   current_state = sms_transaction_information[connection_id].sms_state;

   if (connection_id < 8 )       /* IF MO transaction */
   {
      p_cp_data = ( cp_data_msg_T *)((void *)&CNM_mo_cp_message);

      if ( ( current_state == SMS_MM_CONN_PEND )  ||
           ( current_state == SMS_MM_CONN_EST ) )
      {
         allowed_state = TRUE;
      }
   }
   else
   {


      if ( current_state == SMS_MM_CONN_EST )
      {
         allowed_state = TRUE;
#ifdef FEATURE_MODEM_HEAP
        if(CNM_mt_cp_message[connection_id-8] == NULL)
        {
          CNM_mt_cp_message[connection_id-8] = (mmcnm_data_req_T*)modem_mem_calloc(1, sizeof(mmcnm_data_req_T),MODEM_MEM_CLIENT_NAS);
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= ALLOC - CNM_mt_cp_message[%d]\n", (connection_id-8));
          if(CNM_mt_cp_message[connection_id-8] == NULL)
          {
            MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= Memory allocation failure for CNM_mt_cp_message[%d]\n", (connection_id-8), 0, 0 );
          }
          else
          {
            p_cp_data = ( cp_data_msg_T *)((void *)CNM_mt_cp_message[connection_id-8]);
          }
        }
        else
        {
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot ALLOC - CNM_mt_cp_message[%d] is not NULL", (connection_id-8) );
        }
#else
        p_cp_data = ( cp_data_msg_T *)((void *)&CNM_mt_cp_message[connection_id-8]);
#endif
      }
      else
      {
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Invalid state = %d, to handle RP-ACK", current_state );
      }      

        /* each MT call id has its buffer */

   }
   

   rpdu_len = ((MNSMS_DATA_REQ_T *)((void *)message))->rpdu_len;

   if ( (allowed_state != FALSE) && (p_cp_data != NULL))
   {
      p_cp_data->msg_head.IMH.message_set = MS_CC_MM;
      p_cp_data->msg_head.IMH.message_id = MMCNM_DATA_REQ;
      p_cp_data->msg_head.ti_pd = (unsigned char) (FORM_TI_PD(connection_id, PD_SMS));

      p_cp_data->msg_head.type = CP_DATA;

      /*
       * check that length fits into the 1 byte available for length
       */

      if (rpdu_len > (MAX_LENGTH_SMS_DATA - 3))
      {
         ERR("RPDU too big, msg send aborted: received rpdu_len = %d\n",
              (dword) rpdu_len, 0, 0);

         return_value = NO_ACTION_event;
      }
      else
      {
         p_cp_data->data[0] = (unsigned char) rpdu_len;

         
         (void)memscpy((void *) &(p_cp_data->data[1]),
                       sizeof(p_cp_data->data) -1,
                      (const void *)  &((MNSMS_DATA_REQ_T *)((void *)message))->rpdu,
                      (unsigned int)rpdu_len);
        
         
         /*
          * Output mesage length is input rpdu_len
          * plus pd_ti plus message type +1 for the actual rpdu_len itself
          */


#ifdef FEATURE_DUAL_SIM
         PUT_IMH_LEN(((((sizeof(cp_data_msg_T) -    /* IMH_T + pdti + mt + array */
                      sizeof(IMH_T)) -            /* IMH_T */
                      sizeof(p_cp_data->data)) +  /* array */
                      sizeof (((MNSMS_DATA_REQ_T *)message)->rpdu_len)) + /* rpdu_len field */
                      rpdu_len)                  /* used part of array */
                      -sizeof(byte)  /* as_id */
                      ,&p_cp_data->msg_head.IMH );
#else
         PUT_IMH_LEN(((((sizeof(cp_data_msg_T) -    /* IMH_T + pdti + mt + array */
                      sizeof(IMH_T)) -            /* IMH_T */
                      sizeof(p_cp_data->data)) +  /* array */
                      sizeof (((MNSMS_DATA_REQ_T *)message)->rpdu_len)) + /* rpdu_len field */
                      rpdu_len),                  /* used part of array */
                      &p_cp_data->msg_head.IMH );
#endif

         if ( connection_id < 8 )
         {
            CNM_mo_cp_data_retransmit_counter = CP_DATA_RETRANSMIT_COUNT;
            MSG_MED_DS_2(MN_SUB,"=MNCNM= SMC-CS-%d MO retx counter = %d",connection_id, CNM_mo_cp_data_retransmit_counter);

         }
         else
         {

            // Change: CNM_mt_cp_data_retransmit_counter should be initialized to CP_DATA_RETRANSMIT_COUNT+1
            // since the "CNM_send_cp_data()" routine is called for sending the first CP_DATA itself. The
            // CNM_send_cp_data() routine decrements the counter on every call to it that causes a CP_DATA
            // to be sent. For MT SMS we have CP_DATA_RETRANSMIT_COUNT+1 (1CP-DATA+ 1 retries) 
            // calls to this function before sending an MMCNM_REL_REQ to MM.
            CNM_mt_cp_data_retransmit_counter[connection_id-8] = CP_DATA_RETRANSMIT_COUNT+1;
            MSG_MED_DS_2(MN_SUB,"=MNCNM= SMC-CS-%d MT retx counter = %d",connection_id, CNM_mt_cp_data_retransmit_counter[connection_id-8]);

         }

         return_value = S_RP_MESSAGE_REQ_event;

      }
   }
   else
   {
      ERR("Transaction not allowed in this state: current_state = %d\n",
           (dword) current_state, 0, 0);

      return_value = NO_ACTION_event;
   }

   return return_value;
}
/**************** End of   CNM_send_rp_message **********************************/


/**************** Begin of   CNM_send_cp_data *************************************
 *
 *  Function name: CNM_send_cp_data()
 *  --------------------------------
 *
 *  Description:
 *  ------------
 *  This function checks the repeat counter, to see if a message should be 
 *  If so, it sends the message already prepared, to mm for subsequent
 *  transmission over the air. each attempt to send causes the repeat counter
 *  to be decremented. 
 *  If the repeat counter is zero, the timer is stopped so no more attempts 
 *  occur.
 *  
 *
 *  Parameters:
 *  -----------
 *  connection_id: IN - input message
 *
 *  Returns:
 *  --------
 *  Signal indicating if CP-message was sent or not
 *
 ******************************************************************************/

byte CNM_send_cp_data ( connection_id_T connection_id )
{
   byte              return_value;
   byte              *p_count;
   IMH_T             *message;
   timer_id_T        timer;

   return_value = S_CP_ACK_FALSE_event;

   if (connection_id < 8 )          /* if MO transaction */
   {
      p_count = &CNM_mo_cp_data_retransmit_counter;
      message = (IMH_T *) &CNM_mo_cp_message;
      timer = TIMER_TC1M_MO;
   }
   else                             /* its MT */
   {
      p_count = &CNM_mt_cp_data_retransmit_counter[connection_id-8];
#ifdef FEATURE_MODEM_HEAP
      message = (IMH_T *) CNM_mt_cp_message[connection_id-8];
      if(message == NULL)
      {
         MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= CNM_mt_cp_message[%d] is NULL\n", (connection_id-8), 0, 0 );
      }
#else
      message = (IMH_T *) &CNM_mt_cp_message[connection_id-8];
#endif
      timer = TIMER_TC1M_MT;
   }  

   if(connection_id < MAXNO_SMS_TRANSACTIONS)
   {
     if(*p_count > 0)  /* check retry limit */
     {

       /* get the domain  */
       message->message_set = sms_transaction_information[connection_id].pref_rl_domain;

       CNM_send_message( message, GS_QUEUE_MM );

       (*p_count)--;
       MSG_MED_DS_2(MN_SUB,"=MNCNM= SMC-CS- %d retx counter = %d",connection_id, *p_count);

       return_value = S_CP_ACK_TRUE_event;
     }
     else                             /* No more retries */
     {
       /* make sure its stopped */
       CNM_stop_timer( timer, (unsigned char)FORM_TI_PD( connection_id, PD_SMS ) );
#ifdef FEATURE_MODEM_HEAP
       if((connection_id >= 8) && (CNM_mt_cp_message[connection_id-8] != NULL))
       {
         modem_mem_free(CNM_mt_cp_message[connection_id-8], MODEM_MEM_CLIENT_NAS);
         CNM_mt_cp_message[connection_id-8] = NULL;
         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= FREE - CNM_mt_cp_message[%d]\n", (connection_id-8) );
       }
#endif
       return_value = S_CP_ACK_FALSE_event;
     }
   }
   return return_value;
}
/**************** End of   CNM_send_cp_data ***************************************/


/**************** Begin of   CNM_send_cp_user_data ********************************
 *
 *  Function name: CNM_send_cp_user_data ()
 *  --------------------------------------
 *
 *  Description:
 *  ------------
 *  This function sends cp user information to MN. Thie was previously received
 *  by CNM_process_received_cp_message() and stored in CNM_mo_cp_user_data.
 *
 *
 *  Parameters:
 *  -----------
 *  connection_id
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_send_cp_user_data ( connection_id_T connection_id )
{
   MNSMS_DATA_IND_T      mnsms_data_ind;

   mnsms_data_ind.message_header.message_set = MS_MN_SMS;
   mnsms_data_ind.message_header.message_id  = MNSMS_DATA_IND;

   mnsms_data_ind.connection_id  = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mnsms_data_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   if ( connection_id < 8 )            /* if MO transaction */
   {
      if(CNM_mo_cp_user_data.length > MIN(sizeof(CNM_mo_cp_user_data.data), sizeof(mnsms_data_ind.rpdu.data)))
      {
        CNM_mo_cp_user_data.length =  MIN(sizeof(CNM_mo_cp_user_data.data), sizeof(mnsms_data_ind.rpdu.data));
      }
      (void)memscpy((void *) &mnsms_data_ind.rpdu.data[0],
                   sizeof(mnsms_data_ind.rpdu.data),
                   (const void *) CNM_mo_cp_user_data.data,
                   CNM_mo_cp_user_data.length);

      PUT_IMH_LEN( ((CNM_mo_cp_user_data.length + sizeof(MNSMS_DATA_IND_T))
                     - sizeof(rpdu_T)) - sizeof(IMH_T),
                   ((IMH_T *) &mnsms_data_ind) );

      CNM_send_message( (IMH_T *) &mnsms_data_ind, GS_QUEUE_MN_CM );

   }
   else
   {
     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Unexpected call to function CNM_send_cp_user_data");
   }
 
}
/**************** End of   CNM_send_cp_user_data **********************************/


/**************** Begin of   CNM_send_cp_ack **************************************
 *
 *  Function name: CNM_send_cp_ack()
 *  -------------------------------
 *
 *  Description:
 *  ------------
 *  This function checks the repeat counter, to see if a message should be 
 *  If so, it sends the message already prepared, to mm for subsequent
 *  transmission over the air. each attempt to send causes the repeat counter
 *  to be decremented. 
 *  If the repeat counter is zero, the timer is stopped so no more attempts 
 *  occur.
 *  
 *
 *  Parameters:
 *  -----------
 *  connection_id: IN - input message
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_send_cp_ack ( connection_id_T connection_id )
{
   cp_ack_msg_T         cp_ack_msg;

   if(connection_id < MAXNO_SMS_TRANSACTIONS)
   {
       cp_ack_msg.msg_head.IMH.message_set = sms_transaction_information[connection_id].pref_rl_domain;
   }

   cp_ack_msg.msg_head.IMH.message_id = MMCNM_DATA_REQ;

   cp_ack_msg.msg_head.ti_pd = (unsigned char) (FORM_TI_PD(connection_id, PD_SMS));

   cp_ack_msg.msg_head.type = CP_ACK;

   PUT_IMH_LEN( 2, ((IMH_T *) &cp_ack_msg.msg_head.IMH) );

   CNM_send_message( (IMH_T *) &cp_ack_msg, GS_QUEUE_MM );

}
/**************** End of   CNM_send_cp_ack ****************************************/


/****************  Begin of   CNM_send_error_indication_to_rp_layer ****************
 *
 *  Function name: CNM_send_error_indication_to_rp_layer ()
 *  ------------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function forms an error indication message and send it to the MN layer.
 *  The cause value in the message is set to congestion
 *  
 *
 *  Parameters:
 *  -----------
 *  connection_id
 *  lower_layer_cause: This is sent in MNSMS_ERROR_IND message to MN which determines
 *                     the error_status sent by MN to WMS in SM_RL_REPORT_IND
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_send_error_indication_to_rp_layer ( connection_id_T connection_id, byte lower_layer_cause )
{

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
   mmcc_domain_name_T domain = (sms_transaction_information[connection_id].pref_rl_domain == MS_CC_MM)? MMCC_SMS_CS : MMCC_SMS_PS;
   CNM_send_MNSMS_ERROR_IND( connection_id, cp_cause_value, lower_layer_cause, 0, domain);
#else
   CNM_send_MNSMS_ERROR_IND( connection_id, cp_cause_value, lower_layer_cause);
#endif

   cp_cause_value = 0;        /* reset as now used */

}
/**************** End of   CNM_send_error_indication_to_rp_layer ******************/


/**************** Begin of   CNM_send_cp_error ************************************
 *
 *  Function name: CNM_send_cp_error()
 *  ---------------------------------
 *
 *  Description:
 *  ------------
 *  This function generates a cp_error message and sends it to MM for
 *  subsequent transmission over the air
 *
 *  Parameters:
 *  -----------
 *  connection_id: IN - input message
 *  cause : IN  - value to put in the cp_error message
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_send_cp_error ( connection_id_T connection_id,
                             byte            cause )
{
   cp_error_msg_T         cp_error_msg;

   if(connection_id < MAXNO_SMS_TRANSACTIONS)
   {
     cp_error_msg.msg_head.IMH.message_set = 
         sms_transaction_information[connection_id].pref_rl_domain;
       
     cp_error_msg.msg_head.IMH.message_id = MMCNM_DATA_REQ;
     cp_error_msg.msg_head.ti_pd = (unsigned char) (FORM_TI_PD(connection_id, PD_SMS));

     cp_error_msg.msg_head.type = CP_ERROR;

     cp_error_msg.cp_cause = cause;

     PUT_IMH_LEN( (sizeof(msg_head_T) - sizeof(IMH_T)) + sizeof( byte ), 
                  ((IMH_T *) &cp_error_msg.msg_head.IMH) );

     CNM_send_message( (IMH_T *) &cp_error_msg, GS_QUEUE_MM );
   }
   else
   {
     MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Can't send CP_ERROR on Invalid Connection ID = %d",connection_id);
   }

}
/**************** End of   CNM_send_cp_error **************************************/


/****************  Begin of   CNM_send_cpack_indication_to_rp_layer ****************
 *
 *  Function name: CNM_send_cpack_indication_to_rp_layer ()
 *  ------------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function forms an cp ack indication message and send it to the MN layer.
 *  
 *
 *  Parameters:
 *  -----------
 *  connection_id 
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_send_cpack_indication_to_rp_layer ( connection_id_T connection_id )
{

   CNM_send_CPACK_IND( connection_id, 0, 0);

}
/**************** End of   CNM_send_cpack_indication_to_rp_layer ******************/

