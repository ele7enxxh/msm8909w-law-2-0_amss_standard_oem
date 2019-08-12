/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_serv_req_queue.c_v   1.5   18 Jul 2002 12:36:46   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_serv_req_queue.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

04/19/02   CD      Updated Copyright
                   Replaced ERR by MSG_ERROR

05/15/02   CD      In CNM_manage_cnm_service_requests(), added starting timer 
                   T303 when a MMCNM_EST_REQ is sent

07/16/02   AB      In CNM_manage_cnm_service_requests, added checking for
                   pending release request, mm_rel_request_sent, prior
                   to sending an establish request, in order to avoid 
                   being rejected by MM.

08/02/02   CD      Added debug messages

10/02/02   AB      In CNM_manage_cnm_service_requests(), if queueing of
                   MM service request is not possible, then reject the 
                   service request.

12/16/02   AB      In CNM_manage_cnm_service_request(), don't limit
                   queueing of the service request if CNM is not in 
                   release requesting state.

02/10/03   AB      Removed lint errors. 
 
04/16/03   AB      Updated to new connection reject interface function all 
                   CNM_setup_MNCC_REJ_IND().
                   
12/13/05   NR      Fixed Lint errors for LINT 8.0

02/11/06   NR      Fixing lint errors

04/09/07   RD      SMS optimization changes are incorporated
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"
#include "cause.h"


#include "environ.h"
#include "ms.h"
#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"

#include "nasutils.h"
#include "cmlib.h"
#include "cnm.h"
#include "cm_sms.h"
#include <stringl/stringl.h>


/*
 * global data declaration
 */

#define MAXNO_CNM_SERVICE_REQ_QUEUE_ENTRIES     15


#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)

static byte cnm_service_req_read_ptr_sim[MAX_AS_IDS];
static byte cnm_service_req_write_ptr_sim[MAX_AS_IDS];
static cnm_request_id_T      *cnm_service_req_queue_sim[MAX_AS_IDS][
                              MAXNO_CNM_SERVICE_REQ_QUEUE_ENTRIES];

#define cnm_service_req_read_ptr (cnm_service_req_read_ptr_sim[mn_dsda_as_id])
#define cnm_service_req_write_ptr (cnm_service_req_write_ptr_sim[mn_dsda_as_id])
#define cnm_service_req_queue (cnm_service_req_queue_sim[mn_dsda_as_id])

#else

static byte cnm_service_req_read_ptr;
static byte cnm_service_req_write_ptr;

/*
 * The queue contains cnm_request_id entries.
 */

static cnm_request_id_T      *cnm_service_req_queue[
                              MAXNO_CNM_SERVICE_REQ_QUEUE_ENTRIES];

#endif



/**************** Begin of   CNM_init_cnm_service_request_queue *********************
 *
 *  Function name: CNM_init_cnm_service_request_queue
 *  -----------------------------------------------
 *  Description:
 *  ------------
 *  This function initialises the cnm_service_request queue by setting the read
 *  and write pointers to zero.
 *
 *  Parameters:
 *  -----------
 *  None
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_init_cnm_service_request_queue(void)
{
   cnm_service_req_read_ptr = 0;
   cnm_service_req_write_ptr = 0;
   MSG_LOW_DS_0(MN_SUB,"=MNCNM= CNM serv req queue is empty");

}

/**************** End of   CNM_init_cnm_service_request_queue ********************/


