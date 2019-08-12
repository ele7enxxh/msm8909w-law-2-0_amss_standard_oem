/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_cc_process_msgs.c_v   1.11   12 Jul 2002 17:11:36   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_cc_process_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

08/31/01   CD      Fixed lint/ADS110 compiler warnings

9/17/01    CD      Added processing for MMCNM_EST_CNF, MMCNM_DATA_IND,
                      MMCC_REEST_CNF, MMCNM_ABORT_IND, MMCNM_REL_IND, MMCNM_ERROR_IND
                      in CNM_handle_call_control

26-Oct-2001  VT   DTMF: In CNM_process_tch_assigned(), removed case U10_DTMF.

02-Apr-2002  VT   Bug Fix: In CNM_process_message_type(), if CC_DISCONNECT_event
                      is received in CC state CC_STATE_U19 or CC_STATE_U12, send MNCC_REL_IND to MN
                      in addition to sending RELEASE_COMPLETE to the n/w.

04/19/02   CD      Changed CNM_process_mm_conn_reject function call due to a 
                      prototype change

                   In CNM_handle_call_control(), changed processing of 
                   MMCNM_REL_IND, MMCNM_ERR_IND and MMCNM_ABORT_IND due to 
                   new interface between MM and CNM

                   Added case MMCC_REEST_REJ

                   Deleted processing of MMCNM_ABORT_IND.  It's now part of a 
                   MMCNM_REL_IND w/rel_cause set to ABORT_MSG_RECEIVED

                   Deleted processing of MMCNM_ERR_IND.  It's now part of a 
                   MMCNM_REL_IND w/rel_cause set to OTHER_CAUSE

                   For case MMCNM_EST_CNF replaced call to CNM_get_connection_id
                   by directly getting the conection id from the mm_connection_pending
                   global variable

                   Added processing for MMCNM_EST_REJ

                   In CNM_process_message_type(), merged processing of 
                   MMCNM_EST_IND and MMCNM_DATA_IND

                   Updated Copyright
                   Fixed lint error

04/30/02    CD    Changed processing of MMCNM_EST_CNF in CC_STATE_U0 (race condition) to start
                  clearing procedures by stoping all timers and sending a 
                  RELEASE COMPLETE followed by a MMCNM_REL_REQ

05/07/02    CD    Changed release type enum due to name collision with RR

05/15/02    CD    Correcting processing of MMCNM_EST_REJ to clear the 
                  mm_connection_pending.CNM_service_type

06/25/02    CD    In CNM_process_message_type(), when a DISCONNECT message is received
                  in the wrong state added cause IE to the MNCC_REL_IND primitive
                  sent to MN

07/29/02    CD    Modified processing of MMCNM_REL_IND w/ reest_possible set to TRUE

09/20/02    AB    In CNM_handle_call_control(), added handling for 
                  MNCC_REL_COMPL_REQ event, to defer RELEASE COMPLETE
                  response until MN completed the processing of the 
                  RELEASE message.  

10/12/02    AB    In CNM_process_tch_assigned(), pass the SYNC reason with
                  handoff direction to MN, so that it can reconfigure 
                  the Vocoder accordingly. 

11/16/02    AB    In CNM_handle_call_control(), added handling for invalid 
                  transaction ID, TI and message type at CC message router 
                  level.

02/10/03    AB    In CNM_handle_call_control(), added checking for valid
                  connection ID to pass Lint warning.

04/16/03    AB    In CNM_handle_call_control(), added supports for the
                  new reject type and reject cause received from MM. 

                  Added checking for race condition between user abort 
                  and connection establishment confirmation from MM.

05/11/05    HS    Added support for FEATURE_CCBS

06/14/05    HS    MMCC_SYNC_IND processed in all CC states including CC_STATE_U0.2~CC_STATE_U0.6

09/20/06    NR    Ignoring the Message if syntax error is received in MODIFY_REJECT/MODIFY_COMPLETE msg.

11/02/06    NR    Removing lint errors

07/06/11    PM    Guarding against NULL pointer access

09/26/11    PM    Whenever CNM gets AS_REJ_DEEP_FADE in MMCNM_REL_IND, it passes the same cause value to MN
                  and cause type as AS_REJECT_CAUSE. This change is part of FR 1337.
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "com_iei_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "mn_cnmtask.h"
#include "mn_cnmtask_int.h"
#include "ms.h"
#include "cause.h"

#include "nasutils.h"
#include "cmlib.h"
#include "cnm.h"
#include "mm.h"
#include <stringl/stringl.h>


#define MM_AS_ID_1 1
/*
 * global data/external function reference
 */
extern   struct MessageIETag  *next_ref;
extern boolean is_cs_umts_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_gsm_mode( sys_modem_as_id_e_type as_id );


