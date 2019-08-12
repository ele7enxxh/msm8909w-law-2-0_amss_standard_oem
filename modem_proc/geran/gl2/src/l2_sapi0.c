/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         L 2   S A P I 0

GENERAL DESCRIPTION
   This module contains functions that are specific data link services
   on SAPI 0.

EXTERNALIZED FUNCTIONS
   l2_SAPI0_control

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_sapi0.c_v   1.8   05 Jun 2002 10:55:44   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_sapi0.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
29/10/10   SJW      Added support for Dual SIM
07/22/09   SJW      Added GL2_ALLOC and GL2_FREE macros for new heap manager
02/13/03   JAC      Added handler for link establishment even if link already established
12/05/02   JAC      Added missing semicolon after ASSERT in l2_restore_resume_msg
10/17/02   JAC      Removed wait_UA_sent variable, in l2_data
10/04/02   JAC      Code reorg
08/26/02   JAC      Added handling of data_request in pending state
08/13/02   JAC      send dl_suspend_cnf when doing forced suspend
07/31/02   JAC      added forced suspend for interrat handover
05/30/02   JAC      added SAPI0_UA_PENDING
05/18/02   JAC      Handle release_req in all states even in Null, in case
                    rr or l2 gets in a weird state
03/28/02   JAC      Go to the null state on a reconnection failure
03/25/02   JAC      Store additional l3 messages in link suspended state
02/28/02   JAC      Added logging for states
12/14/01   JAC      Set V(A) to 0 in l2_handle_resumption_ua_reception
10/19/01   JAC      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "environ.h"
#include "sys_type.h"
#include "sys_stru.h"
#include "sys_cnst.h"
#include "rex.h"
#include "timers.h"
#include "gs.h"
#include "msg.h"
#include "err.h"
#include "rr_l2_l1.h"
#include "rr_l2.h"
#include "l2_l1.h"
#include "l2_l1_g.h"
#include "l2i.h"
#include "l2_transfer.h"
#include "l2_msg.h"
#include "l2_establish.h"
#include "l2_release.h"
#include "l2_send.h"
#include "l2_transmit.h"
#include "stringl/stringl.h"
#include "l2_log.h"

/*
 *   Function name:  l2_display_SAPI0_state
 *   --------------------------------------
 *
 *   Description: Display the SAPI0 state
 *   ------------
 *
 *
 *   Parameters:  Sapi0 state
 *   -----------
 *
 *   Returns:     none
 *   --------
 *
 */

static void l2_display_SAPI0_state(SAPI0_state_T SAPI0_state)
{
  switch (SAPI0_state)
  {
    case SAPI0_NULL:
      MSG_GERAN_HIGH_0("L2 state:SAPI0_NULL");
      break;

    case SAPI0_CONNECT_PEND:
      MSG_GERAN_HIGH_0("L2 state:SAPI0_CONNECT_PEND");
      break;

    case SAPI0_IDLE:
      MSG_GERAN_HIGH_0("L2 state:SAPI0_IDLE");
      break;

    case SAPI0_ESTABLISHMENT_PENDING:
      MSG_GERAN_HIGH_0("L2 state:SAPI0_ESTABLISHMENT_PENDING");
      break;

    case SAPI0_RELEASE_PENDING:
      MSG_GERAN_HIGH_0("L2 state:SAPI0_RELEASE_PENDING");
      break;

    case SAPI0_LINK_ESTABLISHED:
      MSG_GERAN_HIGH_0("L2 state:SAPI0_LINK_ESTABLISHED");
      break;

    case SAPI0_TIMER_RECOVERY:
      MSG_GERAN_HIGH_0("L2 state:SAPI0_TIMER_RECOVERY");
      break;

    case LINK_SUSPENDED:
      MSG_GERAN_HIGH_0("L2 state:LINK_SUSPENDED");
      break;

    case SAPI0_UA_PENDING:
      MSG_GERAN_HIGH_0("L2 state:SAPI0 UA PENDING");
      break;

    default:
      MSG_GERAN_HIGH_0("L2 state:UNKNOWN STATE");
      break;
  }
}


