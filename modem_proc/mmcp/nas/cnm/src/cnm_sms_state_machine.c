/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_sms_state_machine.c_v   1.13   19 Jul 2002 11:59:02   rgurumoo  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_sms_state_machine.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
20-Jun-2001  VT    Removed unwanted reference(s) to debug_str

7/5/01     CD      Added more logging messages

8/02/01    CD      Changed state transition log message from MSG_MED to MSG_HIGH

8/3/01      CD     Added call to CNM_send_error_indication_to_rp_layer in case for 
                   TC1M expiration
                   Removed unnecesary include queue.h

08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

09/14/01   CD      Modified call to MSG_HIGH to avoid using %s on target

09/17/01   CD      Added call to CNM_process_conn_accepted for trigger MMSMS_EST_CNF_event
                   received in state MM_CONN_PENDING

                   Added case MMSMS_REEST_CNF_event, MMSMS_ERROR_IND_event
                   for state SMS_MM_CONN_PENDING

                   Changed CNM_sms_repond_to_event to void

10/01/01   CD      Added case MMCNM_ERROR_IND_event for state SMS_MM_CONN_PENDING

04/19/02   CD      Replaced MMSMS_REEST_CNF_event by MMSMS_REEST_REJ_event
                   Added MMSMS_EST_REJ_event

                   Added sending a MMCNM_REL_REQ when processing the 
                   last active connection

                   Updated Copyright

01/29/02   RG      Fixes to handle multiple SMS transactions properly Removed the 
                   sending of CP-ERROR when a MMCNM_REL_IND is received in the 
                   SMS_MM_CONN_EST state.

05/07/02   CD      Changed release type enum due to name collision with RR

05/23/02   RG      In the CNM_sms_respond_to_event() routine, set 
                   mm_connection_pending.CNM_service_type to NO_CNM_SERVICE
                   before calling the CNM_check_for_pending_message() routine.
                   In the SMS_MM_CONN_EST state the processing of T_SMS_RELEASE_event 
                   and the C_ABORT_SMS_event is seperated such that the 
                   CNM_check_for_pending_message() is not called when processing 
                   the T_SMS_RELEASE_event.
                   In the SMS_WAIT_CP_ACK state the processing of S_CP_ACK_FALSE_event, 
                   S_CP_ERROR_event, and the SMS_TIMER_EXPIRY_event sets the 
                   release_connection to TRUE appropriately.

07/19/02   RG      In the CNM_sms_respond_to_event() routine, set the
                   release_connecton to true when the release_pending flag
                   is true and the SMS state transitions from SMS_MM_CONN_EST
                   state to the SMS_IDLE state. This will enable the call to 
                   check_for_last_connection which sends the MMCNM_REL_REQ to
                   MM. This will cause the MM to start 
                   timer 19 (10 seconds) to wait for the
                   CHANNEL_RELEASE message from the network. On expiry of timer 19
                   MM instructs L2 to initiate release by sending a DISC message.

08/01/02   CD      Removed processing of MMCNM_ERR_IND_event

02/14/03   AB      Removed Lint error.

03/13/03   RG      Added code to handle MO SMS during Handover.

07/02/03   RG      Fix for CR 32105

12/15/04   AB     Added supports for AT+CMMS=1, ATCOP multiple MO SMS.

09/28/05   HS      Check/send pending SMS when MO transaction ends

07/14/06   NR      Adding changes for SMS optimization CR 93099 

12/22/06   RD      Added changes to fix lint errors

16/02/06   NR      Adding new reject cause for handling CNM MO SMS timer expiry

04/09/07   RD      SMS optimization changes are incorporated

07/14/11   PM      Fix for CR 294952. NAS sends error report to WMS for thrashed SMS.
                   Also, when CNM receives MMCNM_REL_IND from MM, MMCNM_EST_REQ for next SMS
                   is not sent to MM. See other change in cnm_sms_process_msgs.c

09/27/11   PM      Fix for CR 308048. Sending CP-ERROR, when network sends CP-DATA
                   in UE state Wait_for_CP_ACK
 
