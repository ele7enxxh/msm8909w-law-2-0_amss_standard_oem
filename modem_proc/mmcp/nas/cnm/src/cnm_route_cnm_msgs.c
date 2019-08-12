/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_route_cnm_msgs.c_v   1.10   18 Jul 2002 12:35:40   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_route_cnm_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
20-Jun-2001  VT    Removed unwanted reference(s) to m20000_debug_string

08/06/01    CD     Replaced mn header files by single include of mn.h

8/8/01      CD     Replaced call to gs_log_message by CNM_log_message

8/9/01      CD     Added message direction when calling CNM_log_message

08/14/01   AB      Renamed connection manager references to CNM from CM.

9/13/01    CD      Modified CNM_log_message calling sequence

9/17/01     CD     Fixed routing of MMCNM_EST_CNF, MMCNM_DATA_IND, MMCC_REEST_CNF,
                   MMCNM_ABORT_IND, MMCNM_REL_IND, MMCNM_ERROR_IND

                   Deleted cm_sms.h and cause.h since they were not used

04/19/02    CD     In CNM_route_CNM_msgs(), changed processing of 
                   MMCNM_REL_IND, MMCNM_ERR_IND and MMCNM_ABORT_IND due to 
                   new interface between MM and CNM

                   In CNM_route_CNM_msgs() added case MMCC_REEST_REJ

                   Deleted processing of MMCNM_ABORT_IND.  It's now part of a 
                   EMCNM_REL_IND w/rel_cause set to ABORT_MSG_RECEIVED

                   Deleted processing of MMCNM_ERR_IND.  It's now part of a 
                   MMCNM_REL_IND w/rel_cause set to OTHER_CAUSE

                   Added cases for processing MMCNM_EST_REJ, MMCNM_REL_CNF

                   Removed calls to CNM_check_for_last_connection_released()

                   Updated copyright
                   Replaced ERR by MSG_ERROR

                   Removed lint errors

04/30/02     CD    Added check to see if MMCNM_REL_REQ has been sent 
                   before MMCNM_REL_CNF is processed

07/16/02     AB    In CNM_route_CNM_messages, MMCNM_REL_CNF primitive, 
                   added checking for any pending establish request that 
                   was waiting for MM release confirmation, MMCNM_REL_CNF, 
                   and service it.  

                   Also, for MS_TIMER primitives, added checking for last 
                   connection, check_for_last_connection_release(), to
                   ask MM release a RR/RRC connection because numerous
                   timer recovery  procedure would transistion a TI to CC_STATE_U0
                   state.
                   
08/02/02     CD    Moved calls to the SMS and SS handlers for processing a
                   MMCNM_REL_IND from cnm_process_mm_primitives.c

09/18/02     AB    Added routing for MMCNM_REL_REQ message.

02/10/03     AB    Added supported for the new prefer CS+PS radio link domain,
                   MS_CC_MM_PS, message set/group, for SMS transactions 
                   between CNM and MM.

04/16/03     AB    Send the end call indication that was deferred on RR 
                   connection released confirmation.

09/10/03     AB    Send RAB released indication/notification to CM/UI. 

08/10/04     AB    Fixed CR47640, pop any pending service request in 
                   MM connection establishment request queue.

09/21/04     AB    Added processing of MMCNM_REL_CNF primitives to MMCNM_REL_IND
                       primitives received from MM.

05/11/05     HS    Added support for FEATURE_CCBS

09/01/05     HS    Cut SMS TC1M timer short in G2G HO while waiting CP_ACK

11/08/05     AB    Added checking for TCH reuse on MMCNM_EST_REJ

07/14/06     NR    Adding changes for SMS optimization CR 93099

08/16/11     PM    Fix for CR 301197. Resetting mn_as_id when CNM gets MMCC_SIG_CONN_REL_IN
                   from MM.
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
#include "ms_cc_mm_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"

#include "nasutils.h"
#include "cmlib.h"
#include "cnm.h"
#include "cm_sms.h"
#include <stringl/stringl.h>
#define MM_AS_ID_1 1


//extern mmcc_sync_ind_T CNM_mmcc_sync_ind;
//extern byte cp_data_l2_ack;
//extern mmcnm_data_req_T CNM_mo_cp_message;
extern boolean is_cs_umts_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_gsm_mode( sys_modem_as_id_e_type as_id );
extern boolean is_cs_lte_mode(sys_modem_as_id_e_type as_id );