/**************** Begin of CNM_fill_mmcnm_rej_ind ******************************
 *
 *  Function name: CNM_fill_mmcnm_rej_ind()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 * Function to fill mmcnm_est_rej structure depending upon the cause value and cause type
 * sent by MM in mmcnm_rel_ind.
 * 
 *
 *  Parameters:
 *  -----------
 *  
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_fill_mmcnm_rej_ind(mmcnm_est_rej_T *p_mmcnm_rej_ind, 
                                 byte             rel_ind_cause)
{

    if((rel_ind_cause >= SILENT_REDIAL_REJ_CAUSE_BEGIN) && 
       (rel_ind_cause <= SILENT_REDIAL_REJ_CAUSE_END))
    {
       p_mmcnm_rej_ind->cause_type = MM_REJECT_CAUSE;
       p_mmcnm_rej_ind->cause_value = rel_ind_cause;
    }
    else if(rel_ind_cause == AS_REJ_DEEP_FADE)
    {
       p_mmcnm_rej_ind->cause_type = AS_REJECT_CAUSE;
       p_mmcnm_rej_ind->cause_value = rel_ind_cause;
    }
    else
    {
       p_mmcnm_rej_ind->cause_type = OTA_REJECT_CAUSE;
       p_mmcnm_rej_ind->cause_value = rel_ind_cause;
    }

}

/**************** Begin of   CNM_handle_call_control ******************************
 *
 *  Function name: CNM_handle_call_control()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 * Function to route messages received for call control to sub-functions u
 * for processing
 *
 *  Parameters:
 *  -----------
 *  CNM_input_primitive : IN  - (IMH_T *)
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_handle_call_control( IMH_T * CNM_input_primitive)
{
    connection_id_T     connection_id;
    mmcnm_data_ind_T    *p_mmcnm_data_ind;
    mmcnm_rel_ind_T     *p_mmcnm_rel_ind;
    MNCC_REJ_IND_T      *p_mncc_rej_ind;
    byte                action;
    transaction_id_T    transaction_id;
    boolean             attempt_reestablish = FALSE;

   switch (CNM_input_primitive->message_set)
   {
      case MS_CC_MM:
      case MS_CC_MM_PS:

         switch (CNM_input_primitive->message_id)
         {
            case MMCNM_DATA_IND:

               p_mmcnm_data_ind = (mmcnm_data_ind_T *) ((void *)CNM_input_primitive);

               /* Validate Transaction ID */
               connection_id = CNM_get_connection_id(p_mmcnm_data_ind);
                  
               if((connection_id == UNKNOWN_CONNECTION_ID) ||
                  ((connection_id == 0x0F) &&    /* Extended Transaction ID */
                   (p_mmcnm_data_ind->msg_head.type == 0x00)))  /* reserved */
               {
                   /* Invalid Transaction ID */
#if defined(FEATURE_DUAL_SIM )
                   if(MNCNM_IS_SINGLE_STACK)
                   {
                      unsigned int     length;
                      GET_IMH_LEN (length, &(p_mmcnm_data_ind->msg_head.IMH));
                      if(connection_id == 0x0F)
                      {
                         /* RELEASE_COMPLETE for TID 0x0F is not correct ..
                          * If network sends TID 0x0F, NAS responds with TID 7*/
                         connection_id = 0x07;   
                      }
                      if(mn_call_information_p[connection_id] == NULL)
                      {
   #ifdef FEATURE_MODEM_HEAP
                          mn_call_information_p[connection_id] = (mn_call_information_T*)modem_mem_calloc(1, sizeof(mn_call_information_T),MODEM_MEM_CLIENT_NAS);
                          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= ALLOC - mn_call_information_p\n" );
   #else
                          mn_call_information_p[connection_id] = &mn_call_information[connection_id];
   #endif
                      }
                      mn_call_information_p[connection_id]->as_id = (sys_modem_as_id_e_type)p_mmcnm_data_ind->data[length -3];
                   }
#endif
                   CNM_send_release_complete( connection_id,
                                              PD_CC,
                                              INVALID_TRANSACTION_ID_VALUE,
                                              (ie_facility_T *) 0);
#if defined(FEATURE_DUAL_SIM )
                   if(MNCNM_IS_SINGLE_STACK)
                   {
                      if(mn_call_information_p[connection_id] != NULL)
                      {
#ifdef FEATURE_MODEM_HEAP          
                          modem_mem_free(mn_call_information_p[connection_id], MODEM_MEM_CLIENT_NAS);
                          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= FREE - mn_call_information_p\n" );
#endif
                          mn_call_information_p[connection_id] = NULL; 
                      }
                   }
#endif

               }
               else if (((p_mmcnm_data_ind->msg_head.type & 0x40) != 0) ||
                        ((p_mmcnm_data_ind->msg_head.type & 0x80) != 0))
               {
                   /*
                    * value 1 is reserved for possible future use.
                    * according to 04.07 11.2.4, a message received
                    * with this bit 1 should treat it as not defined
                    * for the PD
                    */
                   MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Invalid MSG TYPE or N(SD)=1: %d\n", connection_id,
                                                                  p_mmcnm_data_ind->msg_head.type);


                   if((connection_id < MAXNO_CC_TRANSACTIONS) &&
                      (cc_transaction_information_p[connection_id] != NULL) &&  
                      (cc_transaction_information_p[connection_id]->cc_state != CC_STATE_U0) &&
                      (cc_transaction_information_p[connection_id]->cc_state != CC_STATE_U0_1))
                   {
                       action = CNM_cc_check_respond_to_event( connection_id,
                                     UNDEFINED_MESSAGE_TYPE_event);

                       if (action != 0 )
                       {
                           CNM_cc_respond_to_event(connection_id, action );

                           CNM_send_status (connection_id,PD_CC,
                                               MESSAGE_TYPE_NON_EXISTENT );
                       }
                   }
                   else  /* Invalid message Type */
                   {
                       CNM_send_release_complete( connection_id,
                                                  PD_CC,
                                                  MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,
                                                  (ie_facility_T *) 0);
                   }
               }
               else
               {
                   CNM_process_message_type( p_mmcnm_data_ind );
               }

               break;

            case MMCC_SYNC_IND:

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
               /* Docomo RAB re-Configuration Hack */
               {
                  connection_id_T    index;
                  boolean            in_call_modification;
                  mmcc_sync_ind_T    *mmcc_sync_ind;
                  
                  mmcc_sync_ind = (mmcc_sync_ind_T *) ((void *)CNM_input_primitive);

                  for(index=0, in_call_modification=FALSE; 
                        ((index < MAXNO_CC_TRANSACTIONS) && (in_call_modification == FALSE)); index++)
                  {
                    if((cc_transaction_information_p[index] != NULL) &&
                        (mn_call_information_p[index] != NULL))
                    {
                      if ( (cc_transaction_information_p[index]->cc_state == CC_STATE_U26) ||
                           (cc_transaction_information_p[index]->cc_state == CC_STATE_U27) ||
                           ((cc_transaction_information_p[index]->cc_state == CC_STATE_U10) &&
                            (mn_call_information_p[index]->repeat_indicator_bc >= REPEAT_SUPPORTS_FALLBACK)) )
                      {
                        in_call_modification = TRUE;
                        break;
                      }
                    }
                  }
               
                  if((in_call_modification == TRUE) &&
                     (mmcc_sync_ind->reason == (byte)WCDMA_RAB_RELEASED))
                  {
                      /* ignore the RAB_RELEASE */
                      CNM_start_timer(TIMER_RAB_RELEASED, 0xff, 0xff
#if defined(FEATURE_DUAL_SIM )
                                    , mn_as_id //we don't want to give this as data to timer module in this timer
#endif
                                                 );
                  }
                  else if ((in_call_modification == TRUE) &&
                           (mmcc_sync_ind->reason == (byte)WCDMA_RAB_ESTABLISHED))
                  {
                      /* fake a RAB-Reconfiguration */
                      mmcc_sync_ind->reason = (byte)WCDMA_RAB_RECONFIGURED;

                      CNM_stop_timer(TIMER_RAB_RELEASED, 0xff);

                      CNM_process_tch_assigned( CNM_input_primitive );
                  }
                  else
                  {
                      CNM_process_tch_assigned( CNM_input_primitive );
                  }
               }
#else
               CNM_process_tch_assigned( CNM_input_primitive );