10/10/11    HC    Removed AEEstd.h inclusion as aeestdlib is obsoleted 
                  Replaced the std_strlprintf with C equivalent function snprintf
                  Included <stdio.h>

11/09/11    PM    Sending CP-ERROR to NW when it sends CP-ACK in state SMS_MM_CONN_EST
                  for an MO SMS.

10/07/11   PM      Adding lower_layer_cause as new argument to function 
                   CNM_send_error_indication_to_rp_layer(). This will determine
                   the error_status to be sent to WMS by MN while handling
                   MNSMS_ERROR_IND. 
===========================================================================*/
#include "mmcp_variation.h"
#include <stdio.h>
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

#include "nasutils.h"
#include "cmlib.h"
#include "cnm.h"
#include "cm_sms.h"
#include "ms.h"


/**************** Begin of   CNM_sms_respond_to_event *****************************
 *
 *  Function name: CNM_sms_respond_to_event ()
 *  -----------------------------------------
 *
 *  Description:
 *  ------------
 *  This function determines if a response is required to a particular
 *  stimulus for the current state of the transaction
 *
 *  Parameters:
 *  -----------
 *  c_sms_event : IN - input message
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  respond_to_event
 *
 ******************************************************************************/

VOID_FUNC CNM_sms_respond_to_event( byte            trigger,
                                 sms_state_T     current_state,
                                 connection_id_T connection_id )
{
   byte        next_state = current_state;   /* start by assuming no change */
   timer_id_T  timer;
   event_T     event;
   char        state_transition[80];
   boolean     release_connection = FALSE;
   byte        release_type = CNM_NORMAL_RELEASE;
   switch (current_state)
   {
      case SMS_IDLE:
         switch (trigger)
         {
            case MNSMS_EST_REQ_event:              /* MO event */
               next_state = SMS_MM_CONN_PEND;
               sms_transaction_information[connection_id].pref_rl_domain
                 = MS_CC_MM;
               break;

            case S_MT_SMS_REQUEST_event:           /* MT event */
               CNM_send_cp_ack( connection_id );
               next_state = SMS_MM_CONN_EST;
               break;


            case S_CP_ACK_TRUE_event:
               CNM_send_cp_error( connection_id, CP_CAUSE_INVALID_TI );
               break;

            case T_SMS_RELEASE_event:
               /* if MO transaction ended, see if there's any pending SMS in
                * the queue. */
               if ( connection_id < 8 )
               {
                  CNM_check_for_pending_message();
               }
               break;

            default:
               break;
         }
         break;

      case SMS_MM_CONN_PEND:
         switch (trigger)
         {
            case MMSMS_EST_CNF_event:
               next_state = SMS_WAIT_CP_ACK;
               /* Start the MN timer just before sending the RP-DATA */
               MN_start_sms_timer(connection_id,TR1M);   /* SMS timer id */
               CNM_start_timer( TIMER_TC1M_MO, connection_id, PD_SMS
#if defined(FEATURE_DUAL_SIM )
                              , mn_as_id //we do not want to give this as data to timer module for this timer
#endif
                               );
               CNM_process_mm_conn_accepted();
               cp_data_l2_ack=FALSE;
               break;

            case MMSMS_REL_IND_event:

               /*
                 This has to be done in order to make sure
                 that the subsequent MM connection requests are
                 not queued in the service request queues. 
                 Check the CNM_manage_cnm_service_requests
                 routine where it queries 
                 the mm_connection_pending.CNM_service_type
                 to decide whether to queue the mm_connection_request
                 if there is one pending.
                */

               if(mm_connection_pending.CNM_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT)
               {
                 mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;
               }

               //CNM_check_for_pending_message();
               /* Commented above call because we don't want to put mmcnm_est_req
                * from here. We need to put mmcnm_est_req from cnm service queue only. 
                */
               sms_transaction_information[connection_id].release_pending
                 = FALSE;

               next_state = SMS_IDLE;
               break;

            case T_SMS_RELEASE_event:
            case C_ABORT_SMS_event:


              /* This has to be done in order to make sure
                 that the subsequent MM connection requests are
                 not queued in the service request queues. 
                 Check the CNM_manage_cnm_service_requests
                 routine where it queries 
                 the mm_connection_pending.CNM_service_type
                 to decide whether to queue the mm_connection_request
                 if there is one pending.
               */
               mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;

               CNM_check_for_pending_message();

               sms_transaction_information[connection_id].release_pending
                 = FALSE;

               release_connection = TRUE;
               release_type = CNM_ESTABLISHMENT_IN_PROGRESS;

               next_state = SMS_IDLE;
               break;


            case MNSMS_EST_REQ_event:
            case S_CP_DATA_event:
            case S_CP_ACK_TRUE_event:
            case S_CP_ACK_FALSE_event:
            case S_CP_ERROR_event:
               CNM_send_cp_error( connection_id,
                                 CP_CAUSE_MSG_NOT_COMPATIBLE_WITH_SMS_STATE );
               break;

            /*case MMCNM_ERR_IND_event:*/
            case MMSMS_REEST_REJ_event:
            case MMSMS_EST_REJ_event:
              /* This has to be done in order to make sure
                 that the subsequent MM connection requests are
                 not queued in the service request queues. 
                 Check the CNM_manage_cnm_service_requests
                 routine where it queries 
                 the mm_connection_pending.CNM_service_type
                 to decide whether to queue the mm_connection_request
                 if there is one pending.
              */
               mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;
               next_state = SMS_IDLE;
               break;

            
            default:
               break;

         }
         break;

      case SMS_MM_CONN_EST:
         switch (trigger)
         {
            case T_SMS_RELEASE_event:
               {
   
                  boolean sms_active = FALSE;
                  connection_id_T local_cid = 0;
   
                  sms_transaction_information[connection_id].release_pending
                    = FALSE;
   
                  release_connection = TRUE;
                  next_state = SMS_IDLE;
   
                  for(local_cid = 0;local_cid < MAXNO_SMS_TRANSACTIONS;local_cid++)
                  {
                    if ((local_cid != connection_id) &&
                         ( sms_transaction_information[ local_cid ].sms_state != SMS_IDLE ) &&
                         ( sms_transaction_information[ local_cid ].sms_state != SMS_MM_CONN_PEND ))
                    {
                      sms_active = TRUE;
                      break;
                    }
                  }
   
   
                  if ((connection_id < 8) && 
                       (sms_active == FALSE) &&
                       (CNM_check_cnm_service_request_queue(SHORT_MESSAGE_SERVICE_ESTABLISHMENT) == FALSE) &&
                       (mm_connection_pending.CNM_service_type != SHORT_MESSAGE_SERVICE_ESTABLISHMENT))
                  {
                    CNM_check_for_pending_message();
                  }
               }
               break;

            case C_ABORT_SMS_event:
               if ( connection_id < 8 )
               {
                  CNM_check_for_pending_message();
               }
               sms_transaction_information[connection_id].release_pending
                 = FALSE;

               release_connection = TRUE;
               next_state = SMS_IDLE;
               break;

            case MMSMS_REL_IND_event:
            /*case MMCNM_ERR_IND_event:*/
               if ( connection_id < 8 )
               {
                  CNM_stop_timer( TIMER_TC1M_MO,
                                 (unsigned char)FORM_TI_PD( connection_id, PD_SMS) );
                  //CNM_check_for_pending_message();
                 /* Commented above call because we don't want to put mmcnm_est_req
                  * from here. We need to put mmcnm_est_req from cnm service queue only. 
                  */
               }
               else
               {
                  CNM_stop_timer( TIMER_TC1M_MT,
                                 (unsigned char)FORM_TI_PD( connection_id, PD_SMS) );
                  // No need to send CP-ERROR when a MMCNM_REL_IND is received
                  // in the SMS_MM_CONN_EST state. Refer SDL in 3GPP TS24.011
                  // V3.5.0(2000-12) Page 56.
                  // CNM_send_cp_error( connection_id,
                  //                    CP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED );
               }
               sms_transaction_information[connection_id].release_pending = FALSE;
               next_state = SMS_IDLE;
               break;

            case S_CP_DATA_event:
               
               if ( connection_id < 8 )
               {
                  CNM_send_cp_user_data ( connection_id );
                  CNM_check_for_pending_message();
#ifdef FEATURE_NAS_LONG_SMS_BRANCH_A
                  if((mm_connection_pending.connection_id != connection_id) && 
                          (mm_connection_pending.CNM_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT) &&
                          (sms_transaction_information[connection_id].pref_rl_domain != MS_CC_MM_PS)) /*If establishment has been sent for a new SMS, then send CP_ACK for previous SMS*/
                  {
                    CNM_send_cp_ack( connection_id );
                  }
                  else
#endif
                  /* AT +CMMS=1: Deferred CP_ACK */
                  if( ( sms_link_control.mode != SMS_LINK_CONTROL_DISABLED ) && 
                      ( sms_link_control.cp_ack_deferred == FALSE ) &&
                      ( (CNM_mo_cp_user_data.data[0]&0x07) == RP_ACK_N ||
                        (CNM_mo_cp_user_data.data[0]&0x07) == RP_ERROR_N ) &&
                      ( sm_rl_transaction_information[connection_id].rl_state == WAIT_FOR_RP_ACK ) )
                  {
                      sms_link_control.connection_id = connection_id;
                      sms_link_control.cp_ack_deferred = TRUE;
                  }
                  else
                  {
                      CNM_send_cp_ack( connection_id );
                  }
               }
               else
               {
                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Unexpected event in this state for an MT SMS");
               }

               break;

            case S_RP_MESSAGE_REQ_event:
               if ( connection_id < 8 )
               {
                  CNM_start_timer( TIMER_TC1M_MO, connection_id, PD_SMS
#if defined(FEATURE_DUAL_SIM )
                                 , mn_as_id //we do not want to give this as data to timer module for this timer
#endif
                                 );
               }
               else
               {
                  CNM_start_timer( TIMER_TC1M_MT, connection_id, PD_SMS
#if defined(FEATURE_DUAL_SIM )
                                 , mn_as_id //we do not want to give this as data to timer module for this timer
#endif
                                 );
               }
               (void)CNM_send_cp_data(connection_id);
               next_state = SMS_WAIT_CP_ACK;
               break;

            case MMSMS_EST_CNF_event:
            case S_CP_ACK_FALSE_event:
            case S_CP_ACK_TRUE_event:
               CNM_send_cp_error( connection_id,
                                 CP_CAUSE_MSG_NOT_COMPATIBLE_WITH_SMS_STATE );
               break;

            case S_CP_ERROR_event: 
               if ( connection_id < 0x08 ) /*Added as part of CR 114966*/
               {
                  CNM_send_error_indication_to_rp_layer( connection_id, 0 ); 
                  sms_transaction_information[connection_id].release_pending = FALSE;
                  release_connection = TRUE;
                  next_state = SMS_IDLE;
               }
               else
               {
                  CNM_send_cp_error( connection_id,
                                 CP_CAUSE_MSG_NOT_COMPATIBLE_WITH_SMS_STATE );
               }
               
               break;

            default:
               break;

         }
         break;

      case SMS_WAIT_CP_ACK:
         switch (trigger)
         {
            case S_CP_ACK_FALSE_event:
            case S_CP_ERROR_event:
               CNM_send_error_indication_to_rp_layer( connection_id, 0 );
               sms_transaction_information[connection_id].release_pending
                 = FALSE;
               if ( connection_id < 8 )
               {
                  CNM_stop_timer( TIMER_TC1M_MO,
                                 (unsigned char)FORM_TI_PD( connection_id, PD_SMS) );
                  CNM_check_for_pending_message();
               }
               else
               {
                  CNM_stop_timer( TIMER_TC1M_MT,
                                 (unsigned char)FORM_TI_PD( connection_id, PD_SMS) );
                  if(trigger == S_CP_ERROR_event)
                  {
                     CNM_send_cp_error( connection_id,
                                    CP_CAUSE_MSG_NOT_COMPATIBLE_WITH_SMS_STATE );
                  }

#ifdef FEATURE_MODEM_HEAP
                  if(CNM_mt_cp_message[connection_id-8] != NULL)
                  {
                    modem_mem_free(CNM_mt_cp_message[connection_id-8], MODEM_MEM_CLIENT_NAS);
                    CNM_mt_cp_message[connection_id-8] = NULL;
                    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= FREE - CNM_mt_cp_message[%d]\n", (connection_id-8));
                  }
#endif
               }
               /* release_connection to be set to TRUE so that a check for
                  last connection is done */
               release_connection = TRUE;
               next_state = SMS_IDLE;
               break;

            case S_CP_ACK_TRUE_event:
               if ( connection_id < 8 )
               {
                  CNM_stop_timer( TIMER_TC1M_MO,
                                 (unsigned char)FORM_TI_PD( connection_id, PD_SMS) );
               }
               else
               {
                  CNM_stop_timer( TIMER_TC1M_MT,
                                 (unsigned char)FORM_TI_PD( connection_id, PD_SMS) );
#ifdef FEATURE_MODEM_HEAP
                  if(CNM_mt_cp_message[connection_id-8] != NULL)
                  {
                    modem_mem_free(CNM_mt_cp_message[connection_id-8], MODEM_MEM_CLIENT_NAS);
                    CNM_mt_cp_message[connection_id-8] = NULL;
                    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= FREE - CNM_mt_cp_message[%d]\n", (connection_id-8) );
                  }
#endif
                  CNM_send_cpack_indication_to_rp_layer(connection_id);
               }
               next_state = SMS_MM_CONN_EST;
               break;

            case CP_ACK_TRUE_NEW_CP_DATA_RXD_event:
               if ( connection_id < 8 )
               {
                  CNM_stop_timer( TIMER_TC1M_MO,
                                 (unsigned char)FORM_TI_PD( connection_id, PD_SMS) );
               }
               else
               {
                  CNM_stop_timer( TIMER_TC1M_MT,
                                 (unsigned char)FORM_TI_PD( connection_id, PD_SMS) );
#ifdef FEATURE_MODEM_HEAP
                  if(CNM_mt_cp_message[connection_id-8] != NULL)
                  {
                    modem_mem_free(CNM_mt_cp_message[connection_id-8], MODEM_MEM_CLIENT_NAS);
                    CNM_mt_cp_message[connection_id-8] = NULL;
                    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= FREE - CNM_mt_cp_message[%d]", (connection_id-8) );
                  }
#endif
                  CNM_send_cpack_indication_to_rp_layer(connection_id);
               }
               next_state = SMS_IDLE;
               sms_transaction_information[connection_id].release_pending = FALSE;
               break;

            case S_CP_DATA_event:
               /* Fix for lost CP_ACK during handover during MO SMS */
               if ( connection_id < 8 )
               {
                  /* forward up to MN only in the MO case */
                  CNM_send_cp_user_data ( connection_id );

                  CNM_stop_timer( TIMER_TC1M_MO,
                                 (unsigned char)FORM_TI_PD( connection_id, PD_SMS) );

                  CNM_mo_cp_data_retransmit_counter = 0;

                  CNM_check_for_pending_message();

                  next_state = SMS_MM_CONN_EST;
#ifdef FEATURE_NAS_LONG_SMS_BRANCH_A
                  if( (mm_connection_pending.connection_id != connection_id) && 
                      (mm_connection_pending.CNM_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT) &&
                      (sms_transaction_information[connection_id].pref_rl_domain != MS_CC_MM_PS)) /*If establishment has been sent for a new SMS, then send CP_ACK for previous SMS*/
                  {
                       CNM_send_cp_ack( connection_id );
                  }
                  else
#endif

                  /* do not send CP_ACK until TLC1M expires */
                  if( ( sms_link_control.mode != SMS_LINK_CONTROL_DISABLED ) && 
                      ( sms_link_control.cp_ack_deferred == FALSE ) &&
                      ( (CNM_mo_cp_user_data.data[0]&0x07) == RP_ACK_N ||
                        (CNM_mo_cp_user_data.data[0]&0x07) == RP_ERROR_N   ) &&
                      ( sm_rl_transaction_information[connection_id].rl_state == WAIT_FOR_RP_ACK ) )
                  {
                      sms_link_control.connection_id = connection_id;
                      sms_link_control.cp_ack_deferred = TRUE;
                  }
                  else
                  {
                      /* send CP_ACK if link control is not enabled */
                      CNM_send_cp_ack( connection_id );
                  }
               }
               
               break;

            case T_SMS_RELEASE_event:
               release_connection = TRUE;
               break;

            case MMSMS_REL_IND_event:
            case C_ABORT_SMS_event: 
            /*case MMCNM_ERR_IND_event:*/
               sms_transaction_information[connection_id].release_pending
                 = FALSE;
               if ( connection_id < 8 )
               {
                  CNM_stop_timer( TIMER_TC1M_MO,
                                 (unsigned char)FORM_TI_PD( connection_id, PD_SMS) );
                  if(trigger == C_ABORT_SMS_event)
                  {
                    CNM_check_for_pending_message();
                    CNM_send_error_indication_to_rp_layer( connection_id, 0);
                  }
               }
               else
               {
                  CNM_stop_timer( TIMER_TC1M_MT,
                                 (unsigned char)FORM_TI_PD( connection_id, PD_SMS) );
#ifdef FEATURE_MODEM_HEAP
                  if(CNM_mt_cp_message[connection_id-8] != NULL)
                  {
                    modem_mem_free(CNM_mt_cp_message[connection_id-8], MODEM_MEM_CLIENT_NAS);
                    CNM_mt_cp_message[connection_id-8] = NULL;
                    MSG_HIGH_DS_1(MN_SUB,"=MNCNM= FREE - CNM_mt_cp_message[%d]\n", (connection_id-8) );
                  }
#endif
                 if(trigger == MMSMS_REL_IND_event)
                 {
                   CNM_send_error_indication_to_rp_layer( connection_id, NO_RESPONSE_FROM_NEWTWORK_RLF);
                 }
                 else if(trigger == C_ABORT_SMS_event)
                 {
                   CNM_send_error_indication_to_rp_layer( connection_id, 0);
                 }
               }

               release_connection = TRUE;
               next_state = SMS_IDLE;

               break;

            case SMS_TIMER_EXPIRY_event:
               event = CNM_send_cp_data(connection_id);
               if ( connection_id < 8 )
               {
                  timer = TIMER_TC1M_MO;
               }
               else
               {
                  timer = TIMER_TC1M_MT;
               }

               if ( event == S_CP_ACK_FALSE_event )
               {
                  sms_transaction_information[connection_id].release_pending
                    = FALSE;

                  if(timer == TIMER_TC1M_MO)
                  {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                     mmcc_domain_name_T domain = (sms_transaction_information[connection_id].pref_rl_domain == MS_CC_MM)? MMCC_SMS_CS : MMCC_SMS_PS;
                     CNM_send_MNSMS_ERROR_IND( connection_id, 0, NO_RESPONSE_FROM_NEWTWORK,
                                                           0, domain);
#else
                     CNM_send_MNSMS_ERROR_IND( connection_id, 0, NO_RESPONSE_FROM_NEWTWORK);
#endif
                  }
                  else
                  {
                     CNM_send_error_indication_to_rp_layer( connection_id, NO_RESPONSE_FROM_NEWTWORK_TC1M_MT );
                  }


                  /* Release connection should be set to true here, so that
                  a check for last connection is done */
                  release_connection = TRUE;

                  next_state = SMS_IDLE;
               }
               else
               {
                  CNM_start_timer( timer, connection_id, PD_SMS
#if defined(FEATURE_DUAL_SIM )
                                 , mn_as_id //we do not want to give this as data to timer module for this timer
#endif
                                 );
                  cp_data_l2_ack=FALSE;
                  /*
                   * no state change
                   */

               }
               break;

            case MMSMS_EST_CNF_event:
               CNM_send_cp_error( connection_id,
                                 CP_CAUSE_MSG_NOT_COMPATIBLE_WITH_SMS_STATE );
               break;

            default:
               break;
         }
         break;


      default:

         ERR("SMS entity is in an unexpected state: current_state = %d\n",
              (dword) current_state, 0, 0);
         break;

   }

#ifdef FEATURE_NAS_LONG_SMS_BRANCH_A
   /* Now MS doesn't have to wait until the CM SERVICE ACCEPT is received for sending
    * the defferred CP-ACK, when the Radio Link Control is enabled. it should be sent immediately 
    * after the CM SERVICE REQ Message is transfered OTA in BRANCH-A Behaviour. The below blok
    * ensures that final CP-ACK of the MO SMS transaction is sent immediately after CM SERVICE REQ is 
    * sent OTA*/
   if ((next_state == SMS_MM_CONN_PEND) && (sms_link_control.cp_ack_deferred))
   {
     if(sms_transaction_information[sms_link_control.connection_id].pref_rl_domain != MS_CC_MM_PS)
     {
       CNM_send_cp_ack(sms_link_control.connection_id);
     }
     sms_link_control.cp_ack_deferred = FALSE;
   }  
#endif /*ifdef FEATURE_NAS_LONG_SMS_BRANCH_A */

   /*
    * do special processing for stored control flow release_pending,
    * which is only checked when we are about to enter state SMS_MM_CONN_EST
    */

   if (( next_state == SMS_MM_CONN_EST ) &&
       ( sms_transaction_information[connection_id].release_pending == TRUE ))
   {
      if ( connection_id < 8 )
      {
         CNM_check_for_pending_message();
      }

      /* Release connection should be set to true here, so that
         a check for last connection is done */
      release_connection = TRUE;

      sms_transaction_information[connection_id].release_pending
        = FALSE;

      next_state = SMS_IDLE;
   }

#ifdef PERLUTF
   #error code not present
#else
   (void)snprintf(state_transition,sizeof(state_transition),"%s-->%s",
       DEBUG_get_sms_state_name(sms_transaction_information[connection_id].sms_state),
       DEBUG_get_sms_state_name(next_state));
#endif

#ifdef FEATURE_CNM_UNIT_TEST
   MSG_HIGH_DS_3(MN_SUB,"=MNCNM= SMC-CS-%d %s Rel_pend = %d\n",
       connection_id, 
       state_transition,
       sms_transaction_information[connection_id].release_pending);
#else

   MSG_HIGH_DS_4(MN_SUB,"=MNCNM= SMC-CS-%d transtioned from state %d to %d Rel_pend = %d",
       connection_id, 
       sms_transaction_information[connection_id].sms_state,
       next_state,
       sms_transaction_information[connection_id].release_pending);
#endif

   sms_transaction_information[connection_id].sms_state = next_state;

   /* 
   *  Check to see if this is the last connection, and send a MMCNM_REL_REQ 
   */
   if (release_connection == TRUE)
   {
      /* AT +CMMS=1: Deferred MMCNM_REL_REQ, RR/RRC connection released */
      if( ( sms_link_control.cp_ack_deferred ) ||
        ( sms_link_control.mode && sms_link_control.status ) )
      {
          MSG_HIGH_DS_1(MN_SUB,"=MNCNM= AT +CMMS, released only the MM connection = %d", connection_id);  /* reset cp_ack_deferred */
      }
      else
      {
          (void)CNM_check_for_last_connection(release_type
#if defined(FEATURE_DUAL_SIM )
                                              , mn_as_id
#endif
                                             );  /* release RR connection */
      }
   }
}
/**************** End of   CNM_sms_respond_to_event *****************************/
