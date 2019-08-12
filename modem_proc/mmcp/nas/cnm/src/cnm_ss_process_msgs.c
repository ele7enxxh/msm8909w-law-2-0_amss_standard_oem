/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2011, 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_ss_process_msgs.c_v   1.11   28 Jun 2002 13:04:16   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_ss_process_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

09/17/01   CD      In CNM_handle_supplementary_services, added logic to decide if the 
                   message needs to be checked for errors or not

                   Added processing of case MMCNM_EST_CNF, MMCC_REEST_CNF, 
                   MMCNM_ABORT_IND, MMCNM_REL_IND in 
                   CNM_SS_process_supplementary_services

10/12/01   AB      Fixed Lint errors.

10/22/01   AB      Replaced all SS state references with SS state changes access
                   function, CNM_get_ss_state().

04/19/02   CD      In CNM_SS_process_supplementary_services(), changed how the
                   MMCNM_REL_IND rel_cause is processed based in new MM-CNM interface

                   Replaced MMCC_REEST_CNF by MMCC_REEST_REJ, since 
                   CNM_handle_supplementary_services() used to be called only when a 
                   MMCC_REEST_CNF was received with a reestablish_success flag set
                   to FALSE

                   Deleted processing of MMCNM_ABORT_IND.  It's now part of a 
                   MMCNM_REL_IND w/rel_cause set to ABORT_MSG_RECEIVED

                   Deleted processing of MMCNM_ERR_IND.  It's now part of a 
                   MMCNM_REL_IND w/rel_cause set to OTHER_CAUSE

                   For case MMCNM_EST_CNF replaced call to CNM_get_connection_id
                   by directly getting the conection id from the mm_connection_pending
                   global variable

                   Added processing for MMCNM_EST_REJ

                   Simplified processing in CNM_handle_supplementary_services()

                   Modified processing of CNM_SS_process_message_type() and 
                   CNM_SS_process_supplementary_services() to account for the
                   merge of MMCNM_EST_IND into MMCNM_DATA_IND

                   Updatred Copyright
                   Fixed lint errors

05/15/02   CD      Correcting processing of MMCNM_EST_REJ to clear the 
                   mm_connection_pending.CNM_service_type

06/30/02   AB      In CNM_handle_supplementary_services(), added a check
                   for last connection to release the transaction (SDCCH).

07/29/02   CD      In CNM_handle_supplementary_services(), added check to process
                   a MMCC_REEST_REJ if a transaction is not in the SS_IDLE state. 
                   Also added check for the message id not being MMCNM_REL_IND
                   or MMCC_REEST_REJ before calling CNM_check_for_last_connection()

06/06/03   AB      In CNM_handle_supplementary_services(), moved/optimized the 
                   check for last connection release inside the
                   CNM_process_supplementaryservices().

                   Removed Lint error.

07/04/11   AM      Putting range check for fixing buffer overflow of cc_IE_info.

09/07/11   AM      Adding comments to the code.

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
#include "cause.h"
#include "cmlib.h"
#include "cnm.h"
#include <stringl/stringl.h>
#include "nasutils.h"


/*
 * global data declaration
 */

extern   struct MessageIETag  *next_ref;


/**************** Begin of   CNM_handle_supplementary_services ********************
 *
 *  Function name: CNM_handle_supplementary_services ()
 *  --------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function deals with all call independent supplementary services
 *  messages received either from MM or MN. If the message was received over
 *  the air, then addition checking is performed.
 *
 *  Parameters:
 *  -----------
 *  SS_input_primitive : IN - SS message for processing
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_handle_supplementary_services ( IMH_T * SS_input_primitive )
{

   boolean                       message_ok;
   boolean                       check_msg;
   connection_id_T               connection_id;
   mmcnm_data_ind_T              *p_mmcnm_data_ind;

   message_ok = TRUE;
   check_msg = TRUE;


   p_mmcnm_data_ind = (mmcnm_data_ind_T *) ((void *)SS_input_primitive);  /* explicit type conversion  */

   if ( SS_input_primitive->message_id == MMCNM_DATA_IND )
   {
       /* check the message type.  See 24.007 11.2.4 */
       /* if OK,  continue checking the message */
       if ( (p_mmcnm_data_ind->msg_head.type & 0x40) != 0 )
       {
           MSG_ERROR_DS_0(MN_SUB,"=MNCNM= N(SD) is 1: message rejected\n");
           message_ok = FALSE;
           check_msg = FALSE;
       }

       if ((p_mmcnm_data_ind->msg_head.type & 0x80) != 0 )
       {
           connection_id = CNM_get_connection_id(p_mmcnm_data_ind);

           CNM_send_release_complete(connection_id, 
                                         PD_SS,
                                         MESSAGE_TYPE_NON_EXISTENT,
                                         (ie_facility_T *) 0
                                     );
           message_ok = FALSE;
           check_msg = FALSE;
       }

       if (check_msg == TRUE)
       {
          /*
           * additional processing for OTA messages
           */
          message_ok = CNM_SS_process_message_type( p_mmcnm_data_ind );

          if (message_ok != FALSE )
          {
              message_ok = CNM_check_for_protocol_errors( p_mmcnm_data_ind );

              if (message_ok != FALSE)
              {
                  /* Process external SS primitives */
                  CNM_SS_process_supplementary_services( SS_input_primitive );
              }
              else 
              {
                  connection_id = CNM_get_connection_id( p_mmcnm_data_ind);

                  MSG_ERROR_DS_1(MN_SUB,"=MNCNM= SS Failed Protocol Checking", connection_id);

                  CNM_send_release_complete( connection_id,
                                             PD_SS,
                                             INVALID_MANDATORY_INFORMATION,
                                              NULL
                                            );
              }

          } /* message_ok */
          else
          {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Invalid SS message type");
          }
       } /* check_msg */

   } /* MMCNM_DATA_IND */
   else
   {
       /* Process other/internal SS primitives */
       CNM_SS_process_supplementary_services( SS_input_primitive );
   }

}