#endif /* defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6) */

               break;

            case MMCNM_EST_CNF:
                
               connection_id = mm_connection_pending.connection_id;

               action = CNM_cc_check_respond_to_event( connection_id,
                                                         MMCC_EST_CONF_event);

               if ((action != 0) && 
                   (cc_transaction_information_p[connection_id] != NULL)&&
                   (cc_transaction_information_p[connection_id]->cc_state != CC_STATE_U0))
               {
                  CNM_cc_respond_to_event( connection_id, action );

                  CNM_process_mm_conn_accepted();  /* also clear out mm_connection_pending service request */
               }
               else /* Race condition:  A MMCNM_EST_CNF is received in CC_STATE_U0 during a user abort */
               {
                  /* Stop all running timers */
                  CNM_stop_timer(ALL_CC_TIMERS, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));

                  /* clear out the MM pending service request */
                  mm_connection_pending.CNM_service_type = NO_CNM_SERVICE; 

                  if(mm_rel_req_sent == TRUE)
                  {
                      /* Change the mm_rel_req_sent flag to FALSE since we need to send 
                         a MMCNM_REL_REQ again */
                      mm_rel_req_sent = FALSE;
                      CNM_stop_timer(CNM_MM_REL_TIMER, 0xff);
                  }
				  
                  /* make sure pending EST is being service */
                  CNM_send_est_request_from_service_queue();  

                  /* Send a MMCNM_REL_REQ */
                  (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE
#if defined(FEATURE_DUAL_SIM )
                                                    , mn_as_id
#endif
                                                      );	
	
               }
  
               break;

            case MMCNM_EST_REJ:
                
               connection_id = mm_connection_pending.connection_id;

               action = CNM_cc_check_respond_to_event(connection_id, MMCC_EST_REJ_event);

               if (action != 0 )
               {
                  /*
                  * If this is in response to a MMCNM_REL_REQ,  
                  * and reset global flags
                  * Else, send a MNCC_REJ_IND to MN 
                  */
                  if (mm_rel_req_sent == TRUE)
                  {
                      mm_rel_req_sent = FALSE;
                      CNM_stop_timer(CNM_MM_REL_TIMER, 0);
                  }
                  else if((cc_transaction_information_p[connection_id] != NULL) &&
                         (!((mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT ||
                             mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT_FULL_SPEECH ||
                             mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT_HALF_SPEECH ||
                             mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_9600 ||
                             mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_2400 ||
                             mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_4800 ) &&
                            cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U0 )))
                  {
                      /* Above condition prevents triggering of a second MT_CALl_END_IND to CM if the 
                           Service Type is MO CAll & it has been already Cleared by User. This is required because 
                           CNM initiates Service Request to MM even if the User has Ended the MO CAll.  */
                           
                      mmcnm_est_rej_T      *mmcnm_rej_ind;

                      mmcnm_rej_ind = (mmcnm_est_rej_T *)((void *)CNM_input_primitive);

                      p_mncc_rej_ind = (MNCC_REJ_IND_T*) CNM_alloc( sizeof( MNCC_REJ_IND_T ));


                      if (CNM_setup_MNCC_REJ_IND(p_mncc_rej_ind, 
                                                 connection_id, 
                                                 mmcnm_rej_ind->cause_type, 
                                                 mmcnm_rej_ind->cause_value,
                                                 (byte *)NULL))
                      {
                          CNM_send_message((IMH_T *)p_mncc_rej_ind, GS_QUEUE_MN_CM );
                      }
                      else
                      {
                          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Invalid reject cause %d", connection_id);
                      }

                      CNM_free( p_mncc_rej_ind );
                  }

                  CNM_cc_respond_to_event(connection_id, action );
                  
               }
               mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;
               
               break;

            case MMCNM_REL_IND:
              {                
                p_mmcnm_rel_ind = (mmcnm_rel_ind_T *)((void *)CNM_input_primitive);

                if (p_mmcnm_rel_ind->reest_possible == TRUE)
                {
                    /* Service Type will be used in MNCC_REEST_REQ to MM, and as re-establishment is only for calls, assign default value as MO_CALL_ESTABLISHMENT */
                    CNMServiceType_T   CNM_service_type = MO_CALL_ESTABLISHMENT;                    
                    /* 
                    * First, look in all CC entities to see if they are in the 
                    * right state for the reestablishment to proceed
                    */
                    for (transaction_id= 0; 
                         transaction_id < MAXNO_CC_TRANSACTIONS; 
                         transaction_id++)
                    {
                       if ((cc_transaction_information_p[transaction_id] != NULL) &&
                           ((cc_transaction_information_p[transaction_id]->cc_state == CC_STATE_U10 ) ||
                           (cc_transaction_information_p[transaction_id]->cc_state == CC_STATE_U26 )) )
                       {
                            attempt_reestablish = TRUE;
			    /* If any one of the active call is of type emergency, reset the cnm_service_type to EMERGENCY_CALL_ESTABLISHMENT as MM has to 
                             * decide to include tmsi/imsi in CM-RESTABLISHMNET_REQUEST if emergency call is active. 
                             */
                             if( (mn_call_information_p[transaction_id] != NULL) &&
                                 (mn_call_information_p[transaction_id]->CNM_service_type == EMERGENCY_CALL_ESTABLISHMENT)  )
                             {
                                CNM_service_type = EMERGENCY_CALL_ESTABLISHMENT;
                             }
                       }
                    }

                    if (attempt_reestablish == TRUE)
                    {
                    /* 
                            Setting mn_as_id again here as Re-establishment is taking place.
                            It was reset in CNM_route
                          */
#if defined(FEATURE_DUAL_SIM )
                        if(MNCNM_IS_SINGLE_STACK)
                        {
                           mn_as_id = p_mmcnm_rel_ind->as_id;
                        }
#endif              
                        CNM_process_mm_establishment_lost(p_mmcnm_rel_ind, CNM_service_type);
                    }
                    else
                    {
                        mmcnm_rel_req_T   mmcnm_rel_req;
                        mmcnm_est_rej_T   mmcnm_rej_ind;
                        
                        mmcnm_rel_req.rel_type = CNM_NORMAL_RELEASE;
#if defined(FEATURE_DUAL_SIM )
                        if(MNCNM_IS_SINGLE_STACK)
                        {
                           mmcnm_rel_req.as_id = p_mmcnm_rel_ind->as_id;
                        }
#endif
                        CNM_send_mmcnm_rel_req(&mmcnm_rel_req.header); /* set MM back to idle state = CC_STATE_U19, from CC_STATE_U22 */

                        mm_rel_req_sent = TRUE;
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

                        MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Cannot attempt call-reestablishment");

                        CNM_fill_mmcnm_rej_ind(&mmcnm_rej_ind, p_mmcnm_rel_ind->rel_cause);
  
                        CNM_process_mm_conn_reject((void *)&mmcnm_rej_ind);
                    }
                }
                else
                {
                    mmcnm_est_rej_T   mmcnm_rej_ind;
                    CNM_fill_mmcnm_rej_ind(&mmcnm_rej_ind, p_mmcnm_rel_ind->rel_cause);
                    CNM_process_mm_conn_reject((void *)&mmcnm_rej_ind);
                }
              }

                break;

            case MMCC_REEST_CNF:
#if defined(FEATURE_DUAL_SIM )
                if(MNCNM_IS_SINGLE_STACK)
                {
                   mn_as_id = ((mmcc_reest_cnf_T *)((void *)CNM_input_primitive))->as_id;
                }
#endif
                CNM_process_mm_reestablishment();
                break;

            case MMCC_REEST_REJ:
                CNM_process_mm_conn_reject(CNM_input_primitive);
                break;

#ifdef FEATURE_CCBS
            case MMCNM_PROMPT_IND:
               {
                  start_cc_msg_T  msg;
                  connection_id_T index = 0;
                  boolean success = FALSE;

                  /* see if we have available TI for MO call. if so, reserve it */        
                  connection_id = UNKNOWN_CONNECTION_ID; /* Initialize to an invalid value */

                  for(index = MO; index <= MAX_MO_CONNECTION_ID; index++)
                  {
                     if((cc_transaction_information_p[index] == NULL) && (mn_call_information_p[index] == NULL))
                     {
                         success = MN_reserve_call_object(index);

                         if(success == TRUE)
                         {
                             connection_id = index;
                         }

                         index = UNKNOWN_CONNECTION_ID;                          
                         break;

                   }
                  }

                  
                  if ( connection_id <= MAX_MO_CONNECTION_ID )
                  {
                     action = CNM_cc_check_respond_to_event(connection_id, MMCNM_PROMPT_IND_event);

                     if (action != 0/*A_BAD_EVENT*/)
                     {
                        CNM_cc_respond_to_event(connection_id, action);

                        action = CNM_cc_check_respond_to_event(connection_id, MNCC_START_CC_REQ_event);

                        if (action != 0/*A_BAD_EVENT*/)
                        {
#if defined(FEATURE_DUAL_SIM )
                           if(MNCNM_IS_SINGLE_STACK)
                           {
                              if(mn_call_information_p[connection_id] != NULL)
                              {
                                mn_call_information_p[connection_id]->as_id = mn_as_id;
                              }
                           }
#endif
                           CNM_cc_respond_to_event(connection_id, action);

                           memset (&msg, 0x00, sizeof(start_cc_msg_T));

                           msg.msg_head.type = START_CC;

                           /* fill Call Control capability data */
                           msg.data[0] = IEI_CALL_CONTROL_CAPABIL;
                           msg.data[1] = 0x02;     /* length */
                           msg.data[2] = 0x01;     /* DTMF supported */
                           msg.data[3] = 0x00;     /* #Speech Bearer = 1 */

                           PUT_IMH_LEN ( 4/*cc capa*/ + sizeof(msg.msg_head.ti_pd) + sizeof(msg.msg_head.type),
                                         &msg.msg_head.IMH );

                           /* send START CC */
                           CNM_send_primitive_to_mm( connection_id, PD_CC, &msg.msg_head.IMH );
                           /* send MMCNM_PROMPT_RES */
                           CNM_send_mmcnm_prompt_res( TRUE, 
                                                      TRUE
#if defined(FEATURE_DUAL_SIM )
                                                      ,mn_as_id
#endif
                                                       );
                        }
                        else /* cannot be here */
                        {
                           MN_clear_call_data(connection_id);

                           MSG_ERROR_DS_1(MN_SUB,"=MNCNM= [ccbs]invalid action:%d",action);
                        }
                     }
                     else
                     {
                        if(cc_transaction_information_p[connection_id] != NULL)
                        {
                          MSG_ERROR_DS_1(MN_SUB,"=MNCNM= [ccbs]invalid state:%d",cc_transaction_information_p[connection_id]->cc_state); 
                        }
                          MN_clear_call_data(connection_id);

                          CNM_send_mmcnm_prompt_res( TRUE, 
                                                   FALSE
#if defined(FEATURE_DUAL_SIM )
                                                   ,((mmcnm_prompt_ind_T*)CNM_input_primitive)->as_id
#endif
                                                    );
                     }
                  }
                  else /* no TI available */
                  {
                     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= [ccbs]no TI available");
                     CNM_send_mmcnm_prompt_res( TRUE, 
                                                FALSE
#if defined(FEATURE_DUAL_SIM )
                                                ,((mmcnm_prompt_ind_T*)CNM_input_primitive)->as_id
#endif
                                                 ); /* yes we support NIMO but not now */
                  }
               }
               
               break;