/*
 *   Function name:  l2_resume_link
 *   -------------------------------
 *
 *   Description: Resume a link
 *   ------------
 *
 *
 *   Parameters: none
 *   -----------
 *
 *
 *
 *   Returns:     none
 *   --------
 *
 *
 */

static void l2_resume_link(l2_store_T *l2_data_ptr)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, SAPI0);


  channel_info = l2_get_channel_info(sapi_info, DCCH);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /* Clears only some parts of the SAPI, not the buffers and queues */
  l2_clear_sapi(l2_data_ptr, SAPI0, FALSE);

  if (l2_data_ptr->establish_mode == CONTENTION_RESOLUTION)
  {
    l2_code_sabm(sapi_info, channel_info, FALSE,
                 l2_data_ptr->l3_message_to_send,
                 l2_data_ptr->l3_message_length);
  }
  else
  {
    l2_code_sabm(sapi_info, channel_info, FALSE, NULL, 0);
  }

  /************************************/
  /* This store is set to prevent the */
  /* Assignment Complete or the       */
  /* Handover Complete to be sent     */
  /* before the UA comes              */
  /************************************/

  /* Disables sending from send buffer */
  channel_info->outstanding_u_frame = TRUE;

  sapi_info->T200_state = TO_BE_SET;

  return;
}


/*
 *   Function name:  l2_suspend_link
 *   -------------------------------
 *
 *   Description: Suspend a link
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

static void l2_suspend_link(l2_store_T *l2_data_ptr)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi0_ptr = l2_get_sapi_info(l2_data_ptr, SAPI0);
  sapi_info_T    *sapi3_ptr = l2_get_sapi_info(l2_data_ptr, SAPI3);

  channel_info = l2_get_channel_info(sapi0_ptr, DCCH);
  if ((sapi0_ptr == NULL) || (sapi3_ptr == NULL) || (channel_info == NULL))
  {
    return;
  }

  if (channel_info->send_buffer.no_of_entries != 0)
  {
    /********************************************/
    /* Put the message in the first position of */
    /* the send queue                           */
    /********************************************/

    // Clearing the send buffer deletes the current
    // message in the send buffer but does not alter
    // the send queue. The send queue is only incremented
    // when the message has been ack. The uncompleted
    // message will still be in the send queue.
    // When the link is restablished, l2 will call
    // l2_restore_resume, which will place the resume message
    // in the DL_RESUME or DL_RECONNECT_REQ into the front of
    // the send queue, pushing our saved messages backwards.
    // When this resume message is ack, our saved message
    // will start to be sent
    // The send queue is cleared everytime l2_establish_link is
    // called so if the resumption failed, the
    // saved messages will be deleted when the a new link
    // is established
    l2_clear_send_buffer(sapi0_ptr, channel_info->channel);
  }

  /*************************************/
  /* Clear ready variables so we don't */
  /* send anything until L1 say it     */
  /* is ready                          */
  /*************************************/
  l2_data_ptr->sacch_ready = FALSE;
  l2_data_ptr->dcch_ready  = FALSE;

  /**********************************************/
  /* Delete the contents of the transmit_buffer */
  /**********************************************/
  channel_info->transmit_buffer.valid_data_flag = FALSE;
  channel_info->transmit_buffer.retransmission  = FALSE;

  /***********************************/
  /* Stop the T200 timer, if running */
  /***********************************/
  l2_clear_T200_timer(sapi0_ptr);

  l2_clear_T_ack_timer(sapi0_ptr);
  l2_clear_T_ack_timer(sapi3_ptr);

  /* Disables sending from send buffer */
  channel_info->outstanding_u_frame = TRUE;

  l2_send_dl_suspend_cnf(l2_data_ptr);

  return;
}


/*
 *   Function name:   l2_handle_resumption_ua_reception
 *   --------------------------------------------------
 *
 *   Description: Receive the UA
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     l2_event - event for state machine
 *   --------
 */

