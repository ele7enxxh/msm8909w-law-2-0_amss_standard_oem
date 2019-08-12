/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_timers.c_v   1.11   18 Jul 2002 12:37:30   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_timers.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
13-Jun-2001  VT    In CM_recover_on_timer_expiry(), cleaned up the logging 
                     on timer expiry
20-Jun-2001  VT    Removed unwanted reference(s) to m20000_debug_string & 
                      debug_str

08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

09/14/01   CD      Modified calls to MSG_HIGH, MSG_MED to avoid using %s on target

3-Sep-2001   VT    In CNM_get_timeout(), corrected the values of TIMER_DTMF_ON 
                     & TIMER_DTMF_OFF from 50millisec to 10sec (as per TS 24.008)

26-Oct-2001  VT    DTMF: Removed TIMER_DTMF_ON/OFF & added TIMER_T336/T337 in:
                     CNM_get_timeout(), CNM_recover_on_timer_expiry()

9-Jan-2002   VT    Removed unwanted \n in the beginning of MSG_HIGH (2 places)

9-Jan-2002   VT    Linted this file & removed most of the errors.

04/19/02     CD    Added processing for CNM_MM_REL_TIMER

                   Updated copyright

                   Replaced ERR by MSG_ERROR

7/1/02       CD    Corrected ignoring return value of CNM_write_cause_IE()

                   Corrected 'unusual pointer cast' lint errors

7/16/02      AB    Added error handling for internal timer, CNM_MM_REL_TIMER,
                   expiry.

7/24/02      RG    Changed the TIMER_TC1M_MO and TIMER_TC1M_MT timeout values to
                   10 seconds (previously 15 seconds) in the CNM_get_timeout() routine.

12/3/02      RG    Changed the TIMER_TC1M_MO and TIMER_TC1M_MT timeout values back to
                   15 seconds in the CNM_get_timeout() routine.

01/28/02     AB    Aborted a the establishment request if T303 expired while the 
                   transaction is still in CC_STATE_U0 or CC_STATE_U0_1 state, per spec TS24.008, 
                   Section 5.4.1.

02/14/03     AB    Removed Lint errors.

04/16/03     AB    Updated to new connection reject interface function all 
                   CNM_setup_MNCC_REJ_IND().

04/12/05     AB    The CNM_MM_REL_TIMER, 36, should only expired once.

08/08/06     NR    Changing the TIMER_TC1M_MO & TIMER_TC1M_MT to 20 sec 
                   to be compliant with spec for TR1M & TR2M timer values
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"
#include "gs_v.h"
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
#include "cm_sms.h"
#include <stringl/stringl.h>
#include "nasutils.h"



byte CNM_allocate_timer( byte , byte );

/*
 * global data declaration
 */

/**************** Begin of   CNM_cc_start_timer ***********************************
 *
 *  Function name: CNM_cc_start_timer()
 *  --------------------------------
 *
 *  Description:
 *  ------------
 *  This function uses a generic function call to start a timer. Note, T310
 *  is only started if the progress indicator associated with the transaction
 *  is not 1 or 2
 *
 *  Parameters:
 *  -----------
 *  timer_id : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC     CNM_cc_start_timer( timer_id_T timer_id,
                                 connection_id_T connection_id )
{
   CNM_start_timer( timer_id, connection_id, PD_CC
#if defined(FEATURE_DUAL_SIM )
                  , mn_as_id //we do not want to give this as data to timer module for this timer
#endif
                  );
}
/**************** End of   CNM_cc_start_timer *************************************/


/**************** Begin of   CNM_start_timer **************************************
 *
 *  Function name: CNM_start_timer()
 *  --------------------------------
 *
 *  Description:
 *  ------------
 *  This function uses a generic function call to start a timer. Note, T310
 *  is only started if the progress indicator associated with the transaction
 *  is not 1 or 2
 *
 *  Parameters:
 *  -----------
 *  timer_id : IN
 *  connection_id : IN
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/


VOID_FUNC     CNM_start_timer( timer_id_T timer_id,
                              connection_id_T connection_id,
                              byte protocol_descriminator
#if defined(FEATURE_DUAL_SIM )
                            , sys_modem_as_id_e_type as_id
#endif
                             )
{
   boolean     timer_required;
   byte        ti_pd = 0, data = 0xff;
   byte        timer_number;
   rex_timer_cb_type func_ptr = (rex_timer_cb_type)mn_cnm_timer_expiry;
   gs_queue_id_T queue_id = GS_QUEUE_MN_CM;

   timer_required = FALSE;

   if ( timer_id == TIMER_T310 )
   {
      if ( (cc_progress_information[connection_id] != 1 ) &&
           (cc_progress_information[connection_id] != 2 ) &&
           (cc_progress_information[connection_id] != 64) )
      {
         timer_required = TRUE;
      }
   }
   else
   {
      timer_required = TRUE;
   }

   if ( timer_required != FALSE )
   {
      if(timer_id != CNM_MM_REL_TIMER)
      {
        ti_pd = (unsigned char)(FORM_TI_PD(connection_id, protocol_descriminator));
        data = ti_pd;
      }
      else
      {
        /*
         * If timer_id = CNM_MM_REL_TIMER, it means parameter passed at protocol_discriminator
         * is as_id for which timer is being started.
         */