#endif /* FEATURE_CCBS */

           default:
                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= CNM CC received Unexpected message_id = %d\n",
                CNM_input_primitive->message_id);
                 break;

         }/* end switch message_id */
         break; /* end case MS_CC_MM*/

      case MS_MN_CC:

         switch (CNM_input_primitive->message_id)
         {

            case MNCC_ALERT_REQ:
               CNM_process_mo_alert_request(CNM_input_primitive);
               break;

            case MNCC_CALL_CONF_REQ:
               CNM_process_mt_call_confirm(CNM_input_primitive);
               break;

            case MNCC_CALL_HOLD_REQ:
               CNM_process_mo_call_hold_request(CNM_input_primitive);

               break;

            case MNCC_CALL_RETRIEVE_REQ:
               CNM_process_mo_call_retrieve_request(CNM_input_primitive);

               break;

            case MNCC_DISC_REQ:
               CNM_process_mo_disc_request(CNM_input_primitive);

               break;

            case MNCC_FACILITY_REQ:
               CNM_process_mo_facility_request(CNM_input_primitive);

               break;

            case MNCC_REJ_REQ:
               CNM_process_mt_call_reject(CNM_input_primitive);

               break;

            case MNCC_REL_REQ:
               CNM_process_mo_release_request(CNM_input_primitive);

               break;

            case MNCC_REL_COMPL_REQ:
               CNM_process_mo_release_complete_request(CNM_input_primitive);

               break;

            case MNCC_MODIFY_REQ:
               CNM_process_mo_modify_request(CNM_input_primitive);

               break;

            case MNCC_MODIFY_RES:
               CNM_process_mt_modify_response(CNM_input_primitive);

               break;

            case MNCC_NOTIFY_REQ:
               CNM_process_mo_notify_request();

               break;

            case MNCC_SETUP_COMPL_REQ:
               CNM_process_user_conn_confirmation(CNM_input_primitive);

               break;

            case MNCC_SETUP_COMPL_RES:
               CNM_process_mt_call_setup_completion(CNM_input_primitive);

               break;

            case MNCC_SETUP_REQ:
               CNM_process_mo_call_request(CNM_input_primitive);

               break;

            case MNCC_SETUP_RES:
               CNM_process_mt_call_answer(CNM_input_primitive);

               break;

            case MNCC_START_DTMF_REQ:
               CNM_process_mo_start_dtmf_request(
                              (MNCC_START_DTMF_REQ_T *)((void *)CNM_input_primitive));

               break;

            case MNCC_STOP_DTMF_REQ:
               CNM_process_mo_stop_dtmf_request(
                              (MNCC_STOP_DTMF_REQ_T *)((void *)CNM_input_primitive));

               break;

#ifdef FEATURE_UUS
            case MNCC_USER_USER_DATA_REQ:
               CNM_process_mo_user_user_data_request(CNM_input_primitive);

               break;
#endif

            case MNCC_MODIFY_ACCEPT:
               CNM_process_mo_modify_accept(CNM_input_primitive);
               break;

            case MNCC_SRVCC_HANDOVER_FAIL_REQ:
               CNM_process_srvcc_handover_fail_req(CNM_input_primitive);
               break;

            default:
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected Message id: given message_id = %d\n",
                        (dword) (CNM_input_primitive->message_id));
               break;
         }/* end switch message_id */
         break; /* end case MS_MN_CC */
      
      default:
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected Message set: given message_set = %d\n",
                   (dword) (CNM_input_primitive->message_set));
         break;

   } /* end switch message_set */

}

/**************** End of   CNM_handle_call_control ********************************/


/**************** Begin of   CNM_process_message_type *****************************
 *
 *  Function name: CNM_process_message_type()
 *  ----------------------------------------
 *
 *  Description:
 *  ------------
 * Function to perform preliminary processing based on the message type
 * field of the layer three message.  The message is a MMCNM_DATA_IND.
 * No other messages will be processed here.
 *
 *  Parameters:
 *  -----------
 *  CNM_input_primitive : IN  - (mmcnm_data_ind_T *)
 *
 *  Returns:
 *  --------
 *  None
 *
 *  Note:
 *  ----
 *     When the message type received is SETUP,  this function checks to see 
 *     if this is the start of a MT transaction.
 *
 *     If the MMCNM_DATA_IND is received with the following values
 *         a) message_type != SETUP,
 *         b) TI corresponds to a CC entity in CC_STATE_U0 state,
 *     the state table returns NULL and a STATUS message w/ 
 *     MSG_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE is sent to the network
 *     
 ******************************************************************************/

