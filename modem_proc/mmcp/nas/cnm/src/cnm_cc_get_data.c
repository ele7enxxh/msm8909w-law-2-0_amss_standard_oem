/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_cc_get_data.c_v   1.8   14 May 2002 18:17:30   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_cc_get_data.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/01   CD      Replaced mn header files by single include of mn.h

08/14/01   AB      Renamed connection manager references to CNM from CM.

09/17/01   CD      Modified CNM_get_connection_id for message_id MMCNM_EST_CNF

04/19/02   CD      Added function CNM_check_for_establish_ind()
                   Modified processing of CNM_get_connection_id()

04/30/02   CD      Corrected state SS and SMS state check in CNM_check_for_establish_ind()
                   Added check for receiving a SETUP/REGISTER/CP-DATA msg for an
                   existing transaction

05/06/02   CD      In CNM_check_for_establish_ind(), added inverting the first bit
                   of the transaction_id

05/14/02   AB      In CNM_check_for_establish_ind(), removed check for SS
                     transaction IDLE, as there can be more then one REGISTER 
                     message sent by the network in the same transaction.  
                     SS use this function to check whether is REGISTER or 
                     FACILITY message. 

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "com_iei_v.h"

#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"

#include "cmlib.h"

#include "cnm.h"

#include "nasutils.h"


/**************** Begin of   CNM_get_connection_id ********************************
 *
 *  Function name: CNM_get_connection_Id()
 *  -------------------------------------
 *
 *  Description:
 *  ------------
 *  This function obtains the connection id from a message received from the
 *  network
 *
 *  Parameters:
 *  -----------
 *  RX_message : INPUT
 *
 *  Returns:
 *  --------
 *  connection_id
 *
 ******************************************************************************/

connection_id_T CNM_get_connection_id( mmcnm_data_ind_T * message ) 
{
   connection_id_T   connection_id;

   /*
    * extract TI from message - returns a value 0-15
    */

   connection_id = (unsigned char)(GET_TI(message->msg_head.ti_pd));

   /*
   * Get the connection_id by inverting the first bit 
   */
   connection_id = (unsigned char)(connection_id ^ 0x08);     

   return (connection_id) ;
}
/**************** End of   CNM_get_connection_id **********************************/


/**************** Begin of   CNM_check_for_establish_ind ********************************
 *
 *  Function name: CNM_check_for_establish_ind()
 *  -------------------------------------
 *
 *  Description:
 *  ------------
 *  This function checks a MMCNM_DATA_IND message to see if it is the
 *  start of a new MT transaction
 *
 *  Parameters:
 *  -----------
 *  :IN MMCNM_DATA_IND
 *
 *  Returns:
 *  --------
 *   TRUE or FALSE
 *
 ******************************************************************************/

boolean CNM_check_for_establish_ind(mmcnm_data_ind_T * mmcnm_data_ind)
{
    boolean         est_ind = FALSE;
    connection_id_T transaction_id = 0x00;
    byte            protocol_discriminator = 0x00;
    boolean         ti_flag;

#if defined(FEATURE_DUAL_SIM )
   unsigned int     length;
   GET_IMH_LEN (length, &(mmcnm_data_ind->msg_head.IMH));
#endif

    /*
    * Get the PD and TI from the message
    */

    protocol_discriminator = (unsigned char)(GET_PD(mmcnm_data_ind->msg_head.ti_pd));
    transaction_id = (unsigned char)(GET_TI(mmcnm_data_ind->msg_head.ti_pd));
    ti_flag = transaction_id >>3;

    
    /* 
     * If the TI flag is set to 0, validate the transaction id 
     */
    if ((ti_flag == 0) && (transaction_id != UNKNOWN_CONNECTION_ID))
    {
       /* Invert the first bit of the transaction_id */
        transaction_id = (unsigned char)(transaction_id ^ 0x08);     
         
       /* 
        * Based on the PD and the message type,  check for the start of a MT transaction
        */
       switch(protocol_discriminator)
       {
       case PD_CC:
           if (mmcnm_data_ind->msg_head.type == SETUP) 
           {
               if (transaction_id < MAXNO_CC_TRANSACTIONS && transaction_id > UNKNOWN_CONNECTION_ID ) 
               {
               
                   if(cc_transaction_information_p[transaction_id] == NULL) 
                   {
                         /* returns true if memory is allocated else returns false. */
                         est_ind = MN_reserve_call_object(transaction_id);
                         if(est_ind == TRUE)
                         {
#if defined(FEATURE_DUAL_SIM )
                            if(MNCNM_IS_SINGLE_STACK)
                            {
                               mn_as_id = (sys_modem_as_id_e_type)mmcnm_data_ind->data[length -3]; /* length accomodates ti_pd + type + data + as id */
                               mn_call_information_p[transaction_id]->as_id = mn_as_id;
                               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Setting mn_as_id to %d",mn_as_id);
                            }
#endif
                         }
                   }
                   else if(cc_transaction_information_p[transaction_id]->cc_state == CC_STATE_U0)
                   {
                        est_ind = TRUE;
#if defined(FEATURE_DUAL_SIM )
                        if(MNCNM_IS_SINGLE_STACK)
                        {
                           if(mn_call_information_p[transaction_id] != NULL)
                           {
                             mn_as_id = (sys_modem_as_id_e_type)mmcnm_data_ind->data[length -3];/* length accomodates ti_pd + type + data + as id */
                             mn_call_information_p[transaction_id]->as_id = mn_as_id;
                             MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Setting mn_as_id to %d",mn_as_id);
                           }
                        }
#endif
                   }
                   else
                   {
                       ERR("ERR:CNM rec'd SETUP msg for active TI_PD %#X",mmcnm_data_ind->msg_head.ti_pd,0,0);
                   }
        
               }
               else
               {
                    ERR("ERR:Tranaction id not in MT call range %d",transaction_id,0,0);    
               }
           }
           
           break;

       case PD_SS:
           if (mmcnm_data_ind->msg_head.type == REGISTER) 
           {
               est_ind = TRUE;
#if defined(FEATURE_DUAL_SIM )
               if(MNCNM_IS_SINGLE_STACK)
               {
                  mn_as_id = (sys_modem_as_id_e_type)mmcnm_data_ind->data[length -3];
                  ss_transaction_information[transaction_id].as_id = mn_as_id; 
                  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Setting mn_as_id to %d",mn_as_id);
               }
#endif
           }
           else
           {
               est_ind = FALSE;
           }
           break;

       case PD_SMS:
           if ((mmcnm_data_ind->msg_head.type == CP_DATA) &&
               ((sms_transaction_information[transaction_id].sms_state == SMS_IDLE) ||
                  (sms_transaction_information[transaction_id].sms_state == SMS_WAIT_CP_ACK))) 
               /* Accept new MT SMS when another MT SMS is waiting for Final CP ACK for the same TI  */
           {
               est_ind = TRUE;
#if defined(FEATURE_DUAL_SIM )
               if(MNCNM_IS_SINGLE_STACK)
               {
                  mn_as_id = (sys_modem_as_id_e_type)mmcnm_data_ind->data[length -3];
                  sm_rl_transaction_information[transaction_id].as_id = mn_as_id;
                  MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Setting mn_as_id to %d",mn_as_id);
               }
#endif
           }
           /*
           * Note that we could receive a CP_DATA(RP_ACK) for an active transaction.
           * The above check should be for a CP-DATA(RP-DATA)
           */
           break;
       default:
           break;
       }
    }
    
    if (transaction_id == UNKNOWN_CONNECTION_ID)
    {
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Received MMCNM_DATA_IND w/ reserved TI");
    }

    return est_ind;
}
/**************** End of   CNM_check_for_establish_ind **********************************/