#if defined(FEATURE_DUAL_SIM )
        if(MNCNM_IS_SINGLE_STACK)
        {
          if(as_id >= 0)
          {
            data = as_id; 
          }
        }
#endif
      }
   /*
    * DEBUG CODE
    */
#ifdef FEATURE_CNM_UNIT_TEST
      MSG_MED_DS_3(MN_SUB,"=MNCNM= Start %s, conn %d PD %d\n",
                   DEBUG_get_timer_name(timer_id),
                   connection_id,
                   protocol_descriminator );
#else
      MSG_MED_DS_3(MN_SUB,"=MNCNM= Start timer %d, conn %d PD %d\n",
                   timer_id,
                   connection_id,
                   protocol_descriminator );
#endif

      timer_number = CNM_allocate_timer( timer_id, ti_pd );

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
     {
       if((!MNCNM_IS_SINGLE_STACK) && (mn_dsda_as_id == (mn_dsda_as_id_e_type)SYS_MODEM_AS_ID_2)) /* set the timer only if the device mode is DSDA */
       {
         func_ptr = mn_cnm_timer_expiry_subs_2;
         queue_id = GS_QUEUE_MN_CM_1;
       }
     }
#endif

      if ( timer_number == 0xff )
      {
         ERR("No more CNM timers, given timer_id = %d\n",
                        (dword) timer_id, 0, 0);
      }
      else
      {
        if (gs_start_timer( queue_id,
                             timer_number,
                             func_ptr,
                             CNM_get_timeout( timer_id ),
                             data ) != GS_SUCCESS)

        {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= gs_start timer failed\n" );
        }
      }
   }
}

/**************** End of   CNM_start_timer ****************************************/


/**************** Begin of   CNM_get_timeout **************************************
 *
 *  Function name: CNM_get_timeout()
 *  -------------------------------
 *
 *  Description:
 *  ------------
 *  This function returns the timeout value for the specified timer
 *
 *  Parameters:
 *  -----------
 *  timer_id : IN
 *
 *  Returns:
 *  --------
 *  timeout value
 *
 ******************************************************************************/

dword CNM_get_timeout( timer_id_T timer_id )
{
   dword       return_value;

   switch (timer_id)
   {

      case TIMER_T303:
         return_value = 30000;
         break;

      case TIMER_T305:
         return_value = 30000;
         break;

      case TIMER_T308:
         return_value = 30000;
         break;

      case TIMER_T310:
         return_value = 30000;
         break;

      case TIMER_T313:
         return_value = 30000;
         break;

      case TIMER_T323:
         return_value = 30000;
         break;

      case TIMER_T324:
         return_value = 15000;
         break;

      case TIMER_T336:  
         return_value = 10000;
         break;

      case TIMER_T337:   
         return_value = 10000;
         break;

     /* Fix for GCF 11.10 FAILURE: CR 23461 */
      /* 
        Changing the timeout value to 15 seconds and number
        of retries to 1(instead of 2). This would  beat
        the 40 second TR1M timeout. This has to be done  for
        a MO SMS message of size 160 characters since L2 takes little
        over 10 seconds to transmit the complete 
        message out to the network; because of which the CP_DATA
        is retransmitted and the GCF test fails (not expecting
        the retransmitted CP_DATA). 
        One of reasons behind having 2 retries was to make sure that
        if one of the CP_DATA's is lost during a handover or a bad
        signal, the retries would come to rescue. But L2 sends the
        SMS messages in ack mode and has multiple retransmissions to
        take care of dropped packets.
        For the SMS peers 1 retransmission should be sufficient considering:
        1) The L2 is operating in ack mode and will retransmit messages in case they are lost.
        2) The chances of the SMS peer in the network not acknowledging a received CP_DATA
           is low.
       */

       /* Set Maxium value per GCF < 25s waiting for CP_ACK for a long message in busy network */

      case TIMER_TC1M_MO:
         return_value = 20000;
         break;

      case TIMER_TC1M_MT:
         return_value = 20000;
         break;

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      case TIMER_RAB_RELEASED:
         return_value = 10000;
         break;
#endif

      case CNM_MM_REL_TIMER:
         return_value = 12000;
         break;

      default:
         ERR("Unknown timer id, setting timeout to 0: given timer_id = %d\n",
              (dword) timer_id, 0, 0);

         return_value = 0;
         break;

   }

   return return_value;
}
/**************** End of   CNM_get_timeout ****************************************/