VOID_FUNC CNM_process_message_type( mmcnm_data_ind_T * p_mmcnm_data_ind)
{

   int                     msg_len;
   connection_id_T         connection_id;
   struct MsgDetailTag     *p_message_details;
   event_T                 cc_event;
   byte                    action_code;
   boolean                 U0_exception;
   boolean                 unknown_message;
   boolean                 est_ind = FALSE;
   boolean                 cause_ie_syntax = FALSE;


   MNCC_REL_IND_T          *p_mncc_rel_ind;

   p_mncc_rel_ind = (MNCC_REL_IND_T*) CNM_alloc( sizeof( MNCC_REL_IND_T ));

   /* The following detects messages which dont generate
    * RELEASE_COMPLETE if received in state CC_STATE_U0
    */

   U0_exception = FALSE;

   /*
    * assume that message is not known for the PD
    */

   unknown_message = TRUE;

   GET_IMH_LEN(msg_len, &p_mmcnm_data_ind->msg_head.IMH);

   /*
    * check that there is a L3 message on the end of this message
    */

   if ( msg_len != 0 )
   {

      /*
       * first byte of data is protocol_descriminator/transaction id
       */

      connection_id = CNM_get_connection_id( p_mmcnm_data_ind );

      if (((connection_id & 0x07) != 0x07) &&
           (connection_id != 0x0F))
      {

         /* check the message type using the following call */

         p_message_details =
            CNM_setup_IE_data_read(p_mmcnm_data_ind->msg_head.type, MT_CALL);

         if (p_message_details  != (struct MsgDetailTag *) 0 )
         {
            unknown_message = FALSE;  /* assume we recognise this message */

            next_ref = GET_FIRST_REF(p_message_details);

            switch (p_mmcnm_data_ind->msg_head.type)
            {

                case CALL_PROCEEDING:
                   cc_event = CC_CALL_PROCEEDING_event;
                   break;

                case ALERTING:
                   cc_event = CC_ALERTING_event;
                   break;

                case CONNECT:
                   cc_event = CC_CONNECT_event;
                   break;

                case PROGRESS:
                   cc_event = CC_PROGRESS_event;
                   break;

                case CONNECT_ACK:
                   cc_event = CC_CONNECT_ACK_event;
                   break;

                case RELEASE:
                   cc_event = CC_RELEASE_event;
                   break;

                case DISCONNECT:
                   cc_event = CC_DISCONNECT_event;
                   break;

                case RELEASE_COMPLETE:
                   U0_exception = TRUE;
                   cc_event = CC_RELEASE_COMPLETE_event;
                   break;

                case MODIFY:
                   cc_event = CC_MODIFY_event;
                   break;

                case MODIFY_COMPLETE:
                   cc_event = CC_MODIFY_COMPLETE_event;
                   break;

                case MODIFY_REJECT:
                   cc_event = CC_MODIFY_REJECT_event;
                   break;

                case START_DTMF_ACK:
                   cc_event = CC_START_DTMF_ACK_event;
                   break;

                case STOP_DTMF_ACK:
                   cc_event = CC_STOP_DTMF_ACK_event;
                   break;

                case START_DTMF_REJ:
                   cc_event = CC_START_DTMF_REJECT_event;
                   break;

                case STATUS_ENQUIRY:
                   cc_event = CC_STATUS_ENQUIRY_event;
                   break;

                case STATUS_MSG:
                   cc_event = CC_STATUS_event;
                   U0_exception = TRUE;
                   break;

                case FACILITY:
                   cc_event = CC_FACILITY_event;
                   break;

                case USER_INFORMATION:
                   cc_event = CC_USER_INFORMATION_event;
                   break;

                case CONGESTION_CONTROL:
                   cc_event = CC_CONGESTION_CONTROL_event;
                   break;

                case NOTIFY:
                   cc_event = CC_NOTIFY_event;
                   break;

                case HOLD_ACK:
                   cc_event = CC_HOLD_ACK_event;
                   break;

                case HOLD_REJECT:
                   cc_event = CC_HOLD_REJECT_event;
                   break;

                case RETRIEVE_ACK:
                   cc_event = CC_RETRIEVE_ACK_event;
                   break;

                case RETRIEVE_REJ:
                   cc_event = CC_RETRIEVE_REJECT_event;
                   break;

                case SETUP:

                   U0_exception = TRUE;

                   /*
                    * verify this is the start of a MT transaction
                    */
                   est_ind = CNM_check_for_establish_ind(p_mmcnm_data_ind);

                   if (est_ind == TRUE)
                   {
                      cc_event = MMCC_EST_IND_event;
                   }
                   else
                   {
                      GS_SET_UNDEFINED(cc_event);   /* ignore the message */
                   }
                   break;

#ifdef FEATURE_CCBS
                case CC_ESTABLISHMENT:
                   cc_event = CC_CC_ESTABLISHMENT_event;
                   break;

                case RECALL:
                   cc_event = CC_RECALL_event;
                   break;
#endif /* FEATURE_CCBS */

                default:
                     unknown_message = TRUE;  /* set flag to make cause 97  */
                                              /* status happen              */

                     GS_SET_UNDEFINED(cc_event);

                     MSG_ERROR_DS_1(MN_SUB,"=MNCNM= No event for message type: given message type = %d\n",
                             (dword) (p_mmcnm_data_ind->msg_head.type));
                   break;
            }/* end switch */

            /*
             * if event should not necessarily be ignored
             */

            if ( GS_IS_NOT_UNDEFINED(cc_event) )
            {
               action_code = CNM_cc_check_respond_to_event( connection_id,
                                                                cc_event );

              if (cc_event == CC_MODIFY_event)
              {
#ifdef FEATURE_WCDMA
                mn_store_rab_status();
#endif
              }

               if ( action_code != 0 )
               {
                  CNM_process_message_content( connection_id,
                                               p_mmcnm_data_ind,
                                               action_code);

               }
               else
               {
                  if((cc_transaction_information_p[connection_id] == NULL )
                    ||( cc_transaction_information_p[connection_id]->cc_state
                                                                      == CC_STATE_U0 )
                    ||( cc_transaction_information_p[connection_id]->cc_state
                                                                      == CC_STATE_U0_1 ))
                  {
                     if ( U0_exception == FALSE )
                     {

                        /*
                         * send release complete message
                         */

#if defined(FEATURE_DUAL_SIM )

                          /* temp_mem_assigned - used to track if in following block memory is assigned 
                           * temporarily so that it can be freed after use */
                          byte temp_mem_assigned = FALSE; 
                          unsigned int   length;
                          GET_IMH_LEN (length, &(p_mmcnm_data_ind->msg_head.IMH));
                          if(MNCNM_IS_SINGLE_STACK)
                          {
   
                             if(mn_call_information_p[connection_id] == NULL)
                             {
                               temp_mem_assigned = TRUE;
      
#ifdef FEATURE_MODEM_HEAP
                               mn_call_information_p[connection_id] = (mn_call_information_T*)modem_mem_calloc(1, sizeof(mn_call_information_T),MODEM_MEM_CLIENT_NAS);
                               MSG_HIGH_DS_0(MN_SUB,"=MNCNM= ALLOC - mn_call_information_p\n" );
#else
                               mn_call_information_p[connection_id] = &mn_call_information[connection_id];
#endif
                             }
                             mn_call_information_p[connection_id]->as_id = (sys_modem_as_id_e_type)p_mmcnm_data_ind->data[length -3];
                          }
#endif
                       CNM_send_release_complete( connection_id,
                                                  PD_CC,
                                                  INVALID_TRANSACTION_ID_VALUE,
                                                  (ie_facility_T *) 0);
#if defined(FEATURE_DUAL_SIM )
                        if(MNCNM_IS_SINGLE_STACK)
                        {
   
                           if((mn_call_information_p[connection_id] != NULL) &&
                              (temp_mem_assigned == TRUE))
                           {
#ifdef FEATURE_MODEM_HEAP          
                             modem_mem_free(mn_call_information_p[connection_id], MODEM_MEM_CLIENT_NAS);
                             MSG_HIGH_DS_0(MN_SUB,"=MNCNM= FREE - mn_call_information_p\n");
#endif
                             mn_call_information_p[connection_id] = NULL;  
                           }     
                        }
#endif
                     }
                  }
                  else
                  if ( cc_event == MMCC_EST_IND_event )
                  {
                     ; /* ignore message */
                  }
                  else
                  if ( cc_event == CC_RELEASE_event )
                  {

                     CNM_send_release_complete(
                                 connection_id,
                                 PD_CC,
                                 MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,
                                 (ie_facility_T *) 0);

                     /*
                      * set this connection call state back to CC_STATE_U0
                      */

                     CNM_set_state_to_U0( connection_id );
                  }
                  else if ( cc_event == CC_DISCONNECT_event )
                  {
                      if ( (cc_transaction_information_p[connection_id] != NULL) &&
                          (( cc_transaction_information_p[connection_id]->cc_state
                                                                      == CC_STATE_U19 )
                            || 
                           ( cc_transaction_information_p[connection_id]->cc_state
                                                                      == CC_STATE_U12 )) )
                      {
                          p_mncc_rel_ind->message_header.message_set = MS_MN_CC;
                          p_mncc_rel_ind->message_header.message_id  = MNCC_REL_IND;
                          p_mncc_rel_ind->connection_id = connection_id;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
                          {
                             p_mncc_rel_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                          }
#endif

                          p_mncc_rel_ind->cause = 0;

                          /* Get the mandatory cause IE from the DISCONNECT msg   */
                          /* This IE has LV format, and is the first IE after the */
                          /* ti_pd and the msg type, so it should start at the    */
                          /*  first element in the data array                     */

                          if ((p_mmcnm_data_ind->data[0] > 0) && 
                              (p_mmcnm_data_ind->data[0] <= GSM_CAUSE_SIZE))
                          {
                              /* Copy the IE to the release indication data structure */

                              p_mncc_rel_ind->data[0] = IEI_CAUSE;
                              p_mncc_rel_ind->data[1] = p_mmcnm_data_ind->data[0];

                              memscpy(&(p_mncc_rel_ind->data[2]), sizeof(p_mmcnm_data_ind->data)-2,
                                     &(p_mmcnm_data_ind->data[1]), p_mmcnm_data_ind->data[0]);

                              /* Check IEI syntax */
                              /* Note that this function needs an IE formatted as TLV */
                              cause_ie_syntax = CNM_check_IE_syntax (IEI_CAUSE,
                                                       p_mncc_rel_ind->data, MAXNO_IES_IN_MESSAGE);
                              if (cause_ie_syntax == FALSE)
                              {
                                  /* zero out the cause copied to the release indication */
                                  memset(p_mncc_rel_ind->data, 0x00, GSM_CAUSE_SIZE);
                              }
                          }

                          PUT_IMH_LEN(sizeof(MNCC_REL_IND_T) - sizeof(IMH_T), 
                                      &p_mncc_rel_ind->message_header);

                          CNM_send_message( (IMH_T *)p_mncc_rel_ind,
                                            GS_QUEUE_MN_CM );

                          CNM_send_release_complete(
                                 connection_id,
                                 PD_CC,
                                 MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,
                                 (ie_facility_T *) 0);
                      }
                      else
                      {
#if defined(FEATURE_DUAL_SIM )
                          unsigned int    length;
                          GET_IMH_LEN (length, &(p_mmcnm_data_ind->msg_head.IMH));
                          if(MNCNM_IS_SINGLE_STACK)
                          {
                             if(mn_call_information_p[connection_id] != NULL)
                             {
                               mn_call_information_p[connection_id]->as_id = (sys_modem_as_id_e_type)p_mmcnm_data_ind->data[length -3];
                             }
                          }
#endif
                      
                          CNM_send_release_complete(
                                 connection_id,
                                 PD_CC,
                                 INVALID_TRANSACTION_ID_VALUE,
                                 (ie_facility_T *) 0);

                      }

                      /*
                       * set this connection call state back to CC_STATE_U0
                       */

                      CNM_set_state_to_U0( connection_id );
                  }
                  else
                  {
                     CNM_send_status(connection_id, PD_CC,
                                     MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE);
                  }
               }/* end if action_code != 0 */
            }/* end if GS_IS_NOT_UNDEFINED */
         }
         if (unknown_message != FALSE )
         {

            /* ie if message not known for PD */

            CNM_send_status(connection_id, PD_CC, MESSAGE_TYPE_NON_EXISTENT);
         }
      }
      else
      {
         MSG_MED_DS_0(MN_SUB,"=MNCNM= TI value should not be 7...ignored");
      }
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Not enough data in message\n");
   }

   CNM_free( p_mncc_rel_ind );

}
/**************** End of   CNM_process_message_type ********************************/