/**************** Begin of   CNM_add_to_cnm_service_request_queue ******************
 *
 *  Function name: CNM_add_to_cnm_service_request_queue
 *  -------------------------------------------------
 *  Description:
 *  ------------
 *  This function adds an entry pointed to by the input parameter to the
 *  cnm_service_request queue.
 *
 *  Parameters:
 *  -----------
 *  cnm_request_id - pointer to detail which is to be added to queue
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_add_to_cnm_service_request_queue(cnm_request_id_T *cnm_request_id)
{
   byte   WritePtr;

/*
 * check if queue is full first - if write pointer would become equal
 * to the read pointer after the write, then the queue is already as
 * full as it can be. First calculate what the write pointer would contain
 * after the update
 */

   WritePtr = (unsigned char)((cnm_service_req_write_ptr+1) % MAXNO_CNM_SERVICE_REQ_QUEUE_ENTRIES);

   if (WritePtr != cnm_service_req_read_ptr)
   {
      cnm_service_req_queue[cnm_service_req_write_ptr] = (cnm_request_id_T*) modem_mem_calloc( 1, sizeof(cnm_request_id_T),MODEM_MEM_CLIENT_NAS );
      if (NULL != cnm_service_req_queue[cnm_service_req_write_ptr])
      {
        (void) memscpy( cnm_service_req_queue[cnm_service_req_write_ptr],
                        sizeof(cnm_request_id_T),
                        cnm_request_id,
                        sizeof(cnm_request_id_T) );
        
        // update stored write pointer to point at next location
        cnm_service_req_write_ptr = WritePtr;
        MSG_LOW_DS_0(MN_SUB,"=MNCNM= Added to CNM serv req queue");
      }
      else
      {
        MSG_ERROR_DS_0(MN_SUB,"=MNCNM= modem_mem_calloc failed, NULL Pointer\n");
      }

   }
   else
   {

      /*
       * queue is full
       */

      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= cm_service_req_queue full\n");
   }

}
/**************** End of   CNM_add_to_cnm_service_request_queue ********************/


/**************** Begin of   CNM_pop_cnm_service_request_queue *********************
 *
 *  Function name: CNM_pop_cnm_service_request_queue()
 *  ------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function removes the top entry from the cnm_service_request queue and
 *  returns it to the caller
 *
 *  Parameters:
 *  -----------
 *  cnm_request_id : INOUT - pointer to area to receive top entry of queue
 *
 *  Returns:
 *  --------
 *  boolean - TRUE if queue was not empty
 *
 ******************************************************************************/

boolean CNM_pop_cnm_service_request_queue(cnm_request_id_T *cnm_request_id)
{
  boolean return_value;

  return_value = CNM_read_cnm_service_request_queue(cnm_request_id);

  if (return_value != FALSE)
  {

    // queue was not empty
    // update stored read pointer to point at next location

    CNM_free( cnm_service_req_queue[cnm_service_req_read_ptr] );
    cnm_service_req_queue[cnm_service_req_read_ptr] = NULL;
    
    cnm_service_req_read_ptr = (unsigned char)((cnm_service_req_read_ptr + 1) %
                                MAXNO_CNM_SERVICE_REQ_QUEUE_ENTRIES);
    MSG_LOW_DS_0(MN_SUB,"=MNCNM= Popped from CNM serv req queue");
  }
  return (return_value);
}
/**************** End of   CNM_pop_cnm_service_request_queue ********************/


/**************** Begin of   CNM_read_cnm_service_request_queue ********************
 *
 *  Function name: CNM_read_cnm_service_request_queue
 *  -----------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function reads the top entry from the cnm_service_request queue and
 *  returns it to the caller
 *
 *  Parameters:
 *  -----------
 *  cnm_request_id : INOUT - pointer to area to receive data from queue
 *
 *  Returns:
 *  --------
 *  success - Boolean value, TRUE if read successfull
 *
 ******************************************************************************/

boolean CNM_read_cnm_service_request_queue(cnm_request_id_T *cnm_request_id)
{
   boolean return_value;

   /*
    * check if queue is not empty first - if write pointer is equal
    * to the read pointer, then the queue is empty
    */


   if (cnm_service_req_read_ptr == cnm_service_req_write_ptr)
   {

      /*
       * queue is empty
       */

      cnm_request_id->CNM_service_type = NO_CNM_SERVICE;
      return_value = FALSE;
   }
   else
   {
      (void) memscpy( cnm_request_id,
                      sizeof(cnm_request_id_T),
                      cnm_service_req_queue[cnm_service_req_read_ptr],
                      sizeof(cnm_request_id_T) );
      return_value = TRUE;

   }
   return(return_value);

}
/**************** End of   CNM_read_cnm_service_request_queue ********************/



/**************** Begin of   CNM_check_cnm_service_request_queue ********************
 *
 *  Function name: CNM_check_cnm_service_request_queue
 *  -----------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function checks if cnm service request queue has an entry of a particular type
  *
 *  Parameters:
 *  -----------
 *  cnm_request_id : INOUT - pointer to area to receive data from queue
 *
 *  Returns:
 *  --------
 *  success - Boolean value, TRUE if entry exists.
 *
 ******************************************************************************/