/**************** End of   CNM_handle_supplementary_services *************************/


/**************** Begin of   CNM_SS_process_message_type **************************
 *
 *  Function name: CNM_SS_process_message_type ()
 *  --------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function checks for a valid message type in a  SS  MMCNM_DATA_IND 
 *  message received from the network
 *
 *  Parameters:
 *  -----------
 *  SS_input_primitive : IN - input message
 *
 *  Returns:
 *  --------
 *  message_ok
 *
 ******************************************************************************/

boolean CNM_SS_process_message_type ( mmcnm_data_ind_T * SS_input_primitive )
{
   boolean                 return_value;  
   connection_id_T         connection_id;
   int                     length;
   struct MsgDetailTag    *p_message_details;
   mmcnm_data_ind_T       *p_mmcnm_data_ind;

   GET_IMH_LEN( length, &SS_input_primitive->msg_head.IMH )
   
#ifdef FEATURE_DUAL_SIM
   length = length - sizeof(byte);
#endif

   return_value = FALSE;

   if ( length >= 2 )      /* there is a message type field in the message */
   {

      p_mmcnm_data_ind = (mmcnm_data_ind_T *) ((void *)SS_input_primitive);

      /* set up pointers for determining the allowed ies in the message */

      p_message_details = 
         CNM_setup_IE_data_read( p_mmcnm_data_ind->msg_head.type, MT_CALL);

      /*
       * if return value is null, something wrong with message
       */

      if (p_message_details  != (struct MsgDetailTag *) 0 )
      {
         
         /*
          * set up IE database access pointer for later use
          */

         next_ref = GET_FIRST_REF( p_message_details);

         connection_id = CNM_get_connection_id( p_mmcnm_data_ind ) ;

         if ( (connection_id & 0x07) != 7 )  /* disallowed id */
         {
            switch ( p_mmcnm_data_ind->msg_head.type )
            {
               case REGISTER:
               case FACILITY:
               case RELEASE_COMPLETE:
                  return_value = TRUE;
                  break;

               default:
                  CNM_send_release_complete( connection_id,
                                             PD_SS,
                                             MESSAGE_TYPE_NON_EXISTENT,
                                             (ie_facility_T *) 0
                                            );

                  MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected SS message type: received msg type = %d\n",
                            p_mmcnm_data_ind->msg_head.type);
                  break;
            }
         } 
         else
         {
            MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Incorrect connection_id of 7: received connection_id = %d\n",
                      connection_id);
         }
      }
      else
      {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Incorrect SS message: ignored\n");
      }
   }
   else
   {
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Incomplete ss message with length < 2: received length = %d\n",
                    length);
   }

   return return_value;
}
/**************** End of   CNM_SS_process_message_type *************************/


