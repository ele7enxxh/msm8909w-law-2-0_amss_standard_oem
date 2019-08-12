/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_cc_incall_mod.c_v   1.3   11 Mar 2002 18:33:26   vtawker  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_cc_incall_mod.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/01   CD      Replaced mn header files by single include of mn.h

08/14/01   AB      Renamed connection manager references to CNM from CM.

11-Mar-2002   VT   Removed Lint errors.
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
#include "ms.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"

#include "cause.h"


#include "cmlib.h"
#include "nasutils.h"

#include "cnm.h"
#include <stringl/stringl.h>



/**************** Begin of   CNM_process_modify_message ***************************
 *
 *  Function name: CNM_process_modify_message()
 *  ------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a modify message received from the network. It walks
 *  through the IEs in the message checking for validity, and if all ok, sends
 *  the request on to MN, otherwise a status message is sent.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_modify_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{

   byte                       *p_IE;
   byte                       *p_write_point;
   byte                       *last_octet_write_point;
   byte                       IEI;
   byte                       IE_count;
   byte                       IE_length;
   dword                      len;
   MNCC_MODIFY_IND_T          mncc_modify_ind;
   p_IE = (byte *) 0;
   IE_count = 2;              /* start at first IE after message type */

   mncc_modify_ind.message_header.message_set = MS_MN_CC;
   mncc_modify_ind.message_header.message_id  = MNCC_MODIFY_IND;
   mncc_modify_ind.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_modify_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   p_write_point = (byte *)mncc_modify_ind.data;
   last_octet_write_point = &mncc_modify_ind.data[sizeof(mncc_modify_ind.data)-1];

   while ( (syntax_ok != FALSE) && ( IE_count < no_of_IEs ))
   {

  /*
   * get information of the next IE from message
   */
      IEI       = cc_IE_info[IE_count].IEI;
      p_IE      = cc_IE_info[IE_count].start;
      IE_length = cc_IE_info[IE_count].length;

      if(cc_IE_info[IE_count].syntax_ok == TRUE)
      {
        switch (IEI)
        {
           case IEI_BEARER_CAPABILITY:
              {
                (void)memscpy( p_write_point, (last_octet_write_point - p_write_point + 1), p_IE, p_IE[1]+2);
                /*
                 * There is no IEI for bearer capability in the input
                 * message, so add one
                 */
                *p_write_point = IEI_BEARER_CAPABILITY;
                p_write_point = ((byte *)(p_write_point)) + p_IE[1]+2;
              }
              break;

           case IEI_LOW_LAYER_COMPATIB:
              {
                (void) memscpy(p_write_point, (last_octet_write_point - p_write_point + 1), p_IE, p_IE[1]+2);
                p_write_point = ((byte *)(p_write_point)) + p_IE[1]+2;
              }
              break;

           case IEI_HIGH_LAYER_COMPATIB:
              {
                (void)memscpy(p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, IE_length);
                p_write_point = ((byte *)(p_write_point)) + IE_length;
              }
              break;

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
           case IEI_REVERSE_CALL_SETUP_DIR:
              {
                *p_write_point = ((ie_reverse_call_setup_dir_T *)p_IE)->Iei;
                p_write_point++;
              }
              break;

           case IEI_IMMEDIDATE_MOD_IND:
              {
                *p_write_point = ((ie_immediate_modify_ind_T *)p_IE)->Iei;
                p_write_point++;
              }
              break;

           case IEI_NW_INITIATED_SVC_UPGRADE_IND:
              {
                *p_write_point = ((ie_nw_initiated_svc_upgrade_ind_T *)p_IE)->Iei;
                p_write_point++;
              }
              break;
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

           default:                            /* this cannot happen! */
              ERR("Unexpected IE in modify msg: received IEI = %d\n",
                     (dword) IEI, 0, 0);

              syntax_ok = FALSE;
              break;
        }     /* end case */
      }

      IE_count++;       /* index next IE in the message */

   }   /* END WHILE more IEs and Syntax is OK */

   if ( syntax_ok != FALSE )
   {

      len = (dword)( (byte *)p_write_point -
                                 (byte *)&mncc_modify_ind.as_id);
      PUT_IMH_LEN( len,
                   &mncc_modify_ind.message_header);

      CNM_send_message((IMH_T *)&mncc_modify_ind,
                       GS_QUEUE_MN_CM );
   }
   else
   {
     CNM_send_status ( connection_id,
                                                 PD_CC,
                                                 cause );
   }

}
/**************** End of   CNM_process_modify_message ***************************/