boolean CNM_check_cnm_service_request_queue(CNMServiceType_T CNM_service_type_arg)
{
   byte local_read_index = cnm_service_req_read_ptr;

   while(local_read_index != cnm_service_req_write_ptr)
   {
      if (cnm_service_req_queue[local_read_index] != NULL && 
          cnm_service_req_queue[local_read_index]->CNM_service_type == CNM_service_type_arg)
      {
        return TRUE;
      }
      local_read_index = (unsigned char)((local_read_index + 1) %
                                                   MAXNO_CNM_SERVICE_REQ_QUEUE_ENTRIES);
   }
   return FALSE;
}

/**************** End of   CNM_check_cnm_service_request_queue ********************/




/**************** Begin of   CNM_send_est_request_from_service_queue ********************
 *
 *  Function name: CNM_send_est_request_from_service_queue
 *  -----------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function pops service request stored in queue and checks if it is still valid and 
 *  puts establishment request to MM.
 *
 *  Parameters:
 *  -----------
 *  
 *
 *  Returns:
 *  --------
 *  
 *
 ******************************************************************************/

VOID_FUNC CNM_send_est_request_from_service_queue(void)
{
   boolean msg_present;
   cnm_request_id_T cnm_request_id;
   mmcnm_est_req_T   mmcnm_est_req;
   boolean is_valid_serv_req = FALSE;
   
   while(!is_valid_serv_req)
   {
     is_valid_serv_req = TRUE;
     msg_present = CNM_pop_cnm_service_request_queue(&cnm_request_id);
 
     if ( msg_present != FALSE )
     {
   
       if((cnm_request_id.CNM_service_type != SUPPLEMENTARY_SERVICE_ACTIVATION) &&
           (cnm_request_id.CNM_service_type != SHORT_MESSAGE_SERVICE_ESTABLISHMENT))
       {
         if(mn_call_information_p[cnm_request_id.connection_id] == NULL)
         {
           MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Rejecting an entry in service request queue as mn_call_information_p is NULL");
           is_valid_serv_req = FALSE;
         }
         else if(mn_call_information_p[cnm_request_id.connection_id]->CNM_service_type != cnm_request_id.CNM_service_type)
         {
           MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Rejecting an entry in service request queue as Service type mismatch is there");
           is_valid_serv_req = FALSE;
         }
       }
       else if(cnm_request_id.CNM_service_type == SUPPLEMENTARY_SERVICE_ACTIVATION)
       {
         if(ss_transaction_information[cnm_request_id.connection_id].ss_state == SS_IDLE)
         {
           MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Rejecting an SS entry in CNM service request queue as the Service was aborted by an upper layer");
           is_valid_serv_req = FALSE;
         }
       }

       if (is_valid_serv_req == TRUE)
       {

       /* Check for the Subscription ID match in Dual Sim baselines .
               If it matches process the serv_req else 
               reject it to the corresponding CC/SS/SMS -MN module */
               
#if defined(FEATURE_DUAL_SIM )
        if(MNCNM_IS_SINGLE_STACK)
        {
           if(cnm_request_id.CNM_service_type == SUPPLEMENTARY_SERVICE_ACTIVATION)
           {
             /* when MM connection is accepted mn_as_id is NOT NONE, So no need to check */
             if( ss_transaction_information[cnm_request_id.connection_id].as_id != mn_as_id )
             {
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= REJ'ing Ser Req Q element; mn_as_id Mismatch Active = %d\n",mn_as_id);
               CNM_send_MNSS_REJ_IND(cnm_request_id.connection_id, AS_REJ_ABORT_RADIO_UNAVAILABLE);
               /*
                        *   call SS state machine to transition state to SS_IDLE
                       */
               if(CNM_ss_respond_to_event(cnm_request_id.connection_id, MMSS_REJ_IND_event) == FALSE) 
               {
                 MSG_HIGH_DS_2(MN_SUB,"=MNCNM= SS connection ID = %d in wrong state",cnm_request_id.connection_id, MMSS_REJ_IND_event);
               }
               /* Mark the current service request node as not valid */
               is_valid_serv_req = FALSE;
             }
           }
           else if(cnm_request_id.CNM_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT)
           {
   
             /* when MM connection is accepted mn_as_id is NOT NONE, So no need to check */
             if( sm_rl_transaction_information[cnm_request_id.connection_id].as_id != mn_as_id )
             {
   
               CNM_sms_respond_to_event(MMSMS_EST_REJ_event,
                 sms_transaction_information[cnm_request_id.connection_id].sms_state,
                 cnm_request_id.connection_id);
   
               MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= REJ'ing Ser Req Q element; mn_as_id Mismatch Active = %d\n",mn_as_id);
   #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
               CNM_send_MNSMS_ERROR_IND( cnm_request_id.connection_id, 
                                                                   (byte)0, 
                                                                   AS_REJ_ABORT_RADIO_UNAVAILABLE, 
                                                                   (byte)0, 
                                                                   MMCC_SMS_CS_PS);
   #else
               CNM_send_MNSMS_ERROR_IND( cnm_request_id.connection_id, 
                                                                   (byte)0, 
                                                                   AS_REJ_ABORT_RADIO_UNAVAILABLE);
   #endif
               /* Mark the current service request node as not valid */
               is_valid_serv_req = FALSE;
             }
   
           }
           else if(cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT ||
                   cnm_request_id.CNM_service_type == EMERGENCY_CALL_ESTABLISHMENT ||
                   cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT_FULL_SPEECH ||
                   cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT_HALF_SPEECH ||
                   cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_9600 ||
                   cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_2400 ||
                   cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_4800 )
           {
             /* when MM connection is accepted mn_as_id is NOT NONE, So no need to check */
             if( mn_call_information_p[cnm_request_id.connection_id]->as_id != mn_as_id )
             {
   
               MNCC_REJ_IND_T  *p_mncc_rej_ind;
               byte                       action;
   
               action = CNM_cc_check_respond_to_event(cnm_request_id.connection_id, MMCC_EST_REJ_event);
               if (action != 0 )
               {
   
                 if((cc_transaction_information_p[cnm_request_id.connection_id] != NULL) &&
                    (!((cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT ||
                         cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT_FULL_SPEECH ||
                         cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT_HALF_SPEECH ||
                         cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_9600 ||
                         cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_2400 ||
                         cnm_request_id.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_4800 ) &&
                        cc_transaction_information_p[cnm_request_id.connection_id]->cc_state == CC_STATE_U0 )))
                 {
                   /* Above condition prevents triggering of a second MT_CALl_END_IND to CM if the 
                            Service Type is MO CAll & it has been already Cleared by User. This is required because 
                            CNM initiates Service Request to MM even if the User has Ended the MO CAll. */
   
                   p_mncc_rej_ind = (MNCC_REJ_IND_T*) CNM_alloc( sizeof( MNCC_REJ_IND_T ));
   
                   if (CNM_setup_MNCC_REJ_IND(p_mncc_rej_ind, 
                                 cnm_request_id.connection_id, 
                                             AS_REJECT_CAUSE, 
                              AS_REJ_ABORT_RADIO_UNAVAILABLE, 
                                                  (byte *)NULL))
                   {
                     CNM_send_message((IMH_T *)p_mncc_rej_ind, GS_QUEUE_MN_CM );
                     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =MN= REJ'ing Ser Req Q element; mn_as_id Mismatch Active = %d\n",mn_as_id);
                   }
                   else
                   {
                     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Invalid reject cause %d", cnm_request_id.connection_id);
                   }
   
                   CNM_free( p_mncc_rej_ind );
                 }
                  
                 CNM_cc_respond_to_event(cnm_request_id.connection_id, action );
                 
               }
   
               /* Mark the current service request node as not valid */
               is_valid_serv_req = FALSE;
             }
              
           }
        }
#endif
       }/* if (is_valid_serv_req == TRUE)*/

     }
   }/*while */

   if(is_valid_serv_req && msg_present)
   {
     mm_connection_pending = cnm_request_id;
     
     mmcnm_est_req.header.message_set = MS_CC_MM;
     mmcnm_est_req.header.message_id  = MMCNM_EST_REQ;

     PUT_IMH_LEN(sizeof(mmcnm_est_req_T) - sizeof(IMH_T),
                &mmcnm_est_req.header );

     mmcnm_est_req.CNM_service_type = cnm_request_id.CNM_service_type;
     mmcnm_est_req.mm_rrc_call_type = CNM_return_calltype();

     CNM_send_message ((IMH_T *) &mmcnm_est_req,
                    GS_QUEUE_MM);
   }
   else
   {
     mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;

#if defined(FEATURE_DUAL_SIM )
     if(MNCNM_IS_SINGLE_STACK)
     {
        if( (CNM_check_if_any_service_object_exists() == FALSE) &&
            (mn_as_id != SYS_MODEM_AS_ID_NONE) )
        {
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= =MN= Reset the mn_as_id here\n");
          mn_as_id = SYS_MODEM_AS_ID_NONE;
        }
     }
#endif

   }


}