/**************** Begin of   CNM_SS_process_supplementary_services ****************
 *
 *  Function name: CNM_SS_process_supplementary_services ()
 *  ------------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function invokes the appropriate handler routine depending on the
 *  message
 *
 *  Parameters:
 *  -----------
 *  SS_input_primitive : IN - input message
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_SS_process_supplementary_services ( IMH_T * SS_input_primitive )
{
    connection_id_T  connection_id;
    mmcnm_rel_ind_T  *p_mmcnm_rel_ind;
    MNSS_REJ_IND_T   *mnss_rej_ind;
    byte             cause;
    boolean          est_ind;
    boolean          release_connection = FALSE;  /* release connection */
 
    switch (SS_input_primitive->message_set)
    {
      case MS_MN_SS:

         switch (SS_input_primitive->message_id)
         {
            case MNSS_BEGIN_REQ:
               CNM_start_mo_ss_request( SS_input_primitive );
               break;

            case MNSS_FACILITY_REQ:
               CNM_process_mo_facility(SS_input_primitive);
               break;

            case MNSS_END_REQ:
               CNM_mo_end_ss_request( SS_input_primitive);

               /* Check for possible last connection release */
               if(mm_connection_pending.CNM_service_type == SUPPLEMENTARY_SERVICE_ACTIVATION) 
               {
                   (void)CNM_check_for_last_connection(CNM_ESTABLISHMENT_IN_PROGRESS
#if defined(FEATURE_DUAL_SIM )
                                                       , mn_as_id
#endif
                                                      );   /* possible user abort */
               }
               else
               {
                   (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE
#if defined(FEATURE_DUAL_SIM )
                                                       , mn_as_id
#endif
                                                      );
               }

               break;

            default:
               ERR("Unknown SS message id: received message id = %d\n",
                     (dword) (SS_input_primitive->message_id), 0, 0);

               release_connection = TRUE;

               break;
         }
         break;

      case MS_CC_MM:
      case MS_CC_MM_PS:

         switch (SS_input_primitive->message_id)
         {
            case MMCNM_DATA_IND:

               est_ind = CNM_check_for_establish_ind((mmcnm_data_ind_T *)((void *)SS_input_primitive));

               if (est_ind == TRUE)
               {
                    CNM_start_mt_ss_request(SS_input_primitive);
               }
               else
               {
                    CNM_process_mt_facility(SS_input_primitive);
               }

               /* Check for possible MM connection release */
               if(((msg_head_T *)((void *)SS_input_primitive))->type == RELEASE_COMPLETE)
               {
                    release_connection = TRUE;
               }
               else
               {
                    release_connection = FALSE;
               }

               break;

            case MMCNM_EST_CNF:

               connection_id = mm_connection_pending.connection_id;

               if (ss_transaction_information[connection_id].ss_state != SS_IDLE) 
               {
                 if(CNM_ss_respond_to_event(connection_id, MMSS_EST_CNF_event) == FALSE) 
                 {
                    MSG_HIGH_DS_2(MN_SUB,"=MNCNM= SS connection ID = %d in wrong state = %d", connection_id, MMSS_EST_CNF_event);  
                 }               
                 CNM_process_mm_conn_accepted();
               }
               else  /* Race condition:  A MMCNM_EST_CNF is received in idle state during a user abort */
               {
                 /* make sure pending EST is being service */
                 CNM_send_est_request_from_service_queue();
               }

               break;

            case MMCNM_EST_REJ:
               {
                 mmcnm_est_rej_T    *mmcnm_est_rej;

                 mmcnm_est_rej = (mmcnm_est_rej_T *)SS_input_primitive;

                 connection_id = mm_connection_pending.connection_id;

                 mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;

                 CNM_send_MNSS_REJ_IND(connection_id, mmcnm_est_rej->cause_value);

                 /*
                 *   call SS state machine to transition state to SS_IDLE
                 */
                 if(CNM_ss_respond_to_event(connection_id, MMSS_REJ_IND_event) == FALSE) 
                 {
                   MSG_HIGH_DS_2(MN_SUB,"=MNCNM= SS connection ID = %d in wrong state", connection_id, MMSS_REJ_IND_event);  
                 }
               }
               break;

            case MMCC_REEST_REJ:

               for ( connection_id = 0; connection_id < MAXNO_SS_TRANSACTIONS; connection_id++)
               {
                   if (ss_transaction_information[connection_id].ss_state != SS_IDLE)
                   {
                      if(CNM_ss_respond_to_event(connection_id, MMSS_REEST_REJ_event) == FALSE)
                      {
                         MSG_HIGH_DS_1(MN_SUB,"=MNCNM= SS- %d <-- MMCC_REEST_REJ in wrong state", connection_id);  
                      }
                   }
               }
               break;

            case MMCNM_REL_IND:
                {
                    p_mmcnm_rel_ind = (mmcnm_rel_ind_T *)((void *)SS_input_primitive);

                    cause = p_mmcnm_rel_ind->rel_cause;

                    /* Invoke ID */
                    for (connection_id = 0; connection_id < MAXNO_SS_TRANSACTIONS; connection_id++)
                    {
                        if (CNM_get_ss_state(connection_id) != SS_IDLE)
                        {
                            mnss_rej_ind = (MNSS_REJ_IND_T*) CNM_alloc( sizeof(MNSS_REJ_IND_T) );
                            mnss_rej_ind->message_header.message_set = MS_MN_SS;
                            mnss_rej_ind->message_header.message_id = MNSS_REJ_IND;
                            mnss_rej_ind->connection_id = connection_id;  /* invoke ID */

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
                            {
                              mnss_rej_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                            }
#endif
                            mnss_rej_ind->lower_layer_cause = 0;
                            mnss_rej_ind->mm_cause = 0;

                            if(cause == ABORT_MSG_RECEIVED)
                            {
                                mnss_rej_ind->mm_cause = cause;
                            }
                            else if(cause == LLC_RADIO_LINK_FAIL)
                            {
                                mnss_rej_ind->lower_layer_cause = cause;
                            }
                            else
                            {
                                mnss_rej_ind->mm_cause = cause;

                                MSG_HIGH_DS_2(MN_SUB,"=MNCNM= SS received MMCNM_REL_IND: invoke=%d, cause=%d", connection_id, cause);
                            }
#if defined(FEATURE_DUAL_SIM )
                            if(MNCNM_IS_SINGLE_STACK)
                            {
                              mnss_rej_ind->as_id = ss_transaction_information[connection_id].as_id;
                            }
#endif

                            PUT_IMH_LEN( sizeof( MNSS_REJ_IND_T ) - sizeof(IMH_T), &mnss_rej_ind->message_header);

                            CNM_send_message((IMH_T *) ((void*) mnss_rej_ind), GS_QUEUE_MN_CM);

                            if(CNM_ss_respond_to_event(connection_id, MMSS_REJ_IND_event) == FALSE)
                            {
                                MSG_HIGH_DS_1(MN_SUB,"=MNCNM= SS invoke=%d in wrong state for a MMSS_REJ_IND", connection_id);  
                            } 

                            CNM_free (mnss_rej_ind);
                        }

                    } /* for (connection) */

                    break;
                }

            default:
               ERR("Unknown SS message id: received message id = %d\n",
                     (dword) (SS_input_primitive->message_id), 0, 0);

               release_connection = TRUE;
               break;

         }  /* MS_CC_MM */

         break;

      default:
         ERR("Unknown SS message set: received message_set = %d\n",
                (dword) (SS_input_primitive->message_set), 0, 0);

         release_connection = TRUE;

         break;
         
    }  /* SS Input Primitives */

    /* Make sure that the MM connection is released after a SS call Independent 
    * TRANSACTION is completed.  First check that we are not processing a 
    * MMCNM_REL_IND
    */
    if(release_connection)
    {
         if ((SS_input_primitive->message_id != MMCNM_REL_IND)&&
             (SS_input_primitive->message_id != MMCC_REEST_REJ))
         {
               (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE
#if defined(FEATURE_DUAL_SIM )
                                                         , mn_as_id
#endif
                                                  );
         }
    }

}