/**************** Begin of   CNM_process_modify_complete_reject_message ***********
 *
 *  Function name: CNM_process_modify_complete_reject_message()
 *  ----------------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a modify complete or modify reject message
 *  received from the network. It walks
 *  through the IEs in the message checking for validity, and if all ok, sends
 *  a confirm on to MN, otherwise a status message is sent.
 *  This function assumes that the message has already been checked for IE
 *  content
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_modify_complete_reject_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{

   byte                       *p_IE;
   byte                       *p_write_point;
   byte                       *last_octet_write_point;
   byte                       IEI;

   byte                       IE_count;
   byte                       IE_length;

   dword                      len;

   MNCC_MODIFY_CNF_T          mncc_modify_cnf;


   p_IE = (byte *) 0;

   IE_count = 2;              /* start at first IE after message type */

   mncc_modify_cnf.message_header.message_set = MS_MN_CC;
   mncc_modify_cnf.message_header.message_id  = MNCC_MODIFY_CNF;
   mncc_modify_cnf.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_modify_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   p_write_point = (byte *)mncc_modify_cnf.data;
   last_octet_write_point = (byte *)&mncc_modify_cnf.data[sizeof(mncc_modify_cnf.data)-1];

   while ( (syntax_ok != FALSE) && ( IE_count < no_of_IEs ))
   {

  /*
   * get information of the next IE from message
   */
      IEI       = cc_IE_info[IE_count].IEI;
      p_IE      = cc_IE_info[IE_count].start;
      IE_length = cc_IE_info[IE_count].length;
      /* 
       * This length has been sanitized and truncated if necessary in function CNM_perform_check_on_IEs()
       */

      if(cc_IE_info[IE_count].syntax_ok == TRUE)
      {
        switch (IEI)
        {
           case IEI_CAUSE:
              {
                  (void)memscpy(p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, IE_length);

                  /*
                   * set the IEI for this case as its missing from the
                   * input message
                   */

                  *p_write_point = IEI_CAUSE;
                  p_write_point = ((byte *)(p_write_point)) + IE_length;
              }
              break;

           case IEI_BEARER_CAPABILITY:
              {

                  (void)memscpy(p_write_point, (last_octet_write_point - p_write_point + 1), p_IE, p_IE[1] + 2);
                      /*
                       * set the IEI for this case as its missing from the
                       * input message
                       */
                  *p_write_point = IEI_BEARER_CAPABILITY;
                  p_write_point = ((byte *)(p_write_point)) + p_IE[1] + 2;

              }
              break;

           case IEI_LOW_LAYER_COMPATIB:
              {

                  (void)memscpy(p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, p_IE[1] + 2);
                  p_write_point = ((byte *)(p_write_point)) + p_IE[1] + 2;

              }
              break;

           case IEI_HIGH_LAYER_COMPATIB:
              {
                  (void)memscpy(p_write_point, (last_octet_write_point - p_write_point + 1) ,p_IE, IE_length);
                  p_write_point = ((byte *)(p_write_point)) + IE_length;

              }
              break;

           case IEI_REVERSE_CALL_SETUP_DIR:
              {
                *p_write_point =
                 ((ie_reverse_call_setup_dir_T *)p_IE)->Iei;

                p_write_point++;
              }
              break;

           default:                            /* this cannot happen! */
              ERR("Unexpected IE in Modify Complete Rej msg: received IEI = %d\n",
                   (dword) IEI, 0, 0);

              syntax_ok = FALSE;
              break;
        }     /* end case */
      }


      IE_count++;       /* index next IE in the message */


   }   /* END WHILE more IEs and Syntax is OK */


   if ( syntax_ok )
   {
      len = (dword)( (byte *)p_write_point -
               (byte *)&mncc_modify_cnf.as_id);

      PUT_IMH_LEN( len,
                   &mncc_modify_cnf.message_header);

      CNM_send_message((IMH_T *)&mncc_modify_cnf,
                       GS_QUEUE_MN_CM );
   }
   else
   {
                 CNM_send_status ( connection_id,
                  PD_CC,
                  cause );
   }
}
/**************** End of   CNM_process_modify_complete_reject_message ******************/