/**************** Begin of   CNM_process_tch_assigned *****************************
 *
 *  Function name: CNM_process_tch_assigned()
 *  ----------------------------------------
 *
 *  Description:
 *  ------------
 *  This function responds to the sync information from mm, indicating that
 *  a traffic channel has been assigned, or a channel mode has been changed.
 *
 *
 *  Parameters:
 *  -----------
 *  message : INPUT
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_process_tch_assigned( IMH_T * message )
{
   MNCC_SYNC_IND_T   mncc_sync_ind;
   mmcc_sync_ind_T   *p_mmcc_sync_ind;
   connection_id_T   connection_id;
   boolean           sync_sent;
   boolean           success = FALSE;
   boolean           is_G_RAT = FALSE;
   boolean           is_W_RAT = FALSE;


   p_mmcc_sync_ind = (mmcc_sync_ind_T *) ((void *)message);

   sync_sent = FALSE;

   mncc_sync_ind.message_header.message_set = MS_MN_CC;
   mncc_sync_ind.message_header.message_id  = MNCC_SYNC_IND;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
      mncc_sync_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   mncc_sync_ind.reason = (sync_enum_T)p_mmcc_sync_ind->reason;  /* determine handoff direction */


#ifdef FEATURE_DUAL_SIM
   if(MNCNM_IS_SINGLE_STACK)
   {
       sys_modem_as_id_e_type local_as_id = SYS_MODEM_AS_ID_NONE;
     if(mn_as_id != SYS_MODEM_AS_ID_NONE)
     {
       local_as_id = mn_as_id;
     }
     else if(p_mmcc_sync_ind->as_id != SYS_MODEM_AS_ID_NONE)
     {
       local_as_id = p_mmcc_sync_ind->as_id;
     }
     else
     {
       MSG_ERROR_DS_0(MN_SUB,"=MNCNM= CNM_process_tch_assigned ! Error in as_id");
     }
     
	 is_G_RAT = is_cs_gsm_mode(local_as_id);
     is_W_RAT = is_cs_umts_mode(local_as_id);
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     is_G_RAT = is_cs_gsm_mode((sys_modem_as_id_e_type)mn_dsda_as_id);
     is_W_RAT = is_cs_umts_mode((sys_modem_as_id_e_type)mn_dsda_as_id);
   }
#endif
#else
   is_G_RAT = is_cs_gsm_mode(MM_AS_ID_1);
   is_W_RAT = is_cs_umts_mode(MM_AS_ID_1);
#endif


   if ( is_G_RAT == TRUE )
   {
      mncc_sync_ind.tch_rate = p_mmcc_sync_ind->tch_rate;
      mncc_sync_ind.sub_channel = p_mmcc_sync_ind->subchannel;
      mncc_sync_ind.channel_mode = p_mmcc_sync_ind->channel_mode;
   }
   else if (is_W_RAT)
   {
      mncc_sync_ind.radio_access_bearer_id = p_mmcc_sync_ind->radio_access_bearer_id;
#ifdef FEATURE_VOC_AMR_WB 
      mncc_sync_ind.umts_codec = p_mmcc_sync_ind->umts_codec;
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= UMTS codec type = %d",p_mmcc_sync_ind->umts_codec);
#endif /* FEATURE_VOC_AMR_WB  */
#ifdef FEATURE_CS_VOICE_OVER_HSPA
      mncc_sync_ind.cs_voice_type = p_mmcc_sync_ind->cs_voice_type;
      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= UMTS cs_voice_type = %d",p_mmcc_sync_ind->cs_voice_type);
#endif
   }
   else
   {
      ERR("Invalid operating mode: gsm=0, wcdma=0", 0, 0, 0);
   }

   PUT_IMH_LEN( (sizeof(MNCC_SYNC_IND_T) - sizeof(IMH_T)) ,
                  &mncc_sync_ind.message_header);

   /*
    * scan through all possible connections.
    */

   if(mn_waiting_for_tch_in_srvcc == FALSE)
   {
     for (connection_id = 0;
          connection_id < MAXNO_CC_TRANSACTIONS;
          connection_id++ )
     {

        /*
         * do check on all transaction ids except 7 and 15 as these
         * are invalid
         */

        if ((connection_id & 0x07) != 0x07)
        {
           if ( cc_transaction_information_p[connection_id] != NULL 
                 && cc_transaction_information_p[connection_id]->cc_state != CC_STATE_U0 )
           {
             mncc_sync_ind.connection_id = connection_id;
             CNM_send_message((IMH_T *)&mncc_sync_ind, GS_QUEUE_MN_CM );
             sync_sent = TRUE;
           }
        }
     }
   }
   else
   {
     /* If mn_waiting_for_tch_in_srvcc is TRUE, send only one SYNC_IND to MN,
      * else vocoder acquisition may have problem */
 
     for (connection_id = 0;
          ((connection_id < MAXNO_CC_TRANSACTIONS) && (sync_sent == FALSE));
          connection_id++ )
     {

        /*
         * do check on all transaction ids except 7 and 15 as these
         * are invalid
         */

        if ((connection_id & 0x07) != 0x07)
        {
           if ( cc_transaction_information_p[connection_id] != NULL 
                 && cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U10 )
           {
             mncc_sync_ind.connection_id = connection_id;
             CNM_send_message((IMH_T *)&mncc_sync_ind, GS_QUEUE_MN_CM );
             sync_sent = TRUE;
           }
        }
     }     
	   
     for (connection_id = 0;
          ((connection_id < MAXNO_CC_TRANSACTIONS) && (sync_sent == FALSE));
          connection_id++ )
     {

        /*
         * do check on all transaction ids except 7 and 15 as these
         * are invalid
         */

        if ((connection_id & 0x07) != 0x07)
        {
           if ( cc_transaction_information_p[connection_id] != NULL 
                 && cc_transaction_information_p[connection_id]->cc_state != CC_STATE_U0 )
           {
             mncc_sync_ind.connection_id = connection_id;
             CNM_send_message((IMH_T *)&mncc_sync_ind, GS_QUEUE_MN_CM );
             sync_sent = TRUE;
           }
        }
     }     
   }


   /*
    * if none of the connection ids are active, then send a sync anyway
    * but with connection id 7. This is normally invalid, but is used here
    * to indicate that the traffic channel has been assigned anyway. This
    * happens when the assignmewnt is made before the first CC message
    */

   if ( !sync_sent )
   {
      mncc_sync_ind.connection_id = 0x07;
      connection_id = 0x07;
      if((cc_transaction_information_p[connection_id] == NULL) && (mn_call_information_p[connection_id] == NULL))
      {

          success = MN_reserve_call_object(connection_id);
          if(success == TRUE)
          {
          
#if defined(FEATURE_DUAL_SIM )
             if(MNCNM_IS_SINGLE_STACK)
             {
                mn_as_id = p_mmcc_sync_ind->as_id;
                mn_call_information_p[mncc_sync_ind.connection_id]->as_id = mn_as_id;
             }
#endif  

             CNM_send_message((IMH_T *)&mncc_sync_ind,
                            GS_QUEUE_MN_CM );             
          }
          else
          {
             ERR("Not propagating MMCC_SYNC_IND to MN", 0, 0, 0); 
          }
          
      }
      else
      {
        ERR("Memory ALLOC failure - cc_transaction_information_p[%d] not NULL", connection_id, 0, 0); 
        ERR("Memory ALLOC failure - mn_call_information_p[%d] not NULL", connection_id, 0, 0); 
      }
   }

}
/**************** End of   CNM_process_tch_assigned ********************************/