/**************** Begin of   CNM_return_calltype ********************************
 *
 *  Function name: CNM_return_calltype(void)
 *  -------------------------------------
 *
 *  Description:
 *  ------------
 *  This function returns  type of call 
 *  This function is added to pass call type from MM in RRC_EST_REQ
 *  Parameters:
 *  -----------
 *  :IN 
 *
 *  Returns:
 *  --------
 *  call_type_for_rrc_est_req_T
 *
 ******************************************************************************/

call_type_for_rrc_est_req_T CNM_return_calltype()
{
  call_type_for_rrc_est_req_T call_type = OTHER_CALL;

  switch (mm_connection_pending.CNM_service_type)
  {
     case MO_CALL_ESTABLISHMENT:
     case MO_CALL_ESTABLISHMENT_FULL_SPEECH:
     case MO_CALL_ESTABLISHMENT_HALF_SPEECH:
     case EMERGENCY_CALL_ESTABLISHMENT:
          call_type = SPEECH_CALL;
        break;

     case MO_CALL_ESTABLISHMENT_DATA_9600:
     case MO_CALL_ESTABLISHMENT_DATA_4800:
     case MO_CALL_ESTABLISHMENT_DATA_2400:

          if(mm_connection_pending.mmcnm_data_req.data[2] == 0x04)
          { /* if repeat indicator is not present, third byte contains ITC
               0x01 and 0x02 corresponds to ITC_UDI and ITC_3_1K */

            if(((mm_connection_pending.mmcnm_data_req.data[4] & 0x07) == 0x01) ||
               ((mm_connection_pending.mmcnm_data_req.data[4] & 0x07) == 0x02))
            {
              call_type = VIDEO_CALL;
            }
            else if((mm_connection_pending.mmcnm_data_req.data[4] & 0x07) != 0x03)
            {
               ERR("Unexpected ITC value in SETUP = %d\n",
               (mm_connection_pending.mmcnm_data_req.data[4] & 0x07), 0, 0);
            }
          }
          else if((mm_connection_pending.mmcnm_data_req.data[2] & 0xf0) == 0xd0)
          { /* if repeat indicator is present, foruth byte contains ITC
               0x01 and 0x02 corresponds to ITC_UDI and ITC_3_1K */
            if(((mm_connection_pending.mmcnm_data_req.data[5] & 0x07) == 0x01) ||
              ((mm_connection_pending.mmcnm_data_req.data[5] & 0x07) == 0x02))
            {
              call_type = VIDEO_CALL;
            }
            else if((mm_connection_pending.mmcnm_data_req.data[4] & 0x07) != 0x03)
            {
             ERR("Unexpected ITC value in SETUP = %d\n",
             (mm_connection_pending.mmcnm_data_req.data[5] & 0x07), 0, 0);
            }
          }
          else
          {
            ERR("Unexpected IEI in SETUP = %d\n",
            (mm_connection_pending.mmcnm_data_req.data[2]), 0, 0);
          }
        break;

     case SHORT_MESSAGE_SERVICE_ESTABLISHMENT:
     case SUPPLEMENTARY_SERVICE_ACTIVATION:
          call_type = OTHER_CALL;
        break;

     default:
          ERR("Unexpected CNM_service_type in MO call_request: received CNM_service_type = %d\n",
               (mm_connection_pending.CNM_service_type), 0, 0);
       break;
  }

 return call_type;
}
/**************** End of   CNM_check_for_establish_ind **********************************/