/**************** Begin of CNM_process_mo_modify_request **************************
 *
 *  Function name: CNM_process_mo_modify_request()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle a request for an in-call modification.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a modify request and sends it on.
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mo_modify_request( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_MODIFY_REQ_T    *p_mncc_modify_req;
   modify_msg_T         data_req_msg;
   byte                 action;
   unsigned int         length;
   word                 bytes_remaining;

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_modify_req = (MNCC_MODIFY_REQ_T *)((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_modify_req->connection_id;

   action =
        CNM_cc_check_respond_to_event(connection_id, MNCC_MODIFY_REQ_event );

#ifdef FEATURE_WCDMA
   mn_store_rab_status();
#endif


   if ( action != 0 )
   {
      CNM_cc_respond_to_event(connection_id, action );

      data_req_msg.msg_head.type  = MODIFY;


      GET_IMH_LEN(length, &p_mncc_modify_req->message_header );

      bytes_remaining = (unsigned short) (length - sizeof(connection_id_T) - sizeof(sys_modem_as_id_e_type));

      if ( bytes_remaining != 0 )
      {
         bytes_remaining--;   /* allow for removal of IEI */

         if(bytes_remaining > MIN(sizeof(p_mncc_modify_req->data) -1, sizeof(data_req_msg.data)))
         {
            bytes_remaining = MIN(sizeof(p_mncc_modify_req->data) -1, sizeof(data_req_msg.data)); /* Protective check before memcpy */
         }

         (void)memscpy( data_req_msg.data,
                       sizeof(data_req_msg.data),
                       &p_mncc_modify_req->data[1],   /* skip Bearer Capab IEI */
                       bytes_remaining );

         PUT_IMH_LEN(bytes_remaining +
                      sizeof(data_req_msg.msg_head.ti_pd) +
                      sizeof(data_req_msg.msg_head.type),
                      &data_req_msg.msg_head.IMH );


         CNM_send_primitive_to_mm(connection_id,
                                 PD_CC,
                                 (IMH_T *)&data_req_msg
                                         );

      }
      else
      {
         ERR("Incomplete Modify Req msg: remaining bytes = %d\n",
              (dword) bytes_remaining, 0, 0);
      }

   }

}
/**************** End of   CNM_process_mo_modify_request ***************************/