/**************** End of   CNM_SS_process_supplementary_services *************************/


/**************** Begin of   CNM_process_facility_message *************************
 *
 *  Function name: CNM_process_facility_message()
 *  ----------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a facility message received from the network.
 *  It walks through the IEs in the message checking for validity, and
 *  if all ok, sends the request on to MN, otherwise a status message is sent.
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

VOID_FUNC   CNM_process_facility_message( connection_id_T connection_id, boolean syntax_ok, byte cause)
{

   byte                       *p_IE;
   byte                       IEI;

   byte                       IE_count;
   byte                       IE_length;

   MNCC_FACILITY_IND_T        mncc_facility_ind;


   p_IE = (byte *) 0;
   IE_count = 2;              /* start at first IE after message type */

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
           case IEI_FACILITY:
              {
                mncc_facility_ind.message_header.message_set = MS_MN_CC;
                mncc_facility_ind.message_header.message_id  =
                                     MNCC_FACILITY_IND;
                mncc_facility_ind.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
                {
                  mncc_facility_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                }
#endif


                IE_length = memscpy( &mncc_facility_ind.data[0], sizeof(mncc_facility_ind.data) ,p_IE, IE_length);

                /*
                 * overiding missing facility IEI to output message
                 */

                mncc_facility_ind.data[0] = IEI_FACILITY;

                PUT_IMH_LEN( IE_length +
                               sizeof(connection_id) +
                               sizeof(sys_modem_as_id_e_type),
                               &mncc_facility_ind.message_header );
              }
              break;

           default:                            /* this cannot happen! */
              ERR("Unexpected IE in facility msg: received IEI = %d\n",
                 (dword) IEI, 0, 0);

              syntax_ok = FALSE;
              break;
        }     /* end case */
      }

      IE_count++;       /* index next IE in the message */

   }   /* END WHILE more IEs and Syntax is OK */


   if ( syntax_ok )
   {

      CNM_send_message( (IMH_T *)&mncc_facility_ind,
                                    GS_QUEUE_MN_CM );
   }
   else
   {
     CNM_send_status ( connection_id,
                    PD_CC,
                    cause );
   }
}
/**************** End of   CNM_process_facility_message *************************/


/**************** Begin of   CNM_process_hold_ack_message *************************
 *
 *  Function name: CNM_process_hold_ack_message()
 *  --------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a hold ack message received from the network.
 *  It assumes that the message has been checked for IE content. There are
 *  no IEs in the message (apart from the PD,TI, and type) so no processing
 *  is required.
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

VOID_FUNC   CNM_process_hold_ack_message( connection_id_T connection_id ) 
{
   MNCC_CALL_HOLD_CNF_T mncc_call_hold_cnf;

   mncc_call_hold_cnf.message_header.message_set = MS_MN_CC;
   mncc_call_hold_cnf.message_header.message_id  = MNCC_CALL_HOLD_CNF;
   mncc_call_hold_cnf.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_call_hold_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   PUT_IMH_LEN( sizeof(mncc_call_hold_cnf) - sizeof(IMH_T), 
                   &mncc_call_hold_cnf.message_header);

   CNM_send_message( (IMH_T *)&mncc_call_hold_cnf,
                    GS_QUEUE_MN_CM );
}

/**************** End of   CNM_process_hold_ack_message ***************************/