/*****************************************************************************
 *
 *  Function name: CMN_put_mn_rab_released_ind()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *     Send the MM connection released to MN to forward it to call manager, CM. 
 *
 *  Parameters:
 *  -----------
 *      None.
 *
 *  Returns:
 *  --------
 *      None.
 *
 ******************************************************************************/
VOID_FUNC CMN_put_mn_rab_released_ind(
#if defined(FEATURE_DUAL_SIM )
  sys_modem_as_id_e_type as_id
#else
  void
#endif
  )
{
#ifdef FEATURE_DUAL_SIM
   MNCC_RAB_REL_IND_T message;
   /* clear buffer */ 
   memset(&message, NULL, sizeof(MNCC_RAB_REL_IND_T)); 
   message.message_header.message_set = MS_MN_CC;
   message.message_header.message_id = (word)MNCC_RAB_REL_IND;
   if(MNCNM_IS_SINGLE_STACK)
   {
     message.as_id = as_id;
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif
   PUT_IMH_LEN(sizeof(MNCC_RAB_REL_IND_T) - sizeof(IMH_T), 
                        &message)
      /* Send the message */
   CNM_send_message((IMH_T *)&message, GS_QUEUE_MN_CM);
#else
   IMH_T message;  /* --> Scope of code optimization */
   /* clear buffer */ 
   memset(&message, NULL, sizeof(IMH_T)); 
   message.message_set = MS_MN_CC;
   message.message_id = (word)MNCC_RAB_REL_IND;
   PUT_IMH_LEN(NULL, &message)
   /* Send the message */
   CNM_send_message((IMH_T *)&message, GS_QUEUE_MN_CM); 
#endif
}

/*****************************************************************************
 *
 *  Function name: CMN_put_mn_cs_sig_conn_released_ind()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *     Send the RRC CS connection released ind to MN, so that if there was any very early assignment 
 *     of TCH prior to this, it will be released.
 *
 *  Parameters:
 *  -----------
 *      None.
 *
 *  Returns:
 *  --------
 *      None.
 *
 ******************************************************************************/
VOID_FUNC CMN_put_mn_cs_sig_conn_rel_ind(void)
{
   MNCC_CS_SIG_CONN_REL_IND_T message;

   MN_clear_call_data(7); /* clear call objects if any for conn id 7*/
  
   /* clear buffer */ 
   memset(&message, NULL, sizeof(IMH_T)); 

   /* build message */
   PUT_IMH_LEN(sizeof(MNCC_CS_SIG_CONN_REL_IND_T) - sizeof(sys_modem_as_id_e_type), &message.message_header)
   message.message_header.message_set = MS_MN_CC;
   message.message_header.message_id = (word)MNCC_CS_SIG_CONN_REL_IND;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     message.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif

   /* Send the message */
   CNM_send_message((IMH_T *)&message, GS_QUEUE_MN_CM);
}

/*****************************************************************************
 *
 *  Function name: CNM_active_cc_connection()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *      Check for any CC active connection available.
 * 
 *  Parameters:
 *  -----------
 *      None.
 *
 *  Returns:
 *  --------
 *      TRUE  : Active CC connection.
 *      FALSE : No active CC connection.
 *
 ******************************************************************************/
boolean CNM_has_active_cc_connection(void)
{
    byte transaction_id, active_connection;

    for (transaction_id = 0, active_connection = FALSE; 
                transaction_id < MAXNO_CC_TRANSACTIONS; transaction_id++)
    {
        if ((cc_transaction_information_p[transaction_id] != NULL)&&
            (cc_transaction_information_p[transaction_id]->cc_state != CC_STATE_U0))
        {
             active_connection = TRUE;
             break;
        }
    }

    return active_connection;
}

/*****************************************************************************
 *
 *  Function name: CNM_check_last_sync_ind()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *      Check for network reused the last TCH connection.
 * 
 *  Parameters:
 *  -----------
 *      CNM_input_primitive : IN - input message
 *
 *  Returns:
 *  --------
 *      None
 *
 ******************************************************************************/
static byte CNM_check_last_sync_ind(IMH_T *message)
{
    
    byte                     conn_released; 

    switch(message->message_id)
    {
        case MMCC_SYNC_IND:  /* copy sync indication */
          {
            memscpy(&CNM_mmcc_sync_ind, sizeof(CNM_mmcc_sync_ind) ,message, sizeof(mmcc_sync_ind_T));

            conn_released = FALSE;
          }
          break;

        case MMCNM_REL_IND:   /* nullify the TCH assignment */
          {
            CNM_mmcc_sync_ind.reason = (byte)WCDMA_RAB_RELEASED;

            conn_released = TRUE;
          } 
          break;

        case MMCNM_REL_CNF:       /* last MM connection released */
        case MMCNM_EST_REJ:       /* last MM connection establishment rejected */
          {

            if( ( ((message->message_id == MMCNM_REL_CNF) && 
                    (((mmcnm_rel_cnf_T*)message)->mm_conn_active_nw_data_recd == TRUE)) ||  /* active MM connection */
                   (message->message_id == MMCNM_EST_REJ) ) &&  
                (CNM_mmcc_sync_ind.reason != ((byte)WCDMA_RAB_RELEASED)) )  /* active RRC connection */
            {
                
				boolean is_G_RAT = FALSE;
#ifdef FEATURE_DUAL_SIM
				sys_modem_as_id_e_type local_as_id = SYS_MODEM_AS_ID_1;				
                if(message->message_id == MMCNM_REL_CNF)
                {
                  local_as_id = ((mmcnm_rel_cnf_T *)((void *)message))->as_id;
                }
                else if(message->message_id == MMCNM_EST_REJ)
                {
                  local_as_id = ((mmcnm_est_rej_T *)((void *)message))->as_id;
                }				
                if(MNCNM_IS_SINGLE_STACK)
                {
                   is_G_RAT = is_cs_gsm_mode(local_as_id);
                }
#ifdef FEATURE_DUAL_ACTIVE
                else
                {
                   is_G_RAT = is_cs_gsm_mode((sys_modem_as_id_e_type)mn_dsda_as_id);
                }
#endif
#else
                {
                   is_G_RAT = is_cs_gsm_mode(MM_AS_ID_1);
                }
#endif
                if(is_G_RAT == TRUE)
                {
                    CNM_mmcc_sync_ind.reason = (byte)GSM_RES_ASSIGNED;
                }
                else
                {
                    CNM_mmcc_sync_ind.reason = (byte)WCDMA_RAB_ESTABLISHED;
                }

#if defined(FEATURE_DUAL_SIM)
                if(MNCNM_IS_SINGLE_STACK)
                {
                  CNM_mmcc_sync_ind.as_id = local_as_id;
                }
#if defined(FEATURE_DUAL_ACTIVE)
                else
                {
                  CNM_mmcc_sync_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                }
#endif
#endif	

                CNM_send_message((IMH_T *)&CNM_mmcc_sync_ind, GS_QUEUE_MN_CM );   /* Restore the last safe sync */

                conn_released = FALSE;
            }
            else
            {
                CNM_mmcc_sync_ind.reason = (byte)WCDMA_RAB_RELEASED;

                conn_released = TRUE;
            }
          }
          break;

        default:
          {
            CNM_mmcc_sync_ind.reason = (byte)WCDMA_RAB_RELEASED;

            conn_released = TRUE;
          }
          break;
    }

    return conn_released;
}

/**************** Begin of   CNM_route_CNM_messages ********************************
 *
 *  Function name: CNM_route_CNM_messages ()
 *  --------------------------------------
 *
 *  Description:
 *  ------------
 *  This function routes all input primitives to handler
 *  routines. There is some additional processing to check
 *  some of the messages or parts of messages if this
 *  affects the destination
 *
 *  Parameters:
 *  -----------
 *  CNM_input_primitive : IN - input message
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_route_CNM_messages ( IMH_T * CNM_input_primitive )
{
   mmcnm_data_ind_T            *p_mmcnm_data_ind;
   int                         message_len;
   timer_expiry_T    *timer_data;

#if defined(FEATURE_DUAL_SIM )
  sys_modem_as_id_e_type     as_id = SYS_MODEM_AS_ID_NONE;
#endif
/* M-spec derived from Pspec 2.1.4
 * Function to route all input primitives to handler
 * routines. There is some additional processing to check
 * some of the messages or parts of messages if this
 * affects the destination
 */

   CNM_log_message(CNM_input_primitive);

   switch (CNM_input_primitive->message_set)
   {
      case MS_MN_CC:
         CNM_handle_call_control( CNM_input_primitive );
         break;

      case MS_MN_SS:
         CNM_handle_supplementary_services( CNM_input_primitive );
         break;

      case MS_MN_SMS:
         CNM_handle_short_message_services( CNM_input_primitive );
         break;

      case MS_CC_MM:
      case MS_CC_MM_PS:

/*
 * extract the message length information from the IMH header
 */

         GET_IMH_LEN( message_len,
                      CNM_input_primitive );

         switch (CNM_input_primitive->message_id)
         {
            case MMCNM_DATA_IND:

               if (message_len >= 2) /* message does not contain type IE */
               {
                  p_mmcnm_data_ind = (mmcnm_data_ind_T *) ((void *)CNM_input_primitive);

                  if ( GET_PD(p_mmcnm_data_ind->msg_head.ti_pd) == PD_CC)
                  {
                     CNM_handle_call_control(CNM_input_primitive);
                  }
                  else if ( GET_PD(p_mmcnm_data_ind->msg_head.ti_pd) == PD_SS)
                  {
                     CNM_handle_supplementary_services(CNM_input_primitive);
                  }
                  else if ( GET_PD(p_mmcnm_data_ind->msg_head.ti_pd) == PD_SMS)
                  {
                     CNM_handle_short_message_services(CNM_input_primitive);
                  }
                  else
                  {
                     /*
                      * this is for information only, as a bad PD in
                      * a message should cause the message to be
                      * simply ignored
                      */

                     MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid PD in msg - msg ignored: received PD = %d\n",
                           (dword) (GET_PD(p_mmcnm_data_ind->msg_head.ti_pd)));
                     break;   /* ignore check for last connection release */
                  }
               }
               else
               {
                  MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Received CNM msg is too small: received message_len = %d\n",
                         (dword) message_len);
               }
               break;

            case MMCC_REEST_CNF:
               CNM_handle_call_control(CNM_input_primitive);
               break;
            case MMCC_REEST_REJ:
               CNM_handle_call_control(CNM_input_primitive);

               /* Reset mn_as_id here */
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
               break;

            case MMCNM_REL_IND:
            case MMCNM_REL_CNF:
            if((CNM_input_primitive->message_id == MMCNM_REL_IND) &&
                (CNM_input_primitive->message_set  == MS_CC_MM_PS) &&
                (((mmcnm_rel_ind_T *)((void *)CNM_input_primitive))->rel_cause == PS_SMS_FAIL))
            {
                /*  When MM sends MMCNM_REL_IND in PS domain with rel_cause PS_SMS_FAIL
                              *  only PS MO SMS is terminated and no other variable or state corresponding to CS service is reset.
                              */
                CNM_handle_short_message_services(CNM_input_primitive);
            }
            else
            {
#if defined(FEATURE_DUAL_SIM )
                if(MNCNM_IS_SINGLE_STACK)
                {
                   if(CNM_input_primitive->message_id == MMCNM_REL_CNF)
                   {  
                     as_id = ((mmcnm_rel_cnf_T *)((void *)CNM_input_primitive))->as_id; /* needed for rab_rel to MN */
                     if( (CNM_check_if_any_service_object_exists() == FALSE) &&
                         (mn_as_id != SYS_MODEM_AS_ID_NONE) )
                     {
                       MSG_HIGH_DS_0(MN_SUB,"=MNCNM= =MN= Reset the mn_as_id here\n");
                       mn_as_id = SYS_MODEM_AS_ID_NONE;
                     }
                   }
                   else /*MMCNM_REL_IND*/
                   {
                     as_id = ((mmcnm_rel_ind_T *)((void *)CNM_input_primitive))->as_id;/* needed for rab_rel to MN */
                     if(CNM_input_primitive->message_set    == MS_CC_MM_PS)
                     {
                       if( (CNM_check_if_any_service_object_exists() == FALSE) &&
                           (mn_as_id != SYS_MODEM_AS_ID_NONE) )
                       {
                         MSG_HIGH_DS_0(MN_SUB,"=MNCNM= =MN= Reset the mn_as_id here\n");
                         mn_as_id = SYS_MODEM_AS_ID_NONE;
                       }
                     }
                     else/* msg_set== MS_CC_MM */
                     {
                       /* If there is an ongoing PS SMS - then do not set mn_as_id to   SYS_MODEM_AS_ID_NONE*/
                       connection_id_T connection_id;
                       boolean flag = FALSE;
                       for ( connection_id = 0;
                             connection_id < MAXNO_SMS_TRANSACTIONS;
                             connection_id++)
                       {
                         if ((sms_transaction_information[connection_id].sms_state != SMS_IDLE) && 
                             (sms_transaction_information[connection_id].pref_rl_domain == MS_CC_MM_PS))
                         {
                           flag = TRUE;
                           break;
                         }
                       }
                       if(flag == FALSE)
                       {
                         if( (CNM_check_if_any_service_object_exists() == FALSE) &&
                             (mn_as_id != SYS_MODEM_AS_ID_NONE) )
                         {
                           MSG_HIGH_DS_0(MN_SUB,"=MNCNM= =MN= Reset the mn_as_id here\n");
                           mn_as_id = SYS_MODEM_AS_ID_NONE;
                         }
                       }
                     }
                   }
                }
#endif

                MSG_HIGH_DS_1(MN_SUB,"=MNCNM= mm_rel_req_sent is %d", mm_rel_req_sent);
                if (mm_rel_req_sent != FALSE)
                {
                    mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;
                    mm_rel_req_sent = FALSE;
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Stopping CNM_MM_REL_TIMER");
                    CNM_stop_timer(CNM_MM_REL_TIMER, 0xff);
                }

                if(CNM_input_primitive->message_id == MMCNM_REL_IND)
                {

                  boolean est_req_sent = FALSE;
				
                  /* Note a MMCNM_REL_IND may trigger a call re-establishment procedure.
                                 * This procedure does not affect SMS or SS
                                 * entities.  The MMCNM_REL_IND is sent to the SMS and SS handlers 
                                 * for processing regardless of any subsequent re-establishment
                                 */
                  /* MMCNM_REL_IND with MS_CC_MM_PS comes only if there was a PS SMS 
                                    ongoing and CS Domain was not UP. */


                 /*If RRC connection releases , MM sends MMCNM_EST_REJ and MMCNM_REL_IND to CNM (when MM is in state # 20).
                  *When CNM receives MMCNM_EST_REJ, CNM will reject the corresponding transaction and sent MMCNM_EST_REQ if any service is pending.
                  *Now CNM will handle the MMCNM_REL_IND  and aborts all the transactions including the service for which it has sent MMCNM_EST_REQ.
                  *Now when CNM receives MMCNM_EST_CNF, CNM can not handle it as MN has already gone to IDLE state.
                  *CNM will send MMCNM_REL_REQ  to MM, when it receives MMCNM_REL_IND and if mm_connection_pending.CNM_service_type != NO_CNM_SERVICE  */

                  if(mm_connection_pending.CNM_service_type != NO_CNM_SERVICE)
                  {
                     est_req_sent = TRUE;
                  }

                  if(CNM_input_primitive->message_set  != MS_CC_MM_PS)
                  {
                    CNM_handle_call_control(CNM_input_primitive);
                  }

                  CNM_handle_short_message_services(CNM_input_primitive);

                  if(CNM_input_primitive->message_set  != MS_CC_MM_PS)
                  {
                    CNM_handle_supplementary_services(CNM_input_primitive);
                  }

                  if(est_req_sent == TRUE)
                  {

                    CNM_check_for_last_connection(CNM_ESTABLISHMENT_IN_PROGRESS
#if defined(FEATURE_DUAL_SIM )
                                                                     , mn_as_id
#endif
                                                  );
                  }
                }
                /* continue process as if CNM received MMCNM_REL_CNF */

                {

                  /* make sure pending EST is being service */
                  CNM_send_est_request_from_service_queue();

                  /* RAB Release Indication is not issued to upper layers, if RELEASE_IND is
                                    for PS Domain, which happens only when CS Domain is NOT UP and PS SMS was
                                    active. So the below check */

                  if(CNM_input_primitive->message_set  != MS_CC_MM_PS)
                  {
                    if(CNM_check_last_sync_ind(CNM_input_primitive))
                    {
                      CMN_put_mn_rab_released_ind(
#if defined(FEATURE_DUAL_SIM )
                             as_id
#endif
                                                 );
                    }

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
                    {
                      byte  i;

                      /* reset REPEAT indicator flag */
                      for (i=0; i < UNKNOWN_CONNECTION_ID; i++)
                      {
                        if(cc_transaction_information_p[i] != NULL)
                        {
                          cc_transaction_information_p[i]->repeat = FALSE; 
                        }
                      }

                      /* RAB reconfiguration guard timer */
                      CNM_stop_timer(TIMER_RAB_RELEASED, 0xff);
                    }
#endif /* FALLBACK */
                  }
                }
            }

               break;

            case MMCC_SYNC_IND:

               /* if HO, release SMS sessions */
               if ( ((((mmcc_sync_ind_T*)((void *)CNM_input_primitive))->reason == (byte)GSM_GSM_HANDOVER))  ||
                    ((((mmcc_sync_ind_T*)((void *)CNM_input_primitive))->reason == (byte)GSM_WCDMA_HANDOVER)) ||
                    ((((mmcc_sync_ind_T*)((void *)CNM_input_primitive))->reason == (byte)WCDMA_GSM_HANDOVER))||
                    ((((mmcc_sync_ind_T*)((void *)CNM_input_primitive))->reason == (byte)GSM_RES_ASSIGNED)&&
                    (((mmcc_sync_ind_T*)((void *)CNM_input_primitive))->channel_mode == SIGNALLING_ONLY)) ||
                    ((((mmcc_sync_ind_T*)((void *)CNM_input_primitive))->reason == (byte)LTE_TRANSPORT_ESTABLISHED))
                    )
               {

                   byte  ti;
                   IMH_T             *message;
                   boolean  is_g_RAT = FALSE;

                   timer_id_T  timer;
				   boolean is_lte_rat = FALSE;

                          
                   /* SMS active and in MM conn establish request */
                   if( (CNM_active_SMS( &ti ) == TRUE) && (ti<MAXNO_SMS_TRANSACTIONS) )
                   {
                       if(cp_data_l2_ack==FALSE)
                       {
#ifdef FEATURE_DUAL_SIM
                           if(MNCNM_IS_SINGLE_STACK)
                           {
                             is_g_RAT = is_cs_gsm_mode(mn_as_id);
                           }
#ifdef FEATURE_DUAL_ACTIVE
                           else
                           {
                             is_g_RAT = is_cs_gsm_mode((sys_modem_as_id_e_type)mn_dsda_as_id);
                           }
#endif
#else
                           {
                             is_g_RAT = is_cs_gsm_mode(MM_AS_ID_1);
                           }
#endif

#ifdef FEATURE_DUAL_SIM
                           if(MNCNM_IS_SINGLE_STACK)
                           {
                             is_lte_rat = is_cs_lte_mode(mn_as_id);
                           }
#ifdef FEATURE_DUAL_ACTIVE
                           else
                           {
                             is_lte_rat = is_cs_lte_mode((sys_modem_as_id_e_type)mn_dsda_as_id);
                           }
#endif
#else
                           {
                             is_lte_rat = is_cs_lte_mode(MM_AS_ID_1);
                           }
#endif

                           if((sms_transaction_information[ti].pref_rl_domain==MS_CC_MM && (is_g_RAT == TRUE)) ||
                              (sms_transaction_information[ti].pref_rl_domain==MS_CC_MM_PS && is_lte_rat == TRUE))
                           {                   
                              //MO SMS 
                             if ( ti < MT )
                             {
                               timer = TIMER_TC1M_MO;
                               message = (IMH_T *) &CNM_mo_cp_message;
                             }
                             //MT SMS 
                             else
                             {
                               timer = TIMER_TC1M_MT;
                               message = (IMH_T *) CNM_mt_cp_message[ti-MT];
                             }

                             if(message == NULL)
                             {
                               MSG_FATAL_MN_DS(MN_SUB,"=MNCNM= CNM_(mo/mt)_cp_message is NULL", (ti), 0, 0 );
                             }

                             CNM_stop_timer( timer,(unsigned char)FORM_TI_PD( ti, PD_SMS) );
                             CNM_start_timer( timer, ti, PD_SMS
#if defined(FEATURE_DUAL_SIM )
                                           , mn_as_id //we do not want to give this as data to timer module for this timer
#endif
                                            );

                             message->message_set = sms_transaction_information[ti].pref_rl_domain;
                             CNM_send_message( message, GS_QUEUE_MM );
                           }
                       }
                   }
               }
               /*
                * If channel_mode is SIGNALLING_ONLY, that means handover happened to G-RAT when 
                * only SMS was in progress.
                */       
               if (((mmcc_sync_ind_T*)((void *)CNM_input_primitive))->channel_mode != SIGNALLING_ONLY)
               {
                  (void)CNM_check_last_sync_ind(CNM_input_primitive);  /* save sync_ind */
                  CNM_handle_call_control(CNM_input_primitive);
               }

               break;

            case MMCNM_EST_CNF:
               {
                  boolean success = TRUE;
#if defined(FEATURE_DUAL_SIM )
                  if(MNCNM_IS_SINGLE_STACK)
                  {
                    if( (mn_as_id == ((mmcnm_est_cnf_T *) ((void *)CNM_input_primitive))->as_id)
                         && (mm_connection_pending.CNM_service_type != NO_CNM_SERVICE))
                    {
                        success = TRUE;
                    }
                    else
                    {
                        success = FALSE;
                    }
                  }
#endif
                  if(success == TRUE)
                  {
                     switch (mm_connection_pending.CNM_service_type)
                     {
                        case MO_CALL_ESTABLISHMENT:
                        case EMERGENCY_CALL_ESTABLISHMENT:
                        case MO_CALL_ESTABLISHMENT_FULL_SPEECH:
                        case MO_CALL_ESTABLISHMENT_HALF_SPEECH:
                        case MO_CALL_ESTABLISHMENT_DATA_9600:
                        case MO_CALL_ESTABLISHMENT_DATA_4800:
                        case MO_CALL_ESTABLISHMENT_DATA_2400:
      
                          CNM_handle_call_control(CNM_input_primitive);
                          break;
   
                        case SUPPLEMENTARY_SERVICE_ACTIVATION:
                
                          CNM_handle_supplementary_services(CNM_input_primitive);
                          break;
   
                        case SHORT_MESSAGE_SERVICE_ESTABLISHMENT:
    
                          CNM_handle_short_message_services(CNM_input_primitive);
                          break;
   
                        default:
                          MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected CNM Service type: received CNM_service_type = %d\n",
                                    (dword) mm_connection_pending.CNM_service_type);
                          break;
                     }
                  }
#if defined(FEATURE_DUAL_SIM )
                  else
                  {
                     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Unexpected MMCNM_EST_CNF, mn_as_id = %d, CNM_Service_Type = %d\n",
                            mn_as_id, (dword) mm_connection_pending.CNM_service_type);
   
                     CNM_check_for_last_connection(CNM_NORMAL_RELEASE,((mmcnm_est_cnf_T *) ((void *)CNM_input_primitive))->as_id);
                     mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;
                  }