/**************** Begin of   CNM_process_message_content **************************
 *
 *  Function name: CNM_process_message_content()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to deal with messages received from the network. It checks
 *  the message for protocol errors, does the detailed processing 
 *  depending on the message type, and sends any resulting messages
 *  on to MN
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN
 *  message : IN
 *  action_code : IN - Parameter which actions to perform
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_process_message_content(  connection_id_T connection_id, 
                                          mmcnm_data_ind_T * message,
                                          byte   action_code )
{
   boolean     valid_message;
   int         length;
   boolean                    syntax_ok;
   byte                         cause;
   valid_message = CNM_check_for_protocol_errors(message);

   if ( valid_message != FALSE )
   {

      /*
       * safety check on message to be processed - must  have a length of at least 
       * 2 to include the type
       */

      GET_IMH_LEN( length, &message->msg_head.IMH );
#if defined(FEATURE_DUAL_SIM )
      if(MNCNM_IS_SINGLE_STACK)
      {
        length = length - sizeof(byte);
      }
#endif

      if ( (unsigned int)(length) >= ( sizeof( msg_head_T ) - sizeof( IMH_T ) ) ) 
      {

         syntax_ok = CNM_perform_check_on_IEs(message->msg_head.type, connection_id, &cause);

         /* Execute the action_code before going to message specific handler */
         switch ( message->msg_head.type )
         {
            case MODIFY_COMPLETE:
            case MODIFY_REJECT:
                if ( syntax_ok == TRUE) /* This check is put as part of fix for CR 101241 */
                {
                  CNM_cc_respond_to_event( connection_id, action_code );
                }
               break;
            default:
               CNM_cc_respond_to_event( connection_id, action_code );
               break;
         }

         /*
          * cast pointer to enable extraction of message type
          */

         switch ( message->msg_head.type )
         {
            case ALERTING:
               CNM_process_alerting_message( connection_id, syntax_ok, cause);
               break;

            case CALL_PROCEEDING:
               CNM_process_call_proceeding_message( connection_id, syntax_ok, cause);
               break;

            case CONNECT:
               CNM_process_connect_message( connection_id, syntax_ok, cause);
               break;

            case CONNECT_ACK:
               CNM_process_connect_ack_message( connection_id );
               break;

            case PROGRESS:
               CNM_process_progress_message( connection_id, syntax_ok, cause);
               break;

            case SETUP:
               CNM_process_setup_message( connection_id, syntax_ok, cause);
               break;

            case MODIFY:
               CNM_process_modify_message( connection_id, syntax_ok, cause);
               break;

            case DISCONNECT:
               CNM_process_disconnect_message( connection_id, syntax_ok, cause);
               break;

            case RELEASE:
               CNM_process_release_message( connection_id, syntax_ok, cause);
               break;

            case RELEASE_COMPLETE:
               CNM_process_release_complete_message( connection_id, syntax_ok, cause);
               break;

            case FACILITY:
               CNM_process_facility_message( connection_id, syntax_ok, cause);
               break;

            case HOLD_REJECT:
               CNM_process_hold_rej_message( connection_id );
               break;

            case HOLD_ACK:
               CNM_process_hold_ack_message( connection_id );
               break;

            case RETRIEVE_ACK:
               CNM_process_retrieve_ack_message( connection_id );
               break;

            case RETRIEVE_REJ:
               CNM_process_retrieve_rej_message( connection_id );
               break;

            case START_DTMF_ACK:
               CNM_process_start_dtmf_ack_message( connection_id, syntax_ok, cause);
               break;

            case START_DTMF_REJ:
               CNM_process_start_dtmf_rej_message( connection_id, syntax_ok, cause);
               break;

            case STOP_DTMF_ACK:
               CNM_process_stop_dtmf_ack_message( connection_id);
               break;

            case STATUS_MSG:
               CNM_process_status_message( connection_id, syntax_ok, cause);
               break;

            case STATUS_ENQUIRY:
               CNM_process_status_enquiry_message( connection_id);
               break;

#ifdef FEATURE_UUS
            case USER_INFORMATION:
               CNM_process_user_information_message( connection_id, syntax_ok, cause);
               break;

            case CONGESTION_CONTROL:
               CNM_process_congestion_control_message( connection_id, syntax_ok, cause);
               break;
#endif

            case NOTIFY:
               CNM_process_notify_message( connection_id, syntax_ok, cause);
               break;

            case MODIFY_COMPLETE:   /* same function processes both */
            case MODIFY_REJECT:
               CNM_process_modify_complete_reject_message( connection_id, syntax_ok, cause);
               break;

#ifdef FEATURE_CCBS
            case CC_ESTABLISHMENT:
               CNM_process_cc_establishment_message( connection_id, syntax_ok);
               break;

            case RECALL:
               CNM_process_recall_message( connection_id, syntax_ok);
               break;
#endif /* FEATURE_CCBS */

            default:
               MSG_MED_DS_0(MN_SUB,"=MNCNM= CM:Unexpected message type" );
               break;
                 
           }
      }
      else
      {
          MSG_MED_DS_0(MN_SUB,"=MNCNM= CM:Bad message header");
      } 
   }
   else
   {
      MSG_MED_DS_0(MN_SUB,"=MNCNM= CM:Protocol errors in message" );
   }
}
/**************** End of   CNM_process_message_content ********************************/