/**************** Begin of   CNM_process_hold_rej_message *************************
 *
 *  Function name: CNM_process_hold_rej_message()
 *  ------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a hold reject message received from the network.
 *  It assumes that the message has been checked for IE content. The cause
 *  information in the message is presently ignored. If this is changed, 
 *  then it should be noted that the cause IE in the message does not have
 *  an IEI
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

VOID_FUNC   CNM_process_hold_rej_message( connection_id_T connection_id ) 
{
   MNCC_CALL_HOLD_REJ_T    mncc_call_hold_rej;

   mncc_call_hold_rej.message_header.message_set = MS_MN_CC;
   mncc_call_hold_rej.message_header.message_id  = MNCC_CALL_HOLD_REJ;
   mncc_call_hold_rej.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_call_hold_rej.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   PUT_IMH_LEN( sizeof(mncc_call_hold_rej) - sizeof(IMH_T), 
                   &mncc_call_hold_rej.message_header);

   CNM_send_message((IMH_T *)&mncc_call_hold_rej,
                    GS_QUEUE_MN_CM );
}

/**************** End of   CNM_process_hold_rej_message ***************************/


/**************** Begin of   CNM_process_retrieve_ack_message *********************
 *
 *  Function name: CNM_process_retrieve_ack_message()
 *  ------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a retrieve ack message received from the network.
 *  It assumes that the message has been checked for IE content. There are
 *  no IEs in the message (apart from the PD,TI, and type) so no processing
 *  is required.
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

VOID_FUNC   CNM_process_retrieve_ack_message( connection_id_T connection_id ) 
{
   MNCC_CALL_RETRIEVE_CNF_T   mncc_call_retrieve_cnf;

   mncc_call_retrieve_cnf.message_header.message_set = MS_MN_CC;
   mncc_call_retrieve_cnf.message_header.message_id  = MNCC_CALL_RETRIEVE_CNF;
   mncc_call_retrieve_cnf.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_call_retrieve_cnf.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   PUT_IMH_LEN( sizeof(mncc_call_retrieve_cnf) - sizeof(IMH_T), 
                   &mncc_call_retrieve_cnf.message_header);

   CNM_send_message((IMH_T *)&mncc_call_retrieve_cnf,
                    GS_QUEUE_MN_CM );
}

/**************** End of   CNM_process_retrieve_ack_message ***********************/


/**************** Begin of   CNM_process_retrieve_rej_message *********************
 *
 *  Function name: CNM_process_retrieve_rej_message()
 *  ------------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a retrieve reject message received from the network.
 *  It assumes that the message has been checked for IE content. currently
 *  the cause IE is not used. Should it be used in the future, then it should
 *  be noted that the cause IE has no IEI
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

VOID_FUNC   CNM_process_retrieve_rej_message( connection_id_T connection_id ) 
{
   MNCC_CALL_RETRIEVE_REJ_T   mncc_call_retrieve_rej;

   mncc_call_retrieve_rej.message_header.message_set = MS_MN_CC;
   mncc_call_retrieve_rej.message_header.message_id  = MNCC_CALL_RETRIEVE_REJ;
   mncc_call_retrieve_rej.connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     mncc_call_retrieve_rej.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   PUT_IMH_LEN( sizeof(mncc_call_retrieve_rej) - sizeof(IMH_T), 
                   &mncc_call_retrieve_rej.message_header);

   CNM_send_message((IMH_T *)&mncc_call_retrieve_rej,
                    GS_QUEUE_MN_CM );
}

/**************** End of   CNM_process_retrieve_rej_message *************************/


/**************** Begin of   CNM_process_mo_call_retrieve_request *****************
 *
 *  Function name: CNM_process_mo_call_retrieve_request()
 *  ----------------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle message indicating that MN is requesting
 *  retrieval of a held call.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a call retrieve and sends it on.
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

VOID_FUNC CNM_process_mo_call_retrieve_request( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_CALL_RETRIEVE_REQ_T   *p_mncc_call_retr;
   retrieve_msg_T       data_req_msg;
   byte                 action;

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_call_retr = (MNCC_CALL_RETRIEVE_REQ_T *) ((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_call_retr->connection_id;

   action = 
        CNM_cc_check_respond_to_event(connection_id, 
                                     MNCC_CALL_RETRIEVE_REQ_event );

   if ( action != 0 )
   {
      CNM_cc_respond_to_event(connection_id, action );

      data_req_msg.msg_head.type  = RETRIEVE;                     


      PUT_IMH_LEN(sizeof(data_req_msg.msg_head.ti_pd) +
                      sizeof(data_req_msg.msg_head.type),
                      &data_req_msg.msg_head.IMH );
         

      CNM_send_primitive_to_mm(connection_id,
                              PD_CC,
                              (IMH_T *)&data_req_msg );

   }

}
/**************** End of   CNM_process_mo_call_retrieve_request *************************/