/**************** Begin of   CNM_allocate_timer ***********************************
 *
 *  Function name: CNM_allocate_timer()
 *  ----------------------------------
 *
 *  Description:
 *  ------------
 *  This function returns the timer number of the allocated timer
 *
 *  Parameters:
 *  -----------
 *  timer_id : IN
 *  ti_pd : IN
 *
 *  Returns:
 *  --------
 *  timer number
 *
 ******************************************************************************/

byte CNM_allocate_timer( byte timer_id, byte ti_pd )
{

   byte pointer;
   byte return_value;

   return_value = 0xff;

   /*
    * first need to see if there is a valid entry corresponding to the
    * requested timer. If so, we need to use this again rather than
    * allocate a new timer
    */

   for ( pointer = 1; pointer < MAX_CNM_TIMER; pointer++)
   {
      if ( ( timer_store[pointer].timer_id == timer_id ) &&
           ( timer_store[pointer].ti_pd == ti_pd ))
      {
         /*
          * found a match
          */

         return_value = pointer;
         break;
      }
   }

   /*
    * if not yet found, allocate a new one
    */

   if ( return_value == 0xff )
   {
      pointer = timer_store_pointer;

      do
      {
         if ( timer_store[pointer].timer_id == 0xff )  /* indicates not used */
         {
            timer_store[pointer].timer_id = timer_id;
            timer_store[pointer].ti_pd = ti_pd;
            return_value = pointer;
            timer_store_pointer = pointer;
            break;
         }

         pointer = (unsigned char)((pointer + 1) % MAX_CNM_TIMER);

      } while (pointer != timer_store_pointer);      /* loop once only */
   }

   return return_value;

}
/**************** End of   CNM_allocate_timer *************************************/


/**************** Begin of   CNM_stop_timer ***************************************
 *
 *  Function name: CNM_stop_timer()
 *  ------------------------------
 *  Description:
 *  ------------
 *  This function uses a generic function call to stop a selected timer. If
 *  the timer id is ALL_CC_TIMERS, then all the CC timers are stopped
 *  individually
 *
 *  Parameters:
 *  -----------
 *  timer_id : IN - id of timer to be stopped, or ALL_CC_TIMERS
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_stop_timer( timer_id_T timer_id, byte ti_pd )
{
   timer_id_T  timer_number;
   gs_queue_id_T queue_id = GS_QUEUE_MN_CM;

   /*
    * DEBUG CODE
    */



#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   if(mn_dsda_as_id == (mn_dsda_as_id_e_type)SYS_MODEM_AS_ID_2)
   {
     queue_id = GS_QUEUE_MN_CM_1;
   }
#endif


   for ( timer_number = 0;
         timer_number < MAX_CNM_TIMER;
         timer_number++ )
   {

      /*
       * if entry matches timer id, or timer id is ALL_CC_TIMERS and the entry's
       * timer id is one of the CC ones (as opposed to SMS or SS), AND the
       * connection id matches, then stop the timer
       */

      if ((( timer_store[timer_number].timer_id == timer_id ) ||
           (( timer_id == ALL_CC_TIMERS ) &&
                     (timer_store[timer_number].timer_id <= MAX_CC_TIMER ))) &&
          ((timer_id == CNM_MM_REL_TIMER) || ( timer_store[timer_number].ti_pd == ti_pd )) )

      {

#ifdef FEATURE_CNM_UNIT_TEST
         MSG_MED_DS_1(MN_SUB,"=MNCNM= Stop %s", DEBUG_get_timer_name(timer_id));
#else
         MSG_MED_DS_1(MN_SUB,"=MNCNM= Stop timer %d",timer_id);
#endif

         timer_store[timer_number].timer_id = 0xff;   /* mark unused */

         if ( gs_cancel_timer(queue_id, timer_number) != GS_SUCCESS)
         {
            MSG_MED_DS_0(MN_SUB,"=MNCNM= Unable to stop timer\n" );
         }
      }
   }
}
/**************** End of   CNM_stop_timer ****************************************/


/**************** Begin of   CNM_recover_on_timer_expiry ***************************
 *
 *  Function name: CNM_recover_on_timer_expiry()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to process timer expiry messages. The action to perform
 *  depends on the timer id concerned.
 *
 *  Parameters:
 *  -----------
 *  timer_expiry_message : INPUT - pointer to message
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_recover_on_timer_expiry( timer_expiry_T * timer_expiry_message )
{
    byte                    action;
    transaction_id_T        transaction_id;
    event_T                 event;
    byte                    ti_pd;   /* protocol descriminator/transaction_id */
    cc_state_T              cc_state = CC_STATE_U0;
    byte                    cause[GSM_CAUSE_SIZE];
    byte                    *p_cause;         /* pointer to a cause ie */
    byte                    *last_octet_cause; /* pointer to last octet of cause ie */
    unsigned int            msg_len;
    byte                    *write_ptr;
    byte                    *last_octet_write_ptr;
    MNCC_REJ_IND_T          *p_mncc_rej_ind;
    MNCC_REL_IND_T          *p_mncc_rel_ind;
    release_msg_T           *p_release;
    disconnect_msg_T        *p_disconnect;
    timer_id_T              timer_id;
    unsigned int            cause_length = 0;
    int                     min_size;
    
    p_mncc_rej_ind = (MNCC_REJ_IND_T*) CNM_alloc( sizeof( MNCC_REJ_IND_T ));
    p_mncc_rel_ind = (MNCC_REL_IND_T*) CNM_alloc( sizeof( MNCC_REL_IND_T ));
    p_release = (release_msg_T*) CNM_alloc( sizeof( release_msg_T ));
    p_disconnect = (disconnect_msg_T*) CNM_alloc( sizeof( disconnect_msg_T ));
    
    /*
    * Identidfy timer event
    */
    
    GS_SET_UNDEFINED(event);           /* default to UNDEFINED event */
    
                                       /*
                                       * convert allocated timer id which is in the return message to a real
                                       * timer id obtained from store
    */
    
    timer_id = timer_store[timer_expiry_message->timer_id].timer_id;
    