/**************** Begin of   CNM_process_mt_modify_response ***********************
 *
 *  Function name: CNM_process_mt_modify_response()
 *  ----------------------------------------------
 *
 *  Description:
 *  ------------
 * Function to handle the message sent by MN in response to a MNCM_MODIFY_IND
 * message, as part of the in-call modification procedure. The content of the
 * message indicates either success, in which case a modify complete message
 * is sent back to the network, or a failure (indicated by a cause IE being
 * present in the message), in which case a MODIFY REJECT message is sent back
 * to the network.
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mt_modify_response( IMH_T * message )
{
   connection_id_T         connection_id;
   MNCC_MODIFY_RES_T       *p_mncc_modify_res;
   modify_complete_msg_T   modify_comp_msg;
   modify_reject_msg_T     modify_rej_msg;
   byte                    action;
   boolean                 fail;
   byte                    *p_ie_cause;
   byte                    *p_cause;
   word                    length_processed;
   unsigned int            msg_length;
   word                    bytes_remaining;

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_modify_res = (MNCC_MODIFY_RES_T *)((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_modify_res->connection_id;


   action =
        CNM_cc_check_respond_to_event(connection_id, MNCC_MODIFY_RES_event );

#ifdef FEATURE_WCDMA
   mn_store_rab_status();
#endif

   if ( action != 0 )
   {
      fail = FALSE;

      /*
       * set up pointer. Note that this may not actually be a cause IE -
       * This is determined later
       */

      p_ie_cause = (byte *)(p_mncc_modify_res->data);

      GET_IMH_LEN(msg_length, &p_mncc_modify_res->message_header );
      msg_length -= sizeof(sys_modem_as_id_e_type); /* Size of as_id is decreased as as_id is added in the structure*/

      length_processed = 0;

      while ((length_processed + sizeof(connection_id_T)) < msg_length )
      {
         if ( (p_ie_cause[0] & 0x80 ) != 0 )      /* if type 1 or 2 IE */
         {
            length_processed += 1;

            /*
             * point at next IE
             */
            if(length_processed >= (
                               sizeof(ie_bearer_capability_T) +
                               GSM_CAUSE_SIZE +
                               sizeof(ie_low_layer_compatibility_T) +
                               sizeof(ie_high_layer_compatibility_T) +
                               sizeof(ie_reverse_call_setup_dir_T)
                            ))
            {
             
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds abd buffer overflow");
              break;
            }
            else
            { 
              p_ie_cause = (byte *)
                           &(p_mncc_modify_res->data[length_processed]);
            }  
         }
         else
         {

            /*
             * check if cause IEI is present, and therefore cause IE
             * IF it is, then this is a modify fail
             */

            if ( p_ie_cause[0] == IEI_CAUSE )
            {
               fail = TRUE;
               break;
            }
            else
            {
               length_processed = (unsigned short) (length_processed + p_ie_cause[1]
                                                 + 2*sizeof(byte));
               if(length_processed >= (
                               sizeof(ie_bearer_capability_T) +
                               GSM_CAUSE_SIZE +
                               sizeof(ie_low_layer_compatibility_T) +
                               sizeof(ie_high_layer_compatibility_T) +
                               sizeof(ie_reverse_call_setup_dir_T)
                            ))
               {
             
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds abd buffer overflow");
                 break;
               }
               else
               { 
                 p_ie_cause = (byte *)
                              &(p_mncc_modify_res->data[length_processed]);
               }
            }
         }

      }   /* end of WHILE */

      CNM_cc_respond_to_event(connection_id, action );

      if ( fail == FALSE )
      {

         modify_comp_msg.msg_head.type  = MODIFY_COMPLETE;

         bytes_remaining = (unsigned short) (msg_length - sizeof(connection_id_T));

         if ( bytes_remaining != 0 )
         {
            bytes_remaining--;      /* allow for removal of IEI */

            (void)memscpy(modify_comp_msg.data,
                         sizeof(modify_comp_msg.data),
                         &p_mncc_modify_res->data[1],
                         bytes_remaining );

            PUT_IMH_LEN(bytes_remaining +
                         sizeof(modify_comp_msg.msg_head.ti_pd) +
                         sizeof(modify_comp_msg.msg_head.type),
                        &modify_comp_msg.msg_head.IMH );

            CNM_send_primitive_to_mm(connection_id,
                              PD_CC,
                              (IMH_T *)&modify_comp_msg);
         }
         else
         {
            ERR("Incomplete Modify Resp msg: remaining bytes = %d\n",
                 (dword) bytes_remaining, 0, 0);
         }


      }
      else
      {
         modify_rej_msg.msg_head.type  = MODIFY_REJECT;

         bytes_remaining = (unsigned short) (msg_length - sizeof(connection_id_T));

         if ( bytes_remaining != 0 )
         {
            bytes_remaining--;      /* allow for removal of IEI */
            if(bytes_remaining < (
                               sizeof(ie_bearer_capability_T) +
                               GSM_CAUSE_SIZE +
                               sizeof(ie_low_layer_compatibility_T) +
                               sizeof(ie_high_layer_compatibility_T) +
                               sizeof(ie_reverse_call_setup_dir_T)
                            ))
            {

              (void)memscpy(modify_rej_msg.data,
                         sizeof(modify_rej_msg.data),
                         &p_mncc_modify_res->data[1],
                         bytes_remaining );
            }
         /*
          * still need to remove the cause IEI from the output message
          * so need to check its there, locate it, and copy everything else
          * down 1 byte
          */

            if (bytes_remaining > ( p_mncc_modify_res->data[1] + 1 ) )
            {
               byte index_temp = p_mncc_modify_res->data[1] + 1;
               word p_cause_buf_len;
               if(index_temp < (MAX_OCTETS_L3_MESSAGE - 2))
               {
                 p_cause = (byte *)&( modify_rej_msg.data [index_temp]);
                 p_cause_buf_len = sizeof(modify_rej_msg.data) - index_temp;
               /*
                * copy the cause IEI down, along with everything else
                */

               (void)memscpy(&p_cause[0],
                            p_cause_buf_len,
                            &p_cause[1],
                            bytes_remaining - ( p_mncc_modify_res->data[1] ) );
               }
               else
               { 
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds abd buffer overflow");
               }
               bytes_remaining--;
            }  
         }
         else
         {
            bytes_remaining = 0;
         }

         PUT_IMH_LEN(bytes_remaining +
                         sizeof(modify_rej_msg.msg_head.ti_pd) +
                         sizeof(modify_rej_msg.msg_head.type),
                        &modify_rej_msg.msg_head.IMH );

         CNM_send_primitive_to_mm(connection_id,
                              PD_CC,
                              (IMH_T *)&modify_rej_msg );

      }  /* END else  fail == FALSE */

   }    /* END if respond_to_event != FALSE */

}
/**************** End of   CNM_process_mt_modify_response ***************************/