/**************** Begin of   CNM_process_mo_facility_request **********************
 *
 *  Function name: CNM_process_mo_facility_request()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle MN request for transmission of a facility message.
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a facilities request and sends it on.
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

VOID_FUNC CNM_process_mo_facility_request( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_FACILITY_REQ_T  *p_mncc_fac_req;
   facility_msg_T       data_req_msg;
   byte                 action;
   int32                length, bytes_remaining;

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_fac_req = (MNCC_FACILITY_REQ_T *) ((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_fac_req->connection_id;

   action =
        CNM_cc_check_respond_to_event(connection_id, MNCC_FACILITY_REQ_event );

   if ( action != 0 )
   {
      CNM_cc_respond_to_event(connection_id, action );

      data_req_msg.msg_head.type  = FACILITY;


      GET_IMH_LEN(length, &p_mncc_fac_req->message_header );

      bytes_remaining = length - (int32)(sizeof(connection_id_T)) - sizeof(sys_modem_as_id_e_type);

      /*
       * Facility message contains a facility IE which should not be
       * sent with an IEI. This is the first byte of the remaining byte
       * so we just skip it, (taking care of the length values accordingly)
       */

      if ( bytes_remaining != 0 )
      {
         bytes_remaining--;            /* allow for removal of IEI byte */

         (void) memscpy( &data_req_msg.data[0], 
                         sizeof(data_req_msg.data),
                         &p_mncc_fac_req->data[1], 
                        (unsigned int)(bytes_remaining) );

      }
      else
      {
         bytes_remaining = 0;
      }

      PUT_IMH_LEN((unsigned long int)(bytes_remaining) +
                   sizeof(data_req_msg.msg_head.ti_pd) +
                   sizeof(data_req_msg.msg_head.type),
                   &data_req_msg.msg_head.IMH );


      CNM_send_primitive_to_mm(connection_id,
                              PD_CC,
                              (IMH_T *)&data_req_msg
                                      );


   }

}
/**************** End of   CNM_process_mo_facility_request *************************/


/**************** Begin of   CNM_process_mo_call_hold_request *********************
 *
 *  Function name: CNM_process_mo_call_hold_request()
 *  -------------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to handle a request that a call is held
 *  The function checks that the message can be handled in the current
 *  state, and if so, forms a hold request and sends it on.
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

VOID_FUNC CNM_process_mo_call_hold_request( IMH_T * message )
{
   connection_id_T      connection_id;
   MNCC_CALL_HOLD_REQ_T *p_mncc_call_hold_req;
   hold_msg_T           data_req_msg;
   byte                 action;

   /*
    * set up pointer to actual type of message being processed
    */

   p_mncc_call_hold_req = (MNCC_CALL_HOLD_REQ_T *) ((void *)message);

   /*
    * get connection id for this transaction
    */

   connection_id = p_mncc_call_hold_req->connection_id;

   action = 
        CNM_cc_check_respond_to_event(connection_id, MNCC_CALL_HOLD_REQ_event );

   if ( action != 0 )
   {
      CNM_cc_respond_to_event(connection_id, action );

      data_req_msg.msg_head.type  = HOLD;


      PUT_IMH_LEN(sizeof(data_req_msg.msg_head.ti_pd) +
                   sizeof(data_req_msg.msg_head.type),
                  &data_req_msg.msg_head.IMH );
         

      CNM_send_primitive_to_mm(connection_id,
                              PD_CC,
                              (IMH_T *)&data_req_msg 
                                         );

   }

}
/**************** End of   CNM_process_mo_call_hold_request *************************/