#ifdef FEATURE_CNM_UNIT_TEST
    MSG_HIGH_DS_3(MN_SUB,"=MNCNM= CNM timer %s expired, TI %d PD %d", DEBUG_get_timer_name(timer_id),
             (unsigned char)GET_TI(timer_expiry_message->data),
             (unsigned char)GET_PD(timer_expiry_message->data));
#else
    MSG_HIGH_DS_3(MN_SUB,"=MNCNM= CNM timer %d expired, TI %d PD %d", timer_id,
             (unsigned char)GET_TI(timer_expiry_message->data),
             (unsigned char)GET_PD(timer_expiry_message->data));

#endif
    
    /*
    * Mark timer as free for another allocation
    */
    
    timer_store[timer_expiry_message->timer_id].timer_id = 0xff;
    
    /*
    * replace timer_id in message
    */
    
    timer_expiry_message->timer_id = timer_id;
    
    switch (timer_expiry_message->timer_id)
    {
    case TIMER_T303:
        event = T303_expiry_event;
        break;
        
    case TIMER_T305:
        event = T305_expiry_event;
        break;
        
    case TIMER_T308:
        event = T308_expiry_event;
        break;
        
    case TIMER_T310:
        event = T310_expiry_event;
        break;
        
    case TIMER_T313:
        event = T313_expiry_event;
        break;
        
    case TIMER_T323:
        event = T323_expiry_event;
        break;
        
    case TIMER_T324:
        event = T324_expiry_event;
        break;
        
    case TIMER_T336:
        event = DTMF_T336_expiry_event;
        break;
        
    case TIMER_T337:
        event = DTMF_T337_expiry_event;
        break;
        
    case TIMER_TC1M_MO:
    case TIMER_TC1M_MT:
        event = SMS_TIMER_EXPIRY_event;
        break;

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
    case TIMER_RAB_RELEASED:
        {
            /* not RAB Re-Configuration; Force clearing of all active call */

            mmcnm_rel_ind_T  mmcnm_rel_ind;
                
            mmcnm_rel_ind.header.message_set = MS_CC_MM;
            mmcnm_rel_ind.header.message_id  = MMCNM_REL_IND;
            mmcnm_rel_ind.reest_possible = FALSE;
            mmcnm_rel_ind.rel_cause = OTHER_CAUSE;
#if defined(FEATURE_DUAL_SIM) 
            if(MNCNM_IS_SINGLE_STACK)
            {
              mmcnm_rel_ind.as_id = mn_as_id;
            }
#ifdef FEATURE_DUAL_ACTIVE
            else
			{
              mmcnm_rel_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
            }
#endif
#endif

            PUT_IMH_LEN(sizeof(mmcnm_rel_ind_T) - sizeof(IMH_T), &mmcnm_rel_ind.header);

            CNM_send_message( (IMH_T *)&mmcnm_rel_ind , GS_QUEUE_MN_CM );   /* Restore the last safe sync */

            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= RAB re-Configuration failed for in-Call Modification");

            CNM_stop_timer(TIMER_RAB_RELEASED, 0xff);  /* expired only onced */
        }
        break;
#endif

    case CNM_MM_REL_TIMER:
        {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= MMCNM_REL_REQ Timeout...");

            /* 
             * Reset sync copy indication 
             */
            CNM_mmcc_sync_ind.reason = ((byte)WCDMA_RAB_RELEASED);

            /* 
             * No need to set an event and go into state machine processing, since this timer
             * is not associated with a TI_PD. Just reset the mm_rel_req_sent flag.
             */
            mm_rel_req_sent = FALSE;

            /* 
             * Abort any pending ESTABLISHMENT request 
             */
            if(mm_connection_pending.CNM_service_type != NO_CNM_SERVICE)
            {
                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Abort pending MM connection request id=%d", mm_connection_pending.connection_id);

                mm_connection_pending.CNM_service_type = NO_CNM_SERVICE;

                (void)CNM_check_for_last_connection(CNM_ESTABLISHMENT_IN_PROGRESS
#if defined(FEATURE_DUAL_SIM )
                                                  , timer_expiry_message->data
#endif
                                                   );
            }
            else
            {
               CNM_send_est_request_from_service_queue();
            }
            /* Reset the REL timer */
            mm_rel_req_sent = FALSE;
            MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Stopping CNM_MM_REL_TIMER");
            CNM_stop_timer(CNM_MM_REL_TIMER, 0xff);  /* should only expired onced */
        }
        break;

    case 0xff:           /* unallocated timer in store - possibly due to */
        break;            /* race between a cancel and an expiry - just   */
        /* ignore message                               */
        
    default:
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unknown Timer id in Timer expiry msg: received timer_id = %d\n",
                   timer_expiry_message->timer_id);
        break;
    }
    
    if (GS_IS_NOT_UNDEFINED(event))
    {
        
    /*
     * extract transaction id and protocol descriminator data from
     * the timout message
     */
        
        ti_pd = timer_expiry_message->data;
        transaction_id = (unsigned char) (GET_TI(ti_pd));  /* transaction_id is 
                                                               * connection_id here */

        if(transaction_id < MAXNO_CC_TRANSACTIONS)
        {
           switch (GET_PD(ti_pd) )
           {
           case PD_CC:       /* if this is a CC transaction */
               
              if(cc_transaction_information_p[transaction_id] != NULL)
              {
                cc_state = cc_transaction_information_p[transaction_id]->cc_state;
              }
              
              action = CNM_cc_check_respond_to_event( transaction_id,event );
   
              if((event == T323_expiry_event) || 
                 (event == T324_expiry_event))
              {
#ifdef FEATURE_WCDMA
                mn_store_rab_status();
#endif
              }
   
              if ( action != 0)
              {
   
                  CNM_cc_respond_to_event(transaction_id, action );
   
                  /* set up standard cause ie */
                                 
                  (void)CNM_write_cause_ie( (byte *)&cause[0], RECOVERY_ON_TIMER_EXPIRY );
                                 
                  switch (timer_expiry_message->timer_id)
                  {
                      case TIMER_T303:
   
                          if (cc_state == CC_STATE_U0 || cc_state == CC_STATE_U0_1)
                          {
                              /* T303 timeout - send reject to MN */
                              if ( CNM_setup_MNCC_REJ_IND(  p_mncc_rej_ind,
                                                            transaction_id,
                                                            CNM_MN_REJECT_CAUSE,  
                                                            CNM_REJ_TIMER_T303_EXP, 
                                                            NULL))
                              {
                                  CNM_send_message((IMH_T *)p_mncc_rej_ind, GS_QUEUE_MN_CM );
                              }                       
                                             
#if defined(FEATURE_DUAL_SIM )
                              if(MNCNM_IS_SINGLE_STACK)
                              {
                                if( mn_as_id == SYS_MODEM_AS_ID_NONE
                                    && mn_call_information_p[transaction_id] != NULL )
                                {
                                  mn_as_id = mn_call_information_p[transaction_id]->as_id;
                                }
                              }
#endif
                              (void)CNM_check_for_last_connection(CNM_ESTABLISHMENT_IN_PROGRESS
#if defined(FEATURE_DUAL_SIM )
                                                                         , mn_as_id
#endif
                                                                 );
                          }                              
                          else 
                          {
                             /* Send DISCONNECT message to network to initiate call clearing  */
                              p_disconnect->msg_head.IMH.message_set = MS_CC_MM;
   
                              p_disconnect->msg_head.IMH.message_id  =  MMCNM_DATA_REQ;
                                         
                              p_disconnect->msg_head.ti_pd =
                                             (unsigned char) ((transaction_id << 4) + PD_CC);
                                         
                              p_disconnect->msg_head.type = DISCONNECT;
                                         
                              /*
                               * note IEI in cause is not copied as it is not
                               * in message
                               */
                             
                              min_size = (int)MIN(sizeof(p_disconnect->data),(sizeof(cause)-1)) ;
                              if( (cause[1] + 1) > min_size ) 
                              {
                                   cause[1] = min_size -1;
                                          MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid cause[1] value, truncating\n");
                              }
   
   
                              (void)memscpy( (VOID_DATA *) &p_disconnect->data[0],
                                             sizeof(p_disconnect->data),
                                             (const VOID_DATA *) &cause[1],
                                             cause[1] + 1);
                                         
                              PUT_IMH_LEN( sizeof(msg_head_T) + cause[1] + 1 -
                                             sizeof(IMH_T),
                                             &p_disconnect->msg_head.IMH);
                                         
                              CNM_manage_cnm_data_queue(
                                             (mmcnm_data_req_T *)((void *)p_disconnect));
                                         
                          }
    
                          /*
                           * if this timeout was related to the current entry in
                           * the mm_connection pending store, then remove this
                           * entry, and check for any more in the service request
                           * queue
                           */
                                     
                          if (  (mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT) ||
                                 (mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT_FULL_SPEECH) ||
                                 (mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT_HALF_SPEECH) ||
                                 (mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_9600) ||
                                 (mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_4800) ||
                                 (mm_connection_pending.CNM_service_type == MO_CALL_ESTABLISHMENT_DATA_2400) ||
                                 (mm_connection_pending.CNM_service_type == EMERGENCY_CALL_ESTABLISHMENT) )
   
                          {
                               if (mm_connection_pending.connection_id == transaction_id)
                               {
                                   CNM_send_est_request_from_service_queue();
                               }
                          }
                                     
                          break;
   
                      case TIMER_T305:
   
                          /* set up release message and send to network
                           */
                                     
                          p_release->msg_head.IMH.message_set = MS_CC_MM;
                                     
                          p_release->msg_head.IMH.message_id  = MMCNM_DATA_REQ;
                                     
                          p_release->msg_head.ti_pd =
                                         (unsigned char) ((transaction_id << 4) + PD_CC);
                                     
                          p_release->msg_head.type = RELEASE;
                                     
                          PUT_IMH_LEN( sizeof(msg_head_T) - sizeof(IMH_T),
                                         &p_release->msg_head.IMH);
                                     
                          /*
                           * set up pointer to cause in transaction information
                           * to simplify code
                           */
                                     
                          p_cause = &(cc_transaction_information_p[transaction_id]->cc_cause[0]);
                          last_octet_cause = p_cause + sizeof(cc_transaction_information_p[transaction_id]->cc_cause) -1;
                                     
                          write_ptr = (byte *)(p_release->data);
                          last_octet_write_ptr = (byte *)&p_release->data[sizeof(p_release->data)-1];
                                     
                          /*
                           * Check that cause is present in
                           * cc_transaction_information
                           */
                                     
                          if ( p_cause[0] == IEI_CAUSE )
                          {
                              cause_length = 
                                  cc_transaction_information_p[transaction_id]->cc_cause[1] + 2;
   
                              min_size = (int)MIN(sizeof(p_release->data),sizeof(cc_transaction_information_p[transaction_id]->cc_cause)) ;
                              if(cause_length >(unsigned int ) min_size)
                              {
                                  cause_length = (unsigned int )min_size;
                                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid cause_length value, truncating\n");
                              }
   
                              (void) memscpy( (VOID_DATA *) write_ptr,
                                             (last_octet_write_ptr - write_ptr + 1),
                                             (const VOID_DATA *) p_cause,
                                             cause_length);
                                         
                              write_ptr += cause_length; 
                                         
                              /*
                               * adjust message length
                               */
                                         
                              GET_IMH_LEN(msg_len, &p_release->msg_head.IMH);
                                         
                              PUT_IMH_LEN(msg_len + cause_length,
                                             &p_release->msg_head.IMH);
   
                              /*
                               * Fill cause in mn_call_information_p, after checking if this is first call clearing message.
                               * This can be checked by checking mn_call_information_p cause's presence field.
                               */
                              if(mn_call_information_p[transaction_id] != NULL)
                              {
                                (void)MN_get_cause_data(p_cause, &(mn_call_information_p[transaction_id]->mo_release_cause_diag_packet), last_octet_cause);
                              }
   
                          }
                                     
                          /*
                           * if first cause is not recovery on timer expiry,
                           * then add that cause to the message as cause 2
                           */
                                     
                          if ( ((p_release->data[2] & 0x60) != 0x60 ) ||
                               ((p_release->data[3] & 0x7f) != RECOVERY_ON_TIMER_EXPIRY) )
                          {
   
                              /*
                               * use previously defined cause IE
                               */
                               
                               min_size =(int)MIN((sizeof(p_release->data) - cause_length),sizeof(cause));
                              
                               if(cause[1]+ 2 > min_size )
                               {
                                  cause[1] = min_size -2 ;
                                  MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid cause[1] value, truncating\n");
                               }
                              (void)memscpy( (VOID_DATA *)(write_ptr),
                                            (last_octet_write_ptr - write_ptr + 1),
                                             (const VOID_DATA *) cause,
                                             cause[1]+2);
                                         
                              /*
                               * adjust message length field to add new cause ie
                               */
                                         
                              GET_IMH_LEN(msg_len, &p_release->msg_head.IMH);
                                         
                              PUT_IMH_LEN(msg_len + cause[1] + 2,
                                             &p_release->msg_head.IMH);
   
                              /*
                               * Fill second cause in mn_call_information_p
                               */
                              if(mn_call_information_p[transaction_id] != NULL)
                              {
                                (void)MN_get_cause_data(cause, &(mn_call_information_p[transaction_id]->mo_release_cause_diag_packet), (cause + sizeof(cause) -1));
                              }
   
                          }
                                     
                          CNM_manage_cnm_data_queue((mmcnm_data_req_T *)
                                                      ((void *)p_release));
                          break;
                                     
                      case TIMER_T308:
                          
                      /*
                       * T308 timeout - If already timed out, set state to CC_STATE_U0,
                       * otherwise send release message to network
                       */
                          
                          if (cc_transaction_information_p[transaction_id]->
                              T308_expired != FALSE)
                          {
                              p_mncc_rel_ind->message_header.message_set = MS_MN_CC;
                              p_mncc_rel_ind->message_header.message_id  =
                                  MNCC_REL_IND;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
                              {
                                p_mncc_rel_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                              }
#endif
                              p_mncc_rel_ind->cause = 0;  /* NULL local cause */
                              
                              (void)CNM_write_cause_ie((byte *)&(p_mncc_rel_ind->data[0]),
                                                        RECOVERY_ON_TIMER_EXPIRY );
                              
                              p_mncc_rel_ind->connection_id = transaction_id;
                              
                              PUT_IMH_LEN( sizeof(MNCC_REL_IND_T) - sizeof(IMH_T),
                                           &p_mncc_rel_ind->message_header );
   
                              if(mn_call_information_p[transaction_id] != NULL)
                              {
                                mn_call_information_p[transaction_id]->mo_release_cause_diag_packet.present = TRUE;
                                mn_call_information_p[transaction_id]->mo_release_cause_diag_packet.cause_value = NORMAL_UNSPECIFIED;
                              }
                              
                              CNM_send_message( (IMH_T *)p_mncc_rel_ind,
                                  GS_QUEUE_MN_CM );
                              
                              
                              CNM_set_state_to_U0( transaction_id );
                          }
                          else
                          {
                              CNM_cc_start_timer(   TIMER_T308,
                                  transaction_id );
                              
                              cc_transaction_information_p[transaction_id]->
                                  T308_expired = TRUE;
                              
                                  /*
                                   * set up release message
                                   */
                              
                              p_release->msg_head.IMH.message_set = MS_CC_MM;
                              
                              p_release->msg_head.IMH.message_id  = MMCNM_DATA_REQ;
                              
                              p_release->msg_head.ti_pd =
                                  (unsigned char) ((transaction_id << 4) + PD_CC);
                              
                              p_release->msg_head.type = RELEASE;
                              
                              PUT_IMH_LEN( sizeof(msg_head_T) - sizeof(IMH_T),
                                  &p_release->msg_head.IMH);
                              
                                  /*
                                   * set up pointer to cause in transaction information
                                   * to simplify code
                                   */
                              
                              p_cause = &cc_transaction_information_p
                                  [transaction_id]->cc_cause[0];
                              last_octet_cause = p_cause + sizeof(cc_transaction_information_p[transaction_id]->cc_cause) -1;
                              
                              write_ptr = (byte *)( p_release->data);
                              last_octet_write_ptr = (byte *)( &p_release->data[sizeof(p_release->data)-1]);
                              
                              /*
                               * Check that cause is present in cc_transaction_information
                               */
                              
                              if ( p_cause[0] == IEI_CAUSE )
                              {
                                  cause_length = 
                                      cc_transaction_information_p[transaction_id]->cc_cause[1] + 2;
   
                                  min_size= (int)MIN(sizeof(p_release->data) , sizeof(cc_transaction_information_p[transaction_id]->cc_cause ) );
                                  if(cause_length >(unsigned int) min_size)
                                  {
                                      cause_length = min_size;
                                      MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid cause length value, truncating\n");
                                  }
   
                                  (void) memscpy( (VOID_DATA *)(write_ptr),
                                                 (last_octet_write_ptr - write_ptr + 1),
                                                 (const VOID_DATA *) p_cause,
                                                 cause_length);
                                  
                                  write_ptr += cause_length;
                                  
                                  /*
                                   * adjust message length
                                   */
                                  
                                  GET_IMH_LEN(msg_len, &p_release->msg_head.IMH);
                                  
                                  PUT_IMH_LEN(msg_len + cause_length,
                                      &p_release->msg_head.IMH);
   
                                  /*
                                   * Fill cause in mn_call_information_p, after checking if this is first call clearing message.
                                   * This can be checked by checking mn_call_information_p cause's presence field.
                                   */
                                  if(mn_call_information_p[transaction_id] != NULL)
                                  {
                                    (void)MN_get_cause_data(p_cause, &(mn_call_information_p[transaction_id]->mo_release_cause_diag_packet), last_octet_cause);
                                  }
   
                              }
                              
                              /*
                               * if first cause is not recovery on timer expiry,
                               * then add that cause to the message as cause 2
                               */
                              
                              if (((p_release->data[2] & 0x60) != 0x60 ) ||
                                  ((p_release->data[3] & 0x7f) != RECOVERY_ON_TIMER_EXPIRY))
                              {
                                  
                              /*
                               * use previously defined cause IE
                               */   
                               
                                   min_size= (int)MIN((sizeof(p_release->data) - cause_length),sizeof(cause));
                                   
                                   if(cause[1]+ 2 > min_size )
                                   {
                                       cause[1] = min_size -2 ;
                                       MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid cause[1] value, truncating\n");
                                   }
                                  (void) memscpy( (VOID_DATA *)(write_ptr),
                                      (last_octet_write_ptr - write_ptr + 1),
                                      (const VOID_DATA *) cause,
                                      cause[1]+2);
                                  
                                  /*
                                   * adjust message length field to add new cause ie
                                   */
                                  
                                  GET_IMH_LEN(msg_len, &p_release->msg_head.IMH);
                                  
                                  PUT_IMH_LEN(msg_len + cause[1] + 2,
                                      &p_release->msg_head.IMH);
   
                                  /*
                                   * Fill second cause in mn_call_information_p
                                   */
                                  if(mn_call_information_p[transaction_id] != NULL)
                                  {
                                    (void)MN_get_cause_data(cause, &(mn_call_information_p[transaction_id]->mo_release_cause_diag_packet), (cause + sizeof(cause) -1));
                                  }
                              }
                              
                              CNM_manage_cnm_data_queue((mmcnm_data_req_T *)
                                  ((void *)p_release));
                              
                          }
                          break;
                                     
                     case TIMER_T323:
                     case TIMER_T313:
                     case TIMER_T310:
                         
                     /*
                      * for all of these, send DISCONNECT message to network
                      */
                         
                         p_disconnect->msg_head.IMH.message_set = MS_CC_MM;
                         
                         p_disconnect->msg_head.IMH.message_id  = MMCNM_DATA_REQ;
                         
                         p_disconnect->msg_head.ti_pd =
                             (unsigned char) ((transaction_id << 4) + PD_CC);
                         
                         p_disconnect->msg_head.type = DISCONNECT;
                         
                         /*
                          * Get cause info from transaction info store.
                          * Note that IEI is not copied to disconnect message
                          */
                         
                         (void) memscpy( (VOID_DATA *) &p_disconnect->data[0],
                             sizeof(p_disconnect->data),
                             (const VOID_DATA *) & cause[1],
                             cause[1] + 1);
                         
                         PUT_IMH_LEN( sizeof(msg_head_T) + cause[1] + 1
                             - sizeof(IMH_T),
                             &p_disconnect->msg_head.IMH);
                         
                         CNM_manage_cnm_data_queue((mmcnm_data_req_T *)
                             ((void *)p_disconnect));
                         
                             /*
                              * store recovery on timer expiry as the cause for this
                              * transaction
                              */
                         min_size = (int)MIN( sizeof(cc_transaction_information_p[transaction_id]->cc_cause), sizeof(cause));
                         if(cause[1]+ 2 > min_size )
                         {
                              cause[1] = min_size -2 ;
                              MSG_ERROR_DS_0(MN_SUB,"=MNCNM=  Invalid cause[1] value, truncating\n");
                         }
                         (void)memscpy( (VOID_DATA *) cc_transaction_information_p[transaction_id]->cc_cause,
                             sizeof(cc_transaction_information_p[transaction_id]->cc_cause),
                             (const VOID_DATA *) cause,
                             cause[1] + 2);
                         
                         break;
                         
                     case TIMER_T324:
                         CNM_process_t324_expiry(transaction_id);
                         break;    
   
                     case TIMER_T336:
                         CNM_process_t336_expiry(transaction_id);
                         break;
                         
                     case TIMER_T337:
                         CNM_process_t337_expiry(transaction_id);
                         break;
                         
                     default:
                         ERR("Unknown Timer id in Timer expiry msg: received timer_id = %d\n",
                             (dword) timer_expiry_message->timer_id, 0, 0);
                         break;
                         
                  }  /* end switch on timer id */
                  
              } /* end if response to action is required */
               
              break;
               
               /*
                * Following cases TBD
                */
                   
            case PD_SS:
                break;
                
            case PD_SMS:
                switch(timer_expiry_message->timer_id)
                {
                case TIMER_TC1M_MO:
                    CNM_process_mo_sms((IMH_T *)((void *)timer_expiry_message),(unsigned char)GET_TI(ti_pd));
                    break;
   
                case TIMER_TC1M_MT:
                    CNM_process_mt_sms((IMH_T *)((void *)timer_expiry_message),(unsigned char)GET_TI(ti_pd));
                    break;
   
                default:
                    break;
   
                }
                break;
                
            default:
                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected PD: received PD = %d\n",(dword) GET_PD(ti_pd));
                break;
                
           } /* switch on PD */
        }
        else
        {
           MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Incorrect data = %d received from timer module\n",ti_pd);
        }
    }  /* event not defined */
   
    CNM_free( p_mncc_rej_ind );
    CNM_free( p_mncc_rel_ind );
    CNM_free( p_release );
    CNM_free( p_disconnect );
   
}
/**************** End of   CNM_recover_on_timer_expiry ****************************************/