/**************** Begin of   CNM_process_mo_modify_accept *************************
 *
 *  Function name: CNM_process_mo_modify_accept()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle notification of acceptance or otherwise of an in-
 *  call modification originating from the mobile
 *
 *
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mo_modify_accept( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_MODIFY_ACCEPT_T *p_mncc_modify_accept;

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_modify_accept = (MNCC_MODIFY_ACCEPT_T *)((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_modify_accept->connection_id;

   /* 
    * if modify was rejected, need to replace the system state to be
    * CC_STATE_U26 as if we never got the message in the first place, otherwise
    * do nothing
    */

   if(cc_transaction_information_p[connection_id] != NULL)
   {
   
     if ( p_mncc_modify_accept->accept == FALSE )
     {
        cc_transaction_information_p[connection_id]->cc_state = CC_STATE_U26;
     }
     else
     {
        cc_transaction_information_p[connection_id]->cc_state = CC_STATE_U10;
#ifdef FEATURE_WCDMA
        mn_store_rab_status();
#endif
     }
   }
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot process MNCC_MODIFY_ACCEPT as cc_transaction_information_p[%d] is NULL",connection_id);    
   }
}
/**************** End of   CNM_process_mo_modify_accept ***************************/

/**************** Begin of   CNM_process_t324_expiry ***********************
 *
 *  Function name: CNM_process_t324_expiry()
 *  ----------------------------------------------
 *
 *  Description:
 *  ------------
 * 
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_t324_expiry( connection_id_T connection_id )
{

  MNCC_MODIFY_CNF_T          mncc_modify_cnf;
  modify_reject_msg_T        modify_rej_msg;
  modify_reject_msg_T        modify_rej_msg_temp;
  byte                       *data_ptr; 
  byte                       *last_octet_data_ptr; 
  int                        data_index;
  int                        index;
  int                        message_length;
  mn_call_information_T      *call_data_ptr;

  if(mn_call_information_p[connection_id] != NULL)
  {
    call_data_ptr = mn_call_information_p[connection_id];
    memset(&modify_rej_msg, '\0', sizeof(modify_reject_msg_T));
    modify_rej_msg.msg_head.type  = MODIFY_REJECT;

    data_index = 0;
    message_length = 0;

    /*  SET UP BEARER CAPABILITY DATA  */
    data_ptr = &modify_rej_msg_temp.data[data_index];
    last_octet_data_ptr = &modify_rej_msg_temp.data[sizeof(modify_rej_msg_temp.data) -1];
    if ( call_data_ptr->active_bearer_capability == 0 )
    {
      index = MN_put_bearer_capability(data_ptr, 
                                       &call_data_ptr->bearer_capability_1,
                                       last_octet_data_ptr);
    }
    else
    {
      index = MN_put_bearer_capability(data_ptr, 
                                       &call_data_ptr->bearer_capability_2,
                                       last_octet_data_ptr);
    }
    message_length += index;
    message_length--;

    message_length = memscpy(modify_rej_msg.data,
                     sizeof(modify_rej_msg.data),
                     &modify_rej_msg_temp.data[1],
                     (unsigned int)message_length);

    data_index += message_length;

    /*  SET UP THE CAUSE DATA ONLY FOR A `REJECT' MESSAGE  */
    modify_rej_msg.data[data_index++] = 2;          /* length, in octets*/
    modify_rej_msg.data[data_index++] = 0xE0;       /* bit 8 - 1 no recommendation present, bit7 and bit 8 - coding standard, bit1-bit4 - location  */
    modify_rej_msg.data[data_index++] = (0x80 | BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE) ;           /* cause value*/

    message_length += 3;

    if ( call_data_ptr->active_bearer_capability == 0 )
    {
      /*  SET UP THE LOW LAYER COMPATIBILITY DATA  */
      /*  THERE CAN BE ONLY 0 OR 1 LOW LAYER COMPATIBILITIES  */
      if ( (call_data_ptr->repeat_indication_llc == 1) &&
           (call_data_ptr->low_layer_compatibility_1.present == TRUE) )
      {
        data_ptr = &modify_rej_msg.data[data_index];
        index = MN_put_llc_data(data_ptr, 
                                &call_data_ptr->low_layer_compatibility_1,
                                last_octet_data_ptr);
        data_index += index;
        message_length += index;
      }
      /*  SET UP THE HIGH LAYER COMPATIBILITY DATA  */
      /*  THERE CAN BE ONLY 0 OR 1 HIGH LAYER COMPATIBILITIES  */
      if ( (call_data_ptr->repeat_indication_hlc == 1) &&
           (call_data_ptr->high_layer_compatibility_1.present == TRUE) )
      {
    
        if(data_index >= (MAX_OCTETS_L3_MESSAGE - 2))
        {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds abd buffer overflow"); 
        }
        else
        {
          data_ptr = &modify_rej_msg.data[data_index];
          index = MN_put_hlc_data(data_ptr, 
                              &call_data_ptr->high_layer_compatibility_1,
                              last_octet_data_ptr);
          data_index += index;
          message_length += index;
        }
      }
    }
    else
    {
      /*  SET UP THE LOW LAYER COMPATIBILITY DATA  */
      /*  THERE CAN BE ONLY 0 OR 1 LOW LAYER COMPATIBILITIES  */
      if ( (call_data_ptr->repeat_indication_llc == 2) &&
           (call_data_ptr->low_layer_compatibility_2.present == TRUE) )
      {

        data_ptr = &modify_rej_msg.data[data_index];
        index = MN_put_llc_data(data_ptr, 
                              &call_data_ptr->low_layer_compatibility_2,
                              last_octet_data_ptr);
        data_index += index;
        message_length += index;
      }
      /*  SET UP THE HIGH LAYER COMPATIBILITY DATA  */
      /*  THERE CAN BE ONLY 0 OR 1 HIGH LAYER COMPATIBILITIES  */
      if ( (call_data_ptr->repeat_indication_hlc == 2) ||
         (call_data_ptr->high_layer_compatibility_2.present == TRUE) )
      {
        if(data_index >= (MAX_OCTETS_L3_MESSAGE - 2))
        {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds abd buffer overflow"); 
        }
        else
        {
          data_ptr = &modify_rej_msg.data[data_index];
          index = MN_put_hlc_data(data_ptr, 
                              &call_data_ptr->high_layer_compatibility_2,
                              last_octet_data_ptr);
          data_index += index;
          message_length += index;
        }
      }

    }

    /*  SET THE MESSAGE LENGTH  */
    PUT_IMH_LEN((unsigned int)message_length +
                sizeof(modify_rej_msg.msg_head.ti_pd) +
                sizeof(modify_rej_msg.msg_head.type),
                &modify_rej_msg.msg_head.IMH );     


    /*  SEND THE MESSAGE  */
    CNM_send_primitive_to_mm(connection_id,
                             PD_CC,
                             &modify_rej_msg.msg_head.IMH );

    memset(&mncc_modify_cnf, '\0', sizeof(MNCC_MODIFY_CNF_T));

    mncc_modify_cnf.message_header.message_set = MS_MN_CC;
    mncc_modify_cnf.message_header.message_id  = MNCC_MODIFY_CNF;
    mncc_modify_cnf.message_header.message_len_lsb = 4 + sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type);
    mncc_modify_cnf.message_header.message_len_msb = 0;

    mncc_modify_cnf.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
    {
      mncc_modify_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
    }
#endif
    mncc_modify_cnf.data[0] = IEI_CAUSE;
    mncc_modify_cnf.data[1] = 2;          /* length, in octets*/
    mncc_modify_cnf.data[2] = 0xE0;       /* bit 8 - 1 no recommendation present, bit7 and bit 8 - coding standard, bit1-bit4 - location  */
    mncc_modify_cnf.data[3] = (0x80 | RECOVERY_ON_TIMER_EXPIRY) ;           /* cause value*/

    CNM_send_message((IMH_T *)&mncc_modify_cnf, GS_QUEUE_MN_CM );
  }
  else
  {
    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot process T324 expiry as mn_call_information_p[%d] is NULL", connection_id);
  }  
 }
/**************** End of   CNM_process_t324_expiry ***************************/