/**************** Begin of   CNM_process_mo_facility *******************************
 *
 *  Function name: CNM_process_mo_facility()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *  This function processes the request from MN to start a MO SS transaction
 *
 *  Parameters:
 *  -----------
 *  SS_input_primitive : IN - input message
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_mo_facility ( IMH_T * SS_input_primitive )
{

   connection_id_T            connection_id;
   facility_msg_T             facility_msg;
   boolean                    respond_to_event;

   connection_id = ((MNSS_FACILITY_REQ_T *)((void *)SS_input_primitive))->connection_id;

   respond_to_event = CNM_ss_respond_to_event( connection_id,
                                              MNSS_FACILITY_REQ_event );

   if (respond_to_event != FALSE)
   {
      facility_msg.msg_head.IMH.message_set = MS_CC_MM;
      facility_msg.msg_head.IMH.message_id = MMCNM_EST_REQ;

      facility_msg.msg_head.ti_pd = (unsigned char) (FORM_TI_PD(connection_id, PD_SS));

      facility_msg.msg_head.type = FACILITY;

      /*
       * copy facility IE, minus the IEI, adding 1 to the length to include
       * the length field
       */

      /* length in this memcpy comes from checked value in MN_put_facility()*/
      (void)memscpy((void *) &facility_msg.data[0],
                    sizeof(facility_msg.data),
                   (const void *) & ((MNSS_FACILITY_REQ_T *)((void *)SS_input_primitive))->data[1],
                   ((MNSS_FACILITY_REQ_T *)((void *)SS_input_primitive))->data[1] + 1 );

      PUT_IMH_LEN( (((((MNSS_FACILITY_REQ_T *)((void *)SS_input_primitive))->data[1] + 
                           sizeof(msg_head_T)) - 
                           sizeof(IMH_T)) +  
                           sizeof(byte)),
                         (IMH_T *)((void *)&facility_msg));


      CNM_send_primitive_to_mm( connection_id,
                               PD_SS,
                               (IMH_T *)((void *)&facility_msg) );


   }
   else
   {
      ERR("Unable to send Facility msg\n", 0, 0, 0);
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
/**************** End of   CNM_process_mo_facility ********************************/


/**************** Begin of   CNM_process_mt_facility ******************************
 *
 *  Function name: CNM_process_mt_facility()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a facility or release complete message (containing
 *  a facility IE), received from the network
 *
 *  Parameters:
 *  -----------
 *  SS_input_primitive : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_mt_facility(  IMH_T *SS_input_primitive )
{
   byte                       *p_IE;
   byte                       IEI;
   byte                       IE_count;
   byte                       IE_length ;
   boolean                    syntax_ok;
   connection_id_T            connection_id;
   MNSS_FACILITY_IND_T        mnss_facility_ind;
   MNSS_END_IND_T             mnss_end_ind;
   boolean                    respond_to_event;
   byte                       type;
   boolean                    facility_found;
   boolean                    cause_found;
   byte                       *write_ptr;
   byte                       *last_octet_write_ptr;
   int                        length;

   p_IE = (byte *) 0;
   facility_found = FALSE;
   cause_found = FALSE;

   IE_count = 2;              /* start at first IE after message type */

   connection_id = CNM_get_connection_id((mmcnm_data_ind_T *)((void *)SS_input_primitive));
   syntax_ok = TRUE;

   if ( (connection_id < MAXNO_CC_TRANSACTIONS) && ((connection_id & 0x07) != 0x07))
   {
      type = ((msg_head_T *)((void *)SS_input_primitive))->type;
   
      if ((type == FACILITY) || (type == RELEASE_COMPLETE))
      {
         if (type == FACILITY)
         {
            mnss_facility_ind.message_header.message_set = MS_MN_SS;
            mnss_facility_ind.message_header.message_id = MNSS_FACILITY_IND;
            mnss_facility_ind.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
            {
              mnss_facility_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
            }
#endif
            write_ptr = &mnss_facility_ind.data[0];
            last_octet_write_ptr = &mnss_facility_ind.data[sizeof(mnss_facility_ind.data)-1];

         }
         else
         {
            mnss_end_ind.message_header.message_set = MS_MN_SS;
            mnss_end_ind.message_header.message_id = MNSS_END_IND;
            mnss_end_ind.connection_id = connection_id;
#ifdef FEATURE_DUAL_SIM 
            GET_IMH_LEN( length, &SS_input_primitive )
            if(MNCNM_IS_SINGLE_STACK)
            {
              mnss_end_ind.as_id = ss_transaction_information[connection_id].as_id;
            }
#ifdef FEATURE_DUAL_ACTIVE
            else
            {
              mnss_end_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
            }
#endif
#endif   
            write_ptr = &mnss_end_ind.data[0];
            last_octet_write_ptr = &mnss_end_ind.data[sizeof(mnss_end_ind.data)-1];
         }
   
         if ( no_of_IEs > MAXNO_IES_IN_MESSAGE )
         {
           ERR("no_of_IEs exceeds MAXNO_IES_IN_MESSAGE", 0, 0, 0);
           no_of_IEs = MAXNO_IES_IN_MESSAGE;
         }
         /*
          * set up write pointer ready to append the IEs
          */
   
         while (( syntax_ok != FALSE ) && ( IE_count < no_of_IEs ))
         {
   
            /*
             * get information of the next IE from global store
             */
   
            IEI       = cc_IE_info[IE_count].IEI;
            p_IE      = cc_IE_info[IE_count].start;
            IE_length = cc_IE_info[IE_count].length;
   
            syntax_ok = CNM_check_IE_syntax(IEI, p_IE,IE_count);
   
            if ( syntax_ok != FALSE )
            {
               switch (IEI)
               {
   
                  /*
                   * accept the following IEs
                   */
   
                  case IEI_FACILITY:  /* NOTE: this IE has no IEI in FACILITY msg */
   
                     if (facility_found == FALSE )
                     {
   
                        facility_found = TRUE;     /* ignore second facility */
   
                        /*
                         * Form mnss_facility_ind message with data
                         */
   
   
                        /* 
                        ** This IEI_FACILITY is already sanitised inside
                        ** CNM_check_IE_syntax
                        */
                        (void) memscpy( (void *) write_ptr,
                                        (last_octet_write_ptr - write_ptr + 1),
                                       (const void *) p_IE,
                                       IE_length );
   
                        *write_ptr = IEI_FACILITY;
   
                        write_ptr += IE_length;
   
                     }
                     break;
   
                  case IEI_CAUSE:
   
                     /*
                      * only allow if release_complete message
                      */
   
                     if (type == RELEASE_COMPLETE )
                     {
                        if (cause_found == FALSE)
                        {
                           cause_found = TRUE;
                           /********************************************************************************** 
                           * if the received size of the Cause IE is > GSM_CAUSE_SIZE, truncate it to GSM_CAUSE_SIZE. Cause IE is a 
                           * Type 4 IE whose max size is GSM_CAUSE_SIZE from Spec TS 24.080 Section 10.5.4.11.
                           **********************************************************************************/
                           if(*(p_IE + 1) > GSM_CAUSE_SIZE - 2)
                           {
                             MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Oversized Cause IE  size = %d,truncating it to %d",
                                        IE_length, GSM_CAUSE_SIZE);
                             IE_length   = GSM_CAUSE_SIZE;
                             *(p_IE + 1) = GSM_CAUSE_SIZE - 2;
                           }
                           (void) memscpy( (void *) write_ptr,
                                          (last_octet_write_ptr - write_ptr + 1),
                                          (const void *) p_IE,
                                          IE_length );
   
                           write_ptr += IE_length;
                        }
                     }
                     else
                     {
                        ERR("Unexpected cause IE in MT Facility msg\n",
                              0, 0, 0);
   
                        syntax_ok = FALSE;
                     }
                     break;
   
                  case IEI_USER_USER:
   
                     /*
                      * only allow if release_complete message
                      */
   
                     if (type != RELEASE_COMPLETE )
                     {
                        ERR("Unexpected USER_USER IE in Facility msg\n",
                                 0, 0, 0);
   
                        syntax_ok = FALSE;
                     }
                     break;
   
                  default:                            /* this cannot happen! */
                     ERR("Unexpected IE in Facility msg: received IEI = %d\n",
                            (dword) IEI, 0, 0);
   
                     syntax_ok = FALSE;
                     break;
               }
            }        /* ENDIF syntax is OK */
   
            IE_count++;       /* index next IE in the message */
   
         }   /* END WHILE more IEs and Syntax is OK */
   
         switch (type)
         {
            case FACILITY:
               if ( syntax_ok != FALSE )
               {
                  if(connection_id >= MAXNO_SS_TRANSACTIONS)
                  {
                    respond_to_event = FALSE;
                    MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
                  }
                  else
                  {
                    respond_to_event =
                     CNM_ss_respond_to_event( connection_id,
                                             MNSS_FACILITY_IND_event );
                  }
   
                  if ( respond_to_event != FALSE )
                  {
                     length =  (int)(((char *) write_ptr - (char *)&mnss_facility_ind) - (int)sizeof(IMH_T));
   
                     PUT_IMH_LEN( length, (IMH_T *)((void *)&mnss_facility_ind));
   
                     CNM_send_message( (IMH_T*)((void *)&mnss_facility_ind), GS_QUEUE_MN_CM );
                  }
                  else
                  {
                     ERR("Facility msg unexpected in this state: respond_to_event = FALSE\n",
                            0, 0, 0);
   
                     if ( CNM_get_ss_state(connection_id) == SS_IDLE )
                     {
#if defined(FEATURE_DUAL_SIM )
                     if(MNCNM_IS_SINGLE_STACK)
                     {
                        GET_IMH_LEN( length, &SS_input_primitive )
                        ss_transaction_information[connection_id].as_id = 
                                     (sys_modem_as_id_e_type)(((mmcnm_data_ind_T *)((void *)SS_input_primitive))->data[length - 3]);
                     }
#endif
   
                     CNM_send_release_complete(
                                               connection_id,
                                               PD_SS,
                                               INVALID_TRANSACTION_ID_VALUE,
                                               (ie_facility_T *) 0 
                                             );
                     }
                     else
                     {
                        CNM_send_release_complete(
                                    connection_id,
                                    PD_SS,
                                    MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,
                                    (ie_facility_T *) 0 
                                                );
   
                     }
   
                  }
               }
               else
               {
                  ERR("Syntax error in Facility msg\n", 0, 0, 0);
   
                  CNM_send_release_complete( connection_id,
                                             PD_SS,
                                             INVALID_MANDATORY_INFORMATION,
                                            (ie_facility_T *) 0 
                                             );
               }
               break;
   
            case RELEASE_COMPLETE:
   
               /*
                * if syntax error, process anyway!
                */
               if(connection_id >= MAXNO_SS_TRANSACTIONS)
               {
                 respond_to_event = FALSE;
                 MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
               }
               else
               {
                 respond_to_event =
                    CNM_ss_respond_to_event( connection_id,
                                            MNSS_END_IND_event );
               }
   
               if ( respond_to_event != FALSE )
               {
                  length = (int)(((char *) write_ptr -(char *)&mnss_end_ind) - (int)sizeof(IMH_T));
   
                  PUT_IMH_LEN(length, ((void *)&mnss_end_ind));
              
                  CNM_send_message( (IMH_T*)((void *)&mnss_end_ind), GS_QUEUE_MN_CM );
               }
               else
               {
                  ERR("Facility (Release Complete) msg unexpected in this state: respond_to_event = FALSE\n",
                          0, 0, 0);
   
                  if ( CNM_get_ss_state(connection_id) != SS_IDLE )
                  {
                     CNM_send_release_complete(
                                       connection_id,
                                       PD_SS,
                                       MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,
                                       (ie_facility_T *) 0 
                                       );
                  }
   
               }
               break;
   
            default :
   
                MSG_HIGH_DS_1(MN_SUB,"=MNCNM= CNM SS: Unknown message type: %d", type);
   
                break;
   
         } /* end switch */
      } /* end if FACILITY or RELEASE COMPLETE */
   }
   else
   {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Connection id should not be %d...ignored\n", connection_id);
   }
}
/**************** End of   CNM_process_mt_facility ********************************/