/**************** Begin of   CNM_perform_check_on_IEs **************************
 *
 *  Function name: CNM_perform_check_on_IEs()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to deal with messages received from the network. It checks
 *  the message for syntax errors.
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN
 *  error_cause : IN
 *   
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

boolean CNM_perform_check_on_IEs(byte message_type, connection_id_T connection_id, byte *error_cause)
{

    boolean syntax_ok = TRUE;
    byte IE_count = 2;
    byte                       IEI;
    byte                       *p_IE;
    byte                       IE_length;
    byte                       *p_progress_description;
    MNCC_FACILITY_IND_T        mncc_facility_ind;
    MNCC_PROGRESS_IND_T        mncc_progress_ind;
    byte                       *p_cause_value;
    byte                         count;


#ifdef FEATURE_UUS
    MNCC_USER_USER_DATA_IND_T  mncc_user_user_data_ind;
    memset(&mncc_user_user_data_ind, NULL, sizeof(mncc_user_user_data_ind));
#endif

    *error_cause = INVALID_MANDATORY_INFORMATION;
   /* Initialize buffer */
    memset(&mncc_facility_ind, NULL, sizeof(mncc_facility_ind));
    memset(&mncc_progress_ind, NULL, sizeof(mncc_progress_ind));


    if((message_type == CONNECT_ACK) ||
               (message_type == HOLD_REJECT) ||
               (message_type == HOLD_ACK) ||
               (message_type == RETRIEVE_ACK) ||
               (message_type == RETRIEVE_REJ) ||
               (message_type == STOP_DTMF_ACK) ||
               (message_type == STATUS_ENQUIRY))
    {
      syntax_ok = FALSE;
    }


    while(syntax_ok && (IE_count < no_of_IEs))
    {
      IEI       = cc_IE_info[IE_count].IEI;
      p_IE      = cc_IE_info[IE_count].start;
      IE_length = cc_IE_info[IE_count].length;

      syntax_ok = CNM_check_IE_syntax(IEI, p_IE,IE_count);

      cc_IE_info[IE_count].syntax_ok = (byte)syntax_ok;

      if (syntax_ok)
      {
           switch (IEI)
           {

              case IEI_FACILITY:
                 if(message_type != FACILITY)
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


                   (void)memscpy( mncc_facility_ind.data,
                                 sizeof(mncc_facility_ind.data),
                                            p_IE,
                                            IE_length);
                   PUT_IMH_LEN( IE_length +
                                  sizeof(connection_id) +
                                  sizeof(sys_modem_as_id_e_type),
                                  &mncc_facility_ind.message_header );

                   CNM_send_message( (IMH_T *)&mncc_facility_ind,
                                     GS_QUEUE_MN_CM );
                 }
                 break;

              case IEI_PROGRESS_INDICATOR:
                 if(message_type != PROGRESS)
                 {
                   CNM_setup_mncc_progress_ind( &mncc_progress_ind,
                                              connection_id,
                                              (ie_progress_indicator_T *)((void *)p_IE) );

                   CNM_send_message( (IMH_T *)&mncc_progress_ind,
                                     GS_QUEUE_MN_CM );
                   if(message_type == CALL_PROCEEDING)
                   {
                     if ( (((ie_progress_indicator_T *)((void *)p_IE))->CodstdLocation & 0x60 ) == STANDARD_GSM )
                     {
                         /*
                           * get pointer to progress description,
                           */

                       p_progress_description = (byte *)(void *)(&((ie_progress_indicator_T *)((void *)p_IE))->ProgressDescription);

                       if (((*p_progress_description & 0x7f) == 1) ||
                           ((*p_progress_description & 0x7f) == 2) ||
                           ((*p_progress_description & 0x7f) == 64)) /* TS24.008, Section 5.2.1.3 */
                       {
                         CNM_stop_timer(TIMER_T310,
                                                    (unsigned char)FORM_TI_PD( connection_id, PD_CC )); 
                       }
                     }
                   }
                 }
                 break;

#ifdef FEATURE_UUS
              case IEI_USER_USER:
                 if(message_type != USER_INFORMATION)
                 {
                   CNM_setup_mncc_user_user_data_ind( &mncc_user_user_data_ind,
                                                     connection_id,
                                                    (ie_user_user_T *) ((void *)p_IE) );

                   CNM_send_message( (IMH_T *) &mncc_user_user_data_ind,
                                     GS_QUEUE_MN_CM );
                 }
                 break;
#endif

              case IEI_CONGESTION_CONTROL:
              case IEI_CAUSE:
                 {
#ifdef FEATURE_UUS
                   if(message_type == CONGESTION_CONTROL)
                   {
                     MNCC_CONGESTION_IND_T      mncc_congestion_ind;
  
                     memset(&mncc_congestion_ind, NULL, sizeof(MNCC_CONGESTION_IND_T));

                     CNM_setup_mncc_congestion_ind( &mncc_congestion_ind,
                                         connection_id,
                                         (ie_congestion_control_T *) (p_IE));

                     CNM_send_message( (IMH_T *)&mncc_congestion_ind,
                                         GS_QUEUE_MN_CM );
                   }
                   else
#endif
                   if(message_type == STATUS_MSG)
                   { /*
                       * check that there is something there
                       */

                     if (p_IE[1] > 0)
                     {

                        /*
                         * check that cause uses GSM coding standard
                         */

                        if (((p_IE[2]) & 0x60) == STANDARD_GSM )
                        {
                           p_cause_value = &(p_IE[2]);

                           count = p_IE[1];

                           /*
                            * skip any additional bytes
                            */

                           while (((*p_cause_value) & 0x80) == 0)
                           {
                              p_cause_value++;

                              if ((count--) == 0)
                              {
                                 ERR("Bad Cause IE in Status msg\n", 0, 0, 0);
                                 break;
                              }
                           }
                           p_cause_value++;

                           if (((*p_cause_value & 0x7f)  ==
                                 MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE ) ||
                               ((*p_cause_value & 0x7f)  ==
                                 MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE ) )
                           {
                              CNM_send_release_complete(
                                       connection_id,
                                       PD_CC,
                                       MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,
                                       (ie_facility_T *) 0 );
                           }

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
                           if ((*p_cause_value & 0x7f) == CONDITIONAL_IE_ERROR ) 
                           {
                              byte   cc_cause_ie[4];

                               if( (connection_id < UNKNOWN_CONNECTION_ID) && /* MO Call */
                                   (cc_transaction_information_p[connection_id] != NULL) &&   
                                   (cc_transaction_information_p[connection_id]->cc_state == CC_STATE_U1) )
                               {
                                  cc_cause_ie[0] = (byte)IEI_CAUSE;
                                  cc_cause_ie[1] = (byte)2;
                                  cc_cause_ie[2] = (byte)STANDARD_GSM | 0x80;
                                  cc_cause_ie[3] = (byte)*p_cause_value | 0x80;
                 
                                  /* End the call with releasing the MM connection */
                                  CNM_send_mn_rel_cnf(connection_id, CONDITIONAL_IE_ERROR, 4, cc_cause_ie);

                                  /* stop T303 */
                                  CNM_stop_timer(TIMER_T303, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));

                                  /* reset the CC state */
                                  CNM_set_state_to_U0(connection_id); 
      
                                  /* possible repeat */
                                  cc_transaction_information_p[connection_id]->repeat = TRUE;
                               }
                           }
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
                        }
                     }
                   }

                    
                 }
                 break; 

              default:                            /* this cannot happen! */
                 break;

           }     /* end case */

      }        /* ENDIF syntax is OK */
      else
      if ( cc_IE_info[IE_count].presence == OPTIONAL_IE )
      {
         syntax_ok = TRUE;
      }

      else
      {
         *error_cause = (cc_IE_info[IE_count].presence == CONDITIONAL_IE) ? CONDITIONAL_IE_ERROR : INVALID_MANDATORY_INFORMATION;
      }

      IE_count++;       /* index next IE in the message */

    }

    return syntax_ok;

}

/**************** End of   CNM_perform_check_on_IEs ********************************/