#endif
               }
               break;

            case MMCNM_EST_REJ:
               {
                 switch (mm_connection_pending.CNM_service_type)
                 {
                    case MO_CALL_ESTABLISHMENT:
                    case EMERGENCY_CALL_ESTABLISHMENT:
                    case MO_CALL_ESTABLISHMENT_FULL_SPEECH:
                    case MO_CALL_ESTABLISHMENT_HALF_SPEECH:
                    case MO_CALL_ESTABLISHMENT_DATA_9600:
                    case MO_CALL_ESTABLISHMENT_DATA_4800:
                    case MO_CALL_ESTABLISHMENT_DATA_2400:
   
                      CNM_handle_call_control(CNM_input_primitive);
                      break;

                    case SUPPLEMENTARY_SERVICE_ACTIVATION:
               
                      CNM_handle_supplementary_services(CNM_input_primitive);
                      break;

                    case SHORT_MESSAGE_SERVICE_ESTABLISHMENT:

                      CNM_handle_short_message_services(CNM_input_primitive);
                      break;

                    default:
                      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected CNM Service type: received CNM_service_type = %d\n",
                                (dword) mm_connection_pending.CNM_service_type);
                      break;
                 }

                 /* make sure pending EST in queue is being service */
                 CNM_send_est_request_from_service_queue();

                 (void)CNM_check_last_sync_ind(CNM_input_primitive);  /* restored any existing sync_ind */

               }
               break;