static l2_event_T l2_handle_resumption_ua_reception(l2_store_T *l2_data_ptr)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info    = l2_get_sapi_info(l2_data_ptr, SAPI0);

  channel_info = l2_get_channel_info(sapi_info, DCCH);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return EV_NO_EVENT;
  }

  /* Will enable sending from send buffer */
  l2_clear_sapi(l2_data_ptr, SAPI0, FALSE);

  MSG_GERAN_MED_0("Resuming link");

  /************************************/
  /* Assignment Complete or the       */
  /* Handover Complete can be sent    */
  /************************************/

  /*
   * Pass the first segment of the message contained in
   * the send_buffer to the transmit_buffer
   */
  if (channel_info->send_buffer.no_of_entries != 0)
  {
    l2_code_i(0, FALSE, sapi_info, channel_info, FALSE);

    sapi_info->T200_state = TO_BE_SET;
  }
  else
  {
    MSG_GERAN_ERROR_0("No resume msg in buffer");
  }

  if (l2_data_ptr->reestablishment_type == RESUMPTION)
  {
    l2_send_dl_resume_cnf(l2_data_ptr);
  }
  else
  {
    l2_send_dl_reconnect_cnf(l2_data_ptr);
  }

  return(EV_LINK_ESTABLISHED);
}

/*
 *   Function name:  l2_store_resume_message
 *   ---------------------------------------
 *
 *   Description: Store the message passed in
 *   ------------ the resume or reconnect signal
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

static void l2_store_resume_message(l2_store_T *l2_data_ptr)
{
  if (l2_data_ptr != NULL)
  {
    /* check if resume message will fit in 1 segment */
    if (l2_data_ptr->l3_message_length > N201_DCCH)
    {
      MSG_GERAN_ERROR_1("Resume msg length %d invalid exceeds DCCH max",(int)(l2_data_ptr->l3_message_length));
      l2_data_ptr->l3_message_length = N201_DCCH;
    }

    (void)memscpy(l2_data_ptr->l3_resume_message, sizeof(l2_data_ptr->l3_resume_message),
                  l2_data_ptr->l3_message_to_send, l2_data_ptr->l3_message_length);
    l2_data_ptr->l3_resume_message_length = l2_data_ptr->l3_message_length;
  }

  return;
}

/*
 *   Function name:  l2_restore_resume_message
 *   ---------------------------------------
 *
 *   Description:
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

static void l2_restore_resume_message(l2_store_T *l2_data_ptr)
{
  segment_descriptor_T *new_segment;
  channel_info_T       *channel_info;
  sapi_info_T          *sapi_info = l2_get_sapi_info(l2_data_ptr, SAPI0);
  message_entry_T      *new_message_entry;

  channel_info = l2_get_channel_info(sapi_info, DCCH);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /**************************************/
  /* Put the message in the send buffer */
  /**************************************/

  if (l2_data_ptr->l3_resume_message_length != 0)
  {
    if (channel_info->send_buffer.no_of_entries != 0)
    {
      MSG_GERAN_HIGH_0("Msg in SAPI0 DCCH buffer during resume");
    }

    channel_info->send_buffer.no_of_entries = 1;

    new_segment = (segment_descriptor_T *)GL2_ALLOC(sizeof(segment_descriptor_T));

    if (new_segment != NULL)
    {
      new_segment->next = NULL;
      new_segment->segment_length = l2_data_ptr->l3_resume_message_length;

      new_segment->segment = (byte *)GL2_ALLOC(new_segment->segment_length);

      if (new_segment->segment != NULL)
      {
        (void)memscpy(new_segment->segment, new_segment->segment_length,
                      l2_data_ptr->l3_resume_message, l2_data_ptr->l3_resume_message_length);
      }
    }

    channel_info->send_buffer.first_segment = new_segment;
    channel_info->send_buffer.next_segment  = new_segment;

    /*********************************************/
    /* Put the message in the first place of the */
    /* send queue                                */
    /*********************************************/

    new_message_entry = (message_entry_T *)GL2_ALLOC(sizeof(message_entry_T));

    if (new_message_entry != NULL)
    {
      new_message_entry->next_message = channel_info->send_queue.first_message;
      new_message_entry->message_length = l2_data_ptr->l3_resume_message_length;
      new_message_entry->message_number = 0;

      new_message_entry->message = (byte *)GL2_ALLOC(new_message_entry->message_length);

      if (new_message_entry->message != NULL)
      {
        (void)memscpy(new_message_entry->message, new_message_entry->message_length,
                      l2_data_ptr->l3_resume_message, l2_data_ptr->l3_resume_message_length);
      }
    }

    channel_info->send_queue.first_message = new_message_entry;
    channel_info->send_queue.no_of_entries++;

    l2_data_ptr->l3_resume_message_length = 0;
  }
}