/**************** End of   CNM_send_est_request_from_service_queue *************************/


/**************** Begin of  CNM_manage_cnm_service_requests ************************
 *
 *  Function name: CNM_manage_cnm_service_requests()
 *  ----------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function deals with service requests for either CC, SS, or SMS
 *  If the mm_link is reestablishing, or there is an outstanding request
 *  in progress, then this request is sent to the service request queue.
 *  Otherwise, the details of the request are stored in mm_connection_pending
 *  ready for later, and an establishment request is sent to MM.
 *
 *  Parameters:
 *  -----------
 *  service_type : IN
 *  connection_id : IN
 *  p_message : IN - pointer to message to send
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_manage_cnm_service_requests( CNMServiceType_T service_type,
                                          connection_id_T connection_id,
                                          IMH_T * p_message )
{
   cnm_request_id_T      cnm_request_id;
   mmcnm_est_req_T       mmcnm_est_req;
   mmcnm_data_req_T      *p_mmcnm_data_req = (mmcnm_data_req_T *)((void *)p_message);

   cnm_request_id.CNM_service_type = service_type;
   cnm_request_id.connection_id = connection_id;
   
   /* copy application data request into buffer */

   (void)memscpy((VOID_DATA *) &cnm_request_id.mmcnm_data_req,
                sizeof(mmcnm_data_req_T ),
                (const VOID_DATA *) p_mmcnm_data_req,
                sizeof(mmcnm_data_req_T ));
   /*
    * If there is a pending mm connection establishment request,  add one
    * more request to the queue,  otherwise send a MMCNM_EST_REQ and start 
    * timer T303 only for CC requests
    */
   if ( ( mm_connection_pending.CNM_service_type != NO_CNM_SERVICE ) ||
        ( mm_link_reestablishing == TRUE ) ||
        ( mm_rel_req_sent == TRUE ) )
   {

      if( ( mm_connection_pending.CNM_service_type == NO_CNM_SERVICE ) &&
          ( mm_link_reestablishing != TRUE) &&
          ( mm_rel_req_sent == TRUE ) )  /* 1st service request after mm_rel_req_sent */
      {
          /* Both mm_connection_pending and service request queue is empty */
          (void)memscpy((VOID_DATA *)&mm_connection_pending, 
                   sizeof(mm_connection_pending),
                   (const VOID_DATA *) &cnm_request_id,
                   sizeof(mm_connection_pending) );

          CNM_add_to_cnm_service_request_queue(&cnm_request_id);

          MSG_MED_DS_1(MN_SUB,"=MNCNM= MMCNM_EST_REQ is put on pending queued", connection_id);
      }
      else if( ((mm_connection_pending.CNM_service_type != NO_CNM_SERVICE ) && 
                (mm_rel_req_sent != TRUE)) ||
               (mm_link_reestablishing == TRUE) )
      {
          /* subsequence service request */
          CNM_add_to_cnm_service_request_queue(&cnm_request_id);

          MSG_MED_DS_1(MN_SUB,"=MNCNM= MMCNM_EST_REQ is on service request queued", connection_id);
      }
      else
      {
          switch (cnm_request_id.CNM_service_type)
          {
               case MO_CALL_ESTABLISHMENT:
               case EMERGENCY_CALL_ESTABLISHMENT:
               case MO_CALL_ESTABLISHMENT_FULL_SPEECH:
               case MO_CALL_ESTABLISHMENT_HALF_SPEECH:
               case MO_CALL_ESTABLISHMENT_DATA_9600:
               case MO_CALL_ESTABLISHMENT_DATA_4800:
               case MO_CALL_ESTABLISHMENT_DATA_2400:
                   {
                       MNCC_REJ_IND_T      *p_mncc_rej_ind;
                       byte action;

                       if((action = CNM_cc_check_respond_to_event(connection_id, MMCC_EST_REJ_event)) != 0)
                       {

                           CNM_cc_respond_to_event(connection_id, action );
                       }

                       p_mncc_rej_ind = (MNCC_REJ_IND_T*) CNM_alloc( sizeof( MNCC_REJ_IND_T ));

                       if (CNM_setup_MNCC_REJ_IND(p_mncc_rej_ind, connection_id, CNM_MN_REJECT_CAUSE, CNM_REJ_NO_RESOURCES, (byte *) 0))
                       {
                           CNM_send_message((IMH_T *)p_mncc_rej_ind, GS_QUEUE_MN_CM );
                       }

                       CNM_free(p_mncc_rej_ind);
                   }
                   break;

               case SUPPLEMENTARY_SERVICE_ACTIVATION:
                   /*
                   * call SS state machine to transition state to SS_IDLE
                   */
                   if(CNM_ss_respond_to_event(connection_id, MMSS_REJ_IND_event) == FALSE) 
                   {
                      MSG_HIGH_DS_2(MN_SUB,"=MNCNM= SS connection ID = %d in wrong state", connection_id, MMSS_REJ_IND_event);  
                   }

                   CNM_send_MNSS_REJ_IND(connection_id, NORMAL_UNSPECIFIED);

                   break;

               case SHORT_MESSAGE_SERVICE_ESTABLISHMENT:

                   CNM_sms_respond_to_event(MMSMS_EST_REJ_event, 
                               sms_transaction_information[connection_id].sms_state,
                               connection_id);
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                   CNM_send_MNSMS_ERROR_IND( connection_id, (byte)0, NORMAL_UNSPECIFIED, 0, MMCC_SMS_CS_PS);
#else
                   CNM_send_MNSMS_ERROR_IND( connection_id, (byte)0, NORMAL_UNSPECIFIED);
#endif

                   break;

               default:
                   MSG_HIGH_DS_1(MN_SUB,"=MNCNM= MMCNM_EST_REQ not possible, request lost", connection_id);
                   break;
          }
#if defined(FEATURE_DUAL_SIM ) 
          if(MNCNM_IS_SINGLE_STACK)
          {
             if( (CNM_check_if_any_service_object_exists() == FALSE) &&
                 (mn_as_id != SYS_MODEM_AS_ID_NONE) )
             {
               MSG_HIGH_DS_0(MN_SUB,"=MNCNM= =MN= Reset the mn_as_id here\n");
               mn_as_id = SYS_MODEM_AS_ID_NONE;
             }
          }
#endif
      }

   }
   else
   {
      (void)memscpy((VOID_DATA *)&mm_connection_pending,
                   sizeof(mm_connection_pending),
                   (const VOID_DATA *) &cnm_request_id,
                   sizeof(mm_connection_pending) );

      mmcnm_est_req.header.message_set = MS_CC_MM;
      mmcnm_est_req.header.message_id  = MMCNM_EST_REQ;
      mmcnm_est_req.CNM_service_type    = service_type;
      mmcnm_est_req.mm_rrc_call_type = CNM_return_calltype();

      PUT_IMH_LEN(sizeof(mmcnm_est_req_T) - sizeof(IMH_T),
                  &mmcnm_est_req.header );

      CNM_send_message( (IMH_T*)&mmcnm_est_req, GS_QUEUE_MM );

      switch (mm_connection_pending.CNM_service_type)
      {
          case MO_CALL_ESTABLISHMENT:
          case EMERGENCY_CALL_ESTABLISHMENT:
          case MO_CALL_ESTABLISHMENT_FULL_SPEECH:
          case MO_CALL_ESTABLISHMENT_HALF_SPEECH:
          case MO_CALL_ESTABLISHMENT_DATA_9600:
          case MO_CALL_ESTABLISHMENT_DATA_4800:
          case MO_CALL_ESTABLISHMENT_DATA_2400:
             CNM_cc_start_timer(TIMER_T303,connection_id);
             break;

          default:
              break;
      }
   }
}

/**************** End of   CNM_manage_cnm_service_requests *************************/