#ifdef FEATURE_CCBS
            case MMCNM_PROMPT_IND:
               {
                  mmcnm_prompt_ind_T  *prompt_ind=(mmcnm_prompt_ind_T*)((void *)CNM_input_primitive);
                  
                  if (GET_PD(prompt_ind->pd_sapi) == PD_CC)
                  {
#if defined(FEATURE_DUAL_SIM )
                     if(MNCNM_IS_SINGLE_STACK)
                     {
                       mn_as_id =  prompt_ind->as_id;
                     }
#endif      
                     CNM_handle_call_control(CNM_input_primitive);
                  }
                  else /* no NIMO services for SS, SMS yet */
                  {
                     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= CM PROMPT not supported for %d yet",prompt_ind->pd_sapi);
                     CNM_send_mmcnm_prompt_res( FALSE, 
                                                FALSE
#if defined(FEATURE_DUAL_SIM )
                                                ,((mmcnm_prompt_ind_T*)CNM_input_primitive)->as_id
#endif
                                                 );
                  }
               }
               break;
#endif /* FEATURE_CCBS */
            /* if the SYNC IND is received after MM has sent CNM the REL IND, it is forwarded to CNM, which is stored by CNM assuming that 
                a very early TCH assignment, which is expected to happen before any CC message reception. But if RRC signalling connection
                is released after the reception of the SYNC IND by UE. Then CNM should forget the SYNC IND reception or TCH assignment */
            case MMCC_SIG_CONN_REL_IND:
               {
                  boolean success = TRUE;
#if defined(FEATURE_DUAL_SIM )
                  if(MNCNM_IS_SINGLE_STACK)
                  {
                    mmcc_sig_conn_rel_ind_T  *sig_conn_rel_ind_p =(mmcc_sig_conn_rel_ind_T*)CNM_input_primitive;
                    if ((mn_as_id != SYS_MODEM_AS_ID_NONE) && (sig_conn_rel_ind_p->as_id != mn_as_id))
                    {
                      MSG_HIGH_DS_1(MN_SUB,"=MNCNM= =CNM=  received ASID %d not the currently active one",(sig_conn_rel_ind_p->as_id + 1));
                      success = FALSE;
                    }
                  }
#endif
                  if(success == TRUE)
                  {
                    if(CNM_input_primitive->message_set  != MS_CC_MM_PS)
                    {
                      if(CNM_check_last_sync_ind(CNM_input_primitive)) /* forget the TCH Assignment, if it has been stored */
                      {
                        CMN_put_mn_cs_sig_conn_rel_ind();   /* signal MN to release the OTA Channel */
                      }  
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
               break;

                 /* The Below Message is sent by MM to indicate that a PS VoIP Call is being handed over to UMTS (WCDMA)  */
                 /* Call objects of CNM and MN should be created for conn_id 0x08 (0x0 from NW side) & Call state should be made Active.  */
                 /* All the Call object variables which can be derived by knowing that the call type is Voice should be updated  */
                 /* This Message is sent by MM to indicate that a PS VoIP Call is being handed over to UMTS (WCDMA)  */
            case MMCNM_SRVCC_HANDOVER_COMPLETE_IND:
               {
                  CNM_process_SRVCC_HO_complete_indication(CNM_input_primitive);
               }
               break;


            default:
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected msg received from MM: received message_id = %d\n",
                        (dword) CNM_input_primitive->message_id);
               break;
         }

         break;

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
      case MS_TIMER:

         if (CNM_input_primitive->message_id == MID_TIMER_EXPIRY)
         {
            CNM_recover_on_timer_expiry(
                           (timer_expiry_T *) ((void *)(CNM_input_primitive)));

            timer_data = (timer_expiry_T *) ((void *)(CNM_input_primitive));

            if(timer_data->timer_id != CNM_MM_REL_TIMER)
            {
              (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE 
#if defined(FEATURE_DUAL_SIM )
                                                 , mn_as_id
#endif
                                                                           );
            }
            else
            {
              (void)CNM_check_for_last_connection(CNM_NORMAL_RELEASE 
#if defined(FEATURE_DUAL_SIM )
                                                 , timer_data->data
#endif
                                                                           );
            }
         }
         else
         {
            MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected Timer Message: received message_id = %d\n",
                   (dword) CNM_input_primitive->message_id);
         }
         break;
#endif /*(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))*/

      default:

         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected Message Set: received message_set = %d\n",
                 (dword) CNM_input_primitive->message_set);

         break;
   }

}
/**************** End of   CNM_route_CNM_messages **********************************/