static void l2_forced_suspend_init(l2_store_T *l2_data_ptr)
{
  /*********************************************/
  /* Clears data structures on SAPI0 and SAPI3 */
  /*********************************************/
  l2_clear_sapi(l2_data_ptr, SAPI0, TRUE);
  l2_clear_sapi(l2_data_ptr, SAPI3, TRUE);

  return;
}

/*
 *   Function name:  l2_SAPI0_control
 *   --------------------------------
 *
 *   Description: Control the SAPI0 state machine
 *   ------------
 *
 *   Parameters: SAPI0_event - input event
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_SAPI0_control(l2_store_T *l2_data_ptr, l2_event_T SAPI0_event)
{
  l2_event_T new_SAPI0_event = EV_NO_EVENT;

  if (SAPI0_event == EV_RESET_L2)
  {
    l2_data_ptr->SAPI0_state     = SAPI0_NULL;
    l2_data_ptr->old_SAPI0_state = SAPI0_NULL;
    new_SAPI0_event              = EV_NO_EVENT;
    SAPI0_event                  = EV_NO_EVENT;
    l2_log_state(l2_data_ptr, SAPI0, (byte)SAPI0_NULL, EV_NO_EVENT);
  }
  else if (SAPI0_event == EV_FORCED_SUSPEND_REQ)
  {
    if ((l2_data_ptr->SAPI0_state != SAPI0_NULL) && (l2_data_ptr->SAPI0_state != SAPI0_IDLE))
    {
      MSG_GERAN_ERROR_1("L2 not reset before forced suspend %d",(int)l2_data_ptr->SAPI0_state);
    }
    l2_forced_suspend_init(l2_data_ptr);
    l2_suspend_link(l2_data_ptr);
    l2_data_ptr->SAPI0_state = LINK_SUSPENDED;
    SAPI0_event = EV_NO_EVENT; /* no need to process this any further */
  }
  else if (SAPI0_event == EV_ESTABLISH_REQUEST)
  {
    if ((l2_data_ptr->SAPI0_state != SAPI0_NULL) && (l2_data_ptr->SAPI0_state != SAPI0_IDLE))
    {
      /* Should never happen */
      MSG_GERAN_ERROR_1("Link already established %d, reset",(int)l2_data_ptr->SAPI0_state);

      /* Go to idle, and handle the establish request there */
      l2_data_ptr->SAPI0_state = SAPI0_IDLE;
    }
  }

  if ((l2_data_ptr->current_l2_channel_type != DCCH) || (l2_data_ptr->current_SAPI != SAPI0))
  {
    MSG_GERAN_HIGH_2("SAPI%1d, current channel type=%d", (int)l2_data_ptr->current_SAPI, (int)l2_data_ptr->current_l2_channel_type);
  }

  while (SAPI0_event != EV_NO_EVENT)
  {
    MSG_GERAN_MED_1("L2 SAPI0 event %d",SAPI0_event);

    l2_log_state(l2_data_ptr, SAPI0,(byte)l2_data_ptr->SAPI0_state,SAPI0_event);

    switch (l2_data_ptr->SAPI0_state)
    {
      case SAPI0_NULL:
        {
          if (SAPI0_event == EV_CONNECT_RECEIVED)
          {
            l2_data_ptr->SAPI0_state = SAPI0_IDLE;
          }
          else if (SAPI0_event == EV_ESTABLISH_REQUEST )
          {
            /*
             * an establishment request has been received ahead of the
             * connect ind from layer 1 - This is possible as there is
             * a race between RR sending the request and layer 1 sending
             * the connect. Need to go to a transient state to handle
             * this situation
             */
            l2_data_ptr->SAPI0_state = SAPI0_CONNECT_PEND;
          }
          else if ((SAPI0_event == EV_RELEASE_REQUEST) && (l2_data_ptr->release_mode == LOCAL_END_RELEASE))
          {
            MSG_GERAN_ERROR_0("Got a release req, when no link is established");
            l2_release_link(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI0_state = SAPI0_NULL;
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event %d",SAPI0_event);
          }
          break;
        }

      case SAPI0_CONNECT_PEND:
        {
          if (SAPI0_event == EV_CONNECT_RECEIVED)
          {
            l2_establish_link(l2_data_ptr, SAPI0, l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->previous_SAPI0_state = SAPI0_IDLE;
            l2_data_ptr->SAPI0_state = SAPI0_ESTABLISHMENT_PENDING;
          }
          else if ((SAPI0_event == EV_RELEASE_REQUEST) && (l2_data_ptr->release_mode == LOCAL_END_RELEASE))
          {
            MSG_GERAN_ERROR_0("Got a release req, when no link is established");
            l2_release_link(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI0_state = SAPI0_NULL;
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event %d",SAPI0_event);
          }
          break;
        }

      case SAPI0_IDLE:
        {
          if (SAPI0_event == EV_RELEASE_REQUEST)
          {
            l2_data_ptr->release_mode = LOCAL_END_RELEASE;
            MSG_GERAN_ERROR_0("Got a release req, when in idle?");
            l2_release_link(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI0_event == EV_ESTABLISH_REQUEST)
          {
            l2_establish_link(l2_data_ptr, SAPI0, l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->previous_SAPI0_state = SAPI0_IDLE;
            l2_data_ptr->SAPI0_state = SAPI0_ESTABLISHMENT_PENDING;
          }
          else if (SAPI0_event == EV_DISC_RECEIVED)
          {
            l2_send_dm_response(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
          }
          else if ((SAPI0_event == EV_I_RECEIVED) ||
                   (SAPI0_event == EV_RR_RECEIVED )||
                   (SAPI0_event == EV_REJ_RECEIVED ))
          {
            if (l2_data_ptr->received_PF == 1)
            {
              l2_send_dm_response(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
            }
          }
          else if (SAPI0_event == EV_CONNECT_RECEIVED)
          {
            /* soak this up: it happens on every new connection */
            MSG_GERAN_LOW_0("EV_CONNECT_RECEIVED in SAPI0_IDLE");
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event %d",SAPI0_event);
          }
          break;
        }

      case SAPI0_ESTABLISHMENT_PENDING:
        {
          if (SAPI0_event == EV_RELEASE_REQUEST)
          {
            MSG_GERAN_ERROR_0("Got a release req, when link not established");
            l2_data_ptr->release_mode = LOCAL_END_RELEASE;
            l2_release_link(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI0_state = SAPI0_IDLE;
          }
          else if (SAPI0_event == EV_UA_RECEIVED)
          {
            if (l2_data_ptr->previous_SAPI0_state == SAPI0_IDLE)
            {
              new_SAPI0_event = l2_handle_establish_ua_reception(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
            }
            else
            {
              /*
               * link has been established, so put the resume message
               * in the buffers for transmission
               */
              l2_restore_resume_message(l2_data_ptr);
              new_SAPI0_event = l2_handle_resumption_ua_reception(l2_data_ptr);
            }
          }
          else if (SAPI0_event == EV_DATA_REQUEST)
          {
            /******************************************************************/
            /* Important not to clear entire SAPI or message gets invalidated */
            /* This will not overwrite the resume msg, since the resume msg   */
            /* is written after the UA arrives, This msg will be placed in    */
            /* the send queue and buffer.                                     */
            /******************************************************************/
            l2_store_new_l3_message(l2_data_ptr, SAPI0, l2_data_ptr->current_l2_channel_type, SAPI_ESTABLISHED);
          }
          else if (SAPI0_event == EV_DM_RECEIVED)
          {
            new_SAPI0_event = l2_handle_establish_dm_reception(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI0_event == EV_CONTENTION_FAILED)
          {
            l2_clear_sapi(l2_data_ptr, SAPI0, TRUE);
            l2_data_ptr->SAPI0_state = SAPI0_IDLE;
          }
          else if (SAPI0_event == EV_ABORT_ESTABLISHMENT)
          {
            if (l2_data_ptr->previous_SAPI0_state == SAPI0_IDLE)
            {
              l2_data_ptr->SAPI0_state = SAPI0_IDLE;
            }
            else if (l2_data_ptr->reestablishment_type==RECONNECTION)
            {
              /* we are trying to reconnect to an old channel but failed
               * A MDL_ERROR_IND was sent in l2_retransmit_sabm
               * restart L2
               */
              l2_data_ptr->SAPI0_state = SAPI0_NULL;
            }
            else
            {
              /* resumption failed go back to suspended state */
              l2_data_ptr->SAPI0_state = LINK_SUSPENDED;
            }
          }
          else if (SAPI0_event == EV_LINK_ESTABLISHED)
          {
            l2_data_ptr->SAPI0_state = SAPI0_LINK_ESTABLISHED;
          }
          else if (SAPI0_event == EV_T200_TIMEOUT)
          {
            new_SAPI0_event = l2_retransmit_sabm(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event %d",SAPI0_event);
          }
          break;
        }

      case SAPI0_RELEASE_PENDING:
        {
          if (SAPI0_event == EV_RELEASE_REQUEST)
          {
            MSG_GERAN_ERROR_0("Got a release req, while already releasing");
            l2_data_ptr->release_mode = LOCAL_END_RELEASE;
            l2_release_link(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI0_state = SAPI0_IDLE;
          }
          else if (SAPI0_event == EV_UA_RECEIVED)
          {
            new_SAPI0_event = l2_handle_release_ua_reception(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI0_event == EV_DM_RECEIVED)
          {
            new_SAPI0_event = l2_handle_release_dm_reception(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI0_event == EV_T200_TIMEOUT)
          {
            new_SAPI0_event = l2_retransmit_disc_command(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI0_event == EV_RELEASE_CONFIRMED)
          {
            l2_data_ptr->SAPI0_state = SAPI0_IDLE;
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event %d",SAPI0_event);
          }
          break;
        }

      case SAPI0_UA_PENDING:
        {
          if (SAPI0_event == EV_RELEASE_REQUEST)
          {
            MSG_GERAN_ERROR_0("Got a release req, while already releasing");
            l2_data_ptr->release_mode = LOCAL_END_RELEASE;
            l2_release_link(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI0_state = SAPI0_IDLE;
          }
          else if (SAPI0_event == EV_T200_TIMEOUT)
          {
            l2_send_dl_release_ind(l2_data_ptr,SAPI0, NORMAL_RELEASE);
            l2_data_ptr->SAPI0_state = SAPI0_IDLE;
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event %d",SAPI0_event);
          }
          break;
        }

      case SAPI0_LINK_ESTABLISHED:
        {
          if (SAPI0_event == EV_RELEASE_REQUEST)
          {
            l2_release_link(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);

            if (l2_data_ptr->release_mode == NORMAL_RELEASE)
            {
              l2_data_ptr->SAPI0_state = SAPI0_RELEASE_PENDING;
            }
            else
            {
              l2_data_ptr->SAPI0_state = SAPI0_IDLE;
            }
          }
          else if (SAPI0_event == EV_SABM_RECEIVED)
          {
            /* re-establishment of SAPI0 - remain in this state */
            (void)l2_handle_sabm_reception(l2_data_ptr, SAPI0, l2_data_ptr->current_l2_channel_type, TRUE);
          }
          else if (SAPI0_event == EV_DISC_RECEIVED)
          {
            l2_handle_disc_reception(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI0_state = SAPI0_UA_PENDING;
          }
          else if (SAPI0_event == EV_UA_RECEIVED)
          {
            L2_SEND_MDL_ERROR_IND(l2_data_ptr, UNSOLICITED_UA_RESPONSE);
          }
          else if (SAPI0_event == EV_DM_RECEIVED)
          {
            if (l2_data_ptr->received_PF == 1)
            {
              L2_SEND_MDL_ERROR_IND(l2_data_ptr, UNSOLICITED_DM_RESPONSE);
            }
            else
            {
              L2_SEND_MDL_ERROR_IND(l2_data_ptr, UNSOLICITED_DM_RESPONSE_PAR);
            }
          }
          else if (SAPI0_event == EV_SUSPEND_REQUEST)
          {
            l2_suspend_link(l2_data_ptr);
            l2_data_ptr->SAPI0_state = LINK_SUSPENDED;
          }
          else if (SAPI0_event == EV_I_RECEIVED)
          {
            l2_handle_info_reception(l2_data_ptr,SAPI0,DCCH);
          }
          else if (SAPI0_event == EV_RR_RECEIVED)
          {
            l2_handle_normal_rr_reception(l2_data_ptr,SAPI0,DCCH);
          }
          else if (SAPI0_event == EV_REJ_RECEIVED)
          {
            l2_handle_normal_rej_reception(l2_data_ptr,SAPI0,DCCH);
          }
          else if (SAPI0_event == EV_T200_TIMEOUT)
          {
            new_SAPI0_event = l2_check_pending_sequence_error(l2_data_ptr,SAPI0,DCCH);
            l2_handle_I_retransmission(l2_data_ptr,SAPI0,DCCH);
            l2_data_ptr->SAPI0_state = SAPI0_TIMER_RECOVERY;
          }
          else if (SAPI0_event == EV_DATA_REQUEST)
          {
            l2_store_new_l3_message(l2_data_ptr, SAPI0, l2_data_ptr->current_l2_channel_type, SAPI_ESTABLISHED);
          }
          else if (SAPI0_event == EV_OPTIONAL_SEND)
          {
            l2_handle_optional_send_timeout(l2_data_ptr, l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI0_event == EV_CONNECT_RECEIVED)
          {
            /* this may happen if the channel mode is modified */
            MSG_GERAN_HIGH_1("SAPI0 PH_CONNECT_IND from Layer 1: channel %d",
                     (int)l2_data_ptr->last_dcch_type);
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event %d",SAPI0_event);
          }
          break;
        }

      case SAPI0_TIMER_RECOVERY:
        {
          if (SAPI0_event == EV_RELEASE_REQUEST)
          {
            l2_release_link(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);

            if (l2_data_ptr->release_mode == NORMAL_RELEASE)
            {
              l2_data_ptr->SAPI0_state = SAPI0_RELEASE_PENDING;
            }
            else
            {
              l2_data_ptr->SAPI0_state = SAPI0_IDLE;
            }
          }
          else if (SAPI0_event == EV_SABM_RECEIVED)
          {
            /* re-establishment of SAPI0 : clear recovery condition if SABM is valid */
            new_SAPI0_event = l2_handle_sabm_reception(l2_data_ptr, SAPI0, l2_data_ptr->current_l2_channel_type, TRUE);
          }
          else if (SAPI0_event == EV_LINK_ESTABLISHED)
          {
            l2_data_ptr->SAPI0_state = SAPI0_LINK_ESTABLISHED;
          }
          else if (SAPI0_event == EV_I_RECEIVED)
          {
            l2_handle_info_reception(l2_data_ptr,SAPI0,DCCH);
          }
          else if (SAPI0_event == EV_RR_RECEIVED)
          {
            new_SAPI0_event = l2_handle_recovery_rr_reception(l2_data_ptr,SAPI0,DCCH);
          }
          else if (SAPI0_event == EV_REJ_RECEIVED)
          {
            new_SAPI0_event = l2_handle_recovery_rej_reception(l2_data_ptr,SAPI0,DCCH);
          }
          else if (SAPI0_event == EV_CLEAR_RECOVERY_CONDITION)
          {
            l2_data_ptr->SAPI0_state = SAPI0_LINK_ESTABLISHED;
          }
          else if (SAPI0_event == EV_DATA_REQUEST)
          {
            l2_store_new_l3_message(l2_data_ptr, SAPI0, l2_data_ptr->current_l2_channel_type, SAPI_ESTABLISHED);
          }
          else if (SAPI0_event == EV_T200_TIMEOUT)
          {
            new_SAPI0_event = l2_check_pending_sequence_error(l2_data_ptr,SAPI0,DCCH);
            l2_handle_I_retransmission(l2_data_ptr,SAPI0,DCCH);
          }
          else if (SAPI0_event == EV_SUSPEND_REQUEST)
          {
            l2_suspend_link(l2_data_ptr);
            l2_data_ptr->SAPI0_state = LINK_SUSPENDED;
          }
          else if (SAPI0_event == EV_DISC_RECEIVED)
          {
            l2_handle_disc_reception(l2_data_ptr,SAPI0,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI0_state = SAPI0_UA_PENDING;
          }
          else if (SAPI0_event == EV_OPTIONAL_SEND)
          {
            l2_handle_optional_send_timeout(l2_data_ptr, l2_data_ptr->current_l2_channel_type);
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event %d",SAPI0_event);
          }
          break;
        }

      case LINK_SUSPENDED:
        {
          if (SAPI0_event == EV_RELEASE_REQUEST)
          {
            MSG_GERAN_ERROR_0("Got a release req, in suspended");
            l2_data_ptr->release_mode = LOCAL_END_RELEASE;
            l2_release_link(l2_data_ptr,SAPI0,DCCH);
            l2_data_ptr->SAPI0_state = SAPI0_IDLE;
          }
          else if ((SAPI0_event == EV_RESUME_REQUEST) ||
                   (SAPI0_event == EV_RECONNECT_REQUEST))
          {
            if (SAPI0_event == EV_RESUME_REQUEST)
            {
              l2_data_ptr->reestablishment_type = RESUMPTION;
            }
            else
            {
              l2_data_ptr->reestablishment_type = RECONNECTION;
            }

            l2_store_resume_message(l2_data_ptr);
            l2_resume_link(l2_data_ptr);
            l2_data_ptr->previous_SAPI0_state = LINK_SUSPENDED;
            l2_data_ptr->SAPI0_state = SAPI0_ESTABLISHMENT_PENDING;
          }
          else if (SAPI0_event == EV_DATA_REQUEST)
          {
            /*****************************************************************/
            /* Msg would be put in send buffer and queue, since              */
            /* outstanding_u_frame is set it will not be sent out until      */
            /* we resume                                                     */
            /*****************************************************************/
            MSG_GERAN_HIGH_0("Data request when suspended");

            if (  l2_data_ptr->sapi0.DCCH_channel.outstanding_u_frame == FALSE )
            {
              MSG_GERAN_ERROR_0("Outstanding frame=0 when suspended");
            }

            l2_store_new_l3_message(l2_data_ptr, SAPI0, l2_data_ptr->current_l2_channel_type, SAPI_SUSPENDED);
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event %d",SAPI0_event);
          }
          break;
        }
    }

    if (new_SAPI0_event != EV_NO_EVENT)
    {
      SAPI0_event = new_SAPI0_event;
      new_SAPI0_event = EV_NO_EVENT;
    }
    else
    {
      SAPI0_event = EV_NO_EVENT;
    }
    if (l2_data_ptr->old_SAPI0_state != l2_data_ptr->SAPI0_state)
    {
      l2_display_SAPI0_state(l2_data_ptr->SAPI0_state);
      l2_data_ptr->old_SAPI0_state = l2_data_ptr->SAPI0_state;
    }
  }
}


