/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         L 2   S A P I 3

GENERAL DESCRIPTION
   This module contains functions that are specific data link services
   on SAPI 3.

EXTERNALIZED FUNCTIONS
   l2_SAPI3_control

Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_sapi3.c_v   1.4   05 Jun 2002 10:56:18   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_sapi3.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
29/10/10   SJW      Added support for Dual SIM
02/13/03   JAC      Added handler for link establishment even if link already established
10/18/02   JAC      Removed wait_UA_sent variable, in l2_data
10/17/02   JAC      Interface change to l2_handle_optional_send_timeout
10/04/02   JAC      Code reorg
05/30/02   JAC      added SAPI3_UA_PENDING
05/15/02   JAC      Added release req handling
03/03/02   JAC      Added function to display state
02/28/02   JAC      Added logging for states
10/19/01   JAC      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
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
#include "l2_log.h"
#include "l2_transmit.h"

/*
 *   Function name:  l2_display_SAPI3_state
 *   --------------------------------------
 *
 *   Description: Display the SAPI3 state
 *   ------------
 *
 *
 *   Parameters:  Sapi3 state
 *   -----------
 *
 *   Returns:     none
 *   --------
 *
 */

static void l2_display_SAPI3_state(SAPI3_state_T SAPI3_state)
{
  switch (SAPI3_state)
  {
    case SAPI3_NULL:
      MSG_GERAN_HIGH_0("L2 state:SAPI3_NULL");
      break;

    case SAPI3_CONNECT_PEND:
      MSG_GERAN_HIGH_0("L2 state:SAPI3_CONNECT_PEND");
      break;

    case SAPI3_IDLE:
      MSG_GERAN_HIGH_0("L2 state:SAPI3_IDLE");
      break;

    case SAPI3_ESTABLISHMENT_PENDING:
      MSG_GERAN_HIGH_0("L2 state:SAPI3_ESTABLISHMENT_PENDING");
      break;

    case SAPI3_RELEASE_PENDING:
      MSG_GERAN_HIGH_0("L2 state:SAPI3_RELEASE_PENDING");
      break;

    case SAPI3_LINK_ESTABLISHED:
      MSG_GERAN_HIGH_0("L2 state:SAPI3_LINK_ESTABLISHED");
      break;

    case SAPI3_TIMER_RECOVERY:
      MSG_GERAN_HIGH_0("L2 state:SAPI3_TIMER_RECOVERY");
      break;

    case SAPI3_UA_PENDING:
      MSG_GERAN_HIGH_0("L2 state:SAPI3_UA_PENDING");
      break;

    default:
      MSG_GERAN_HIGH_0("L2 state:UNKNOWN STATE");
      break;

  }

  return;
}



/*
 *   Function name:  l2_SAPI3_control
 *   --------------------------------
 *
 *   Description: Control the SAPI3 state machine
 *   ------------
 *
 *   Parameters:  SAPI3_event - input event for state machine
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_SAPI3_control(l2_store_T *l2_data_ptr, l2_event_T SAPI3_event)
{
  l2_event_T new_SAPI3_event = EV_NO_EVENT;

  if (SAPI3_event == EV_RESET_L2)
  {
    l2_data_ptr->SAPI3_state = SAPI3_NULL;
    l2_data_ptr->old_SAPI3_state = SAPI3_NULL;
    SAPI3_event = EV_NO_EVENT;
    l2_log_state(l2_data_ptr, SAPI3,(byte)l2_data_ptr->SAPI3_state,SAPI3_event);
  }
  else if (SAPI3_event == EV_ESTABLISH_REQUEST)
  {
    if ((l2_data_ptr->SAPI3_state != SAPI3_NULL) && (l2_data_ptr->SAPI3_state != SAPI3_IDLE))
    {
      /* Should never happen */
      MSG_GERAN_ERROR_1("Link already established %d, reset",(int)l2_data_ptr->SAPI3_state);

      /* Go to idle, and handle the establish request there */
      l2_data_ptr->SAPI3_state = SAPI3_IDLE;
    }
  }

  while (SAPI3_event != EV_NO_EVENT)
  {
    l2_log_state(l2_data_ptr, SAPI3,(byte)l2_data_ptr->SAPI3_state,SAPI3_event);

    switch (l2_data_ptr->SAPI3_state)
    {
      case SAPI3_NULL:
        {
          if (SAPI3_event == EV_CONNECT_RECEIVED)
          {
            l2_data_ptr->SAPI3_state = SAPI3_IDLE;
          }
          else if (SAPI3_event == EV_ESTABLISH_REQUEST)
          {
            /*
             * an establishment request has been received ahead of the
             * connect ind from layer 1 - This is possible as there is
             * a race between RR sending the request and layer 1 sending
             * the connect. Need to go to a transient state to handle
             * this situation
             */
            l2_data_ptr->SAPI3_state = SAPI3_CONNECT_PEND;
          }
          else if ((SAPI3_event == EV_RELEASE_REQUEST) && (l2_data_ptr->release_mode == LOCAL_END_RELEASE))
          {
            MSG_GERAN_ERROR_0("Received release req before link established");
            l2_release_link(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI3_state = SAPI3_NULL;
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event:%d ",SAPI3_event);
          }


          break;
        }

      case SAPI3_CONNECT_PEND:
        {
          if (SAPI3_event == EV_CONNECT_RECEIVED)
          {
            l2_establish_link(l2_data_ptr, SAPI3, l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI3_state = SAPI3_ESTABLISHMENT_PENDING;
          }
          else if ((SAPI3_event == EV_RELEASE_REQUEST) && (l2_data_ptr->release_mode == LOCAL_END_RELEASE))
          {
            MSG_GERAN_ERROR_0("Received release req before link established");
            l2_release_link(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI3_state = SAPI3_NULL;
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event:%d ",SAPI3_event);
          }
          break;
        }


      case SAPI3_IDLE:
        {
          if (SAPI3_event == EV_RELEASE_REQUEST)
          {
            MSG_GERAN_ERROR_0("Received release req before link established");
            l2_data_ptr->release_mode = LOCAL_END_RELEASE;
            l2_release_link(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI3_state = SAPI3_IDLE;
          }
          else if (SAPI3_event == EV_ESTABLISH_REQUEST)
          {
            l2_establish_link(l2_data_ptr, SAPI3, l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI3_state = SAPI3_ESTABLISHMENT_PENDING;
          }
          else if (SAPI3_event == EV_SABM_RECEIVED)
          {
            new_SAPI3_event = l2_handle_sabm_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type,FALSE);
          }
          else if (SAPI3_event == EV_LINK_ESTABLISHED)
          {
            l2_data_ptr->SAPI3_state = SAPI3_LINK_ESTABLISHED;
          }
          else if (SAPI3_event == EV_DISC_RECEIVED)
          {
            l2_send_dm_response(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if ((SAPI3_event == EV_I_RECEIVED) ||
                   (SAPI3_event == EV_RR_RECEIVED ) ||
                   (SAPI3_event == EV_REJ_RECEIVED ))
          {
            if (l2_data_ptr->received_PF == 1)
            {
              l2_send_dm_response(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            }
          }
          else if (SAPI3_event == EV_CONNECT_RECEIVED)
          {
            /* soak this up: it happens on every new connection */
            MSG_GERAN_LOW_0("EV_CONNECT_RECEIVED in SAPI3_IDLE");
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event:%d ",SAPI3_event);
          }
          break;
        }

      case SAPI3_ESTABLISHMENT_PENDING:
        {
          if (SAPI3_event == EV_RELEASE_REQUEST)
          {
            MSG_GERAN_ERROR_0("Received release req before link established");
            l2_data_ptr->release_mode = LOCAL_END_RELEASE;
            l2_release_link(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI3_state = SAPI3_IDLE;
          }
          else if (SAPI3_event == EV_UA_RECEIVED)
          {
            new_SAPI3_event = l2_handle_establish_ua_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_DM_RECEIVED)
          {
            new_SAPI3_event = l2_handle_establish_dm_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_ABORT_ESTABLISHMENT)
          {
            l2_data_ptr->SAPI3_state = SAPI3_IDLE;
          }
          else if (SAPI3_event == EV_LINK_ESTABLISHED)
          {
            l2_data_ptr->SAPI3_state = SAPI3_LINK_ESTABLISHED;
          }
          else if (SAPI3_event == EV_T200_TIMEOUT)
          {
            new_SAPI3_event = l2_retransmit_sabm(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_SABM_RECEIVED)
          {
            /* establish collision */
            new_SAPI3_event = l2_handle_sabm_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type,FALSE);
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event:%d ",SAPI3_event);
          }

          break;
        }

      case SAPI3_RELEASE_PENDING:
        {
          if (SAPI3_event == EV_RELEASE_REQUEST)
          {
            MSG_GERAN_ERROR_0("Already releasing");
            l2_data_ptr->release_mode = LOCAL_END_RELEASE;
            l2_release_link(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI3_state = SAPI3_IDLE;
          }
          else if (SAPI3_event == EV_UA_RECEIVED)
          {
            new_SAPI3_event = l2_handle_release_ua_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_DM_RECEIVED)
          {
            new_SAPI3_event = l2_handle_release_dm_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_T200_TIMEOUT)
          {
            new_SAPI3_event = l2_retransmit_disc_command(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_RELEASE_CONFIRMED)
          {
            l2_data_ptr->SAPI3_state = SAPI3_IDLE;
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event:%d ",SAPI3_event);
          }

          break;
        }

      case SAPI3_UA_PENDING:
        {
          if (SAPI3_event == EV_RELEASE_REQUEST)
          {
            MSG_GERAN_ERROR_0("Already releasing");
            l2_data_ptr->release_mode = LOCAL_END_RELEASE;
            l2_release_link(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI3_state = SAPI3_IDLE;
          }
          else if (SAPI3_event == EV_T200_TIMEOUT)
          {
            l2_send_dl_release_ind(l2_data_ptr,SAPI3, NORMAL_RELEASE);
            l2_data_ptr->SAPI3_state = SAPI3_IDLE;
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event %d",SAPI3_event);
          }

          break;
        }
      case SAPI3_LINK_ESTABLISHED:
        {
          if (SAPI3_event == EV_RELEASE_REQUEST)
          {
            l2_release_link(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);

            if (l2_data_ptr->release_mode == NORMAL_RELEASE)
            {
              l2_data_ptr->SAPI3_state = SAPI3_RELEASE_PENDING;
            }
            else
            {
              l2_data_ptr->SAPI3_state = SAPI3_IDLE;
            }
          }
          else if (SAPI3_event == EV_SABM_RECEIVED)
          {
            /* re-establishment of SAPI3 - remain in this state */
            (void)l2_handle_sabm_reception(l2_data_ptr, SAPI3, l2_data_ptr->current_l2_channel_type, TRUE);
          }
          else if (SAPI3_event == EV_DISC_RECEIVED)
          {
            l2_handle_disc_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI3_state = SAPI3_UA_PENDING;
          }
          else if (SAPI3_event == EV_UA_RECEIVED)
          {
            L2_SEND_MDL_ERROR_IND(l2_data_ptr, UNSOLICITED_UA_RESPONSE);
          }
          else if (SAPI3_event == EV_DM_RECEIVED)
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
          else if (SAPI3_event == EV_I_RECEIVED)
          {
            l2_handle_info_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_RR_RECEIVED)
          {
            l2_handle_normal_rr_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_REJ_RECEIVED)
          {
            l2_handle_normal_rej_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_T200_TIMEOUT)
          {
            new_SAPI3_event = l2_check_pending_sequence_error(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            l2_handle_I_retransmission(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI3_state = SAPI3_TIMER_RECOVERY;
          }
          else if (SAPI3_event == EV_DATA_REQUEST)
          {
            l2_store_new_l3_message(l2_data_ptr, SAPI3, l2_data_ptr->current_l2_channel_type, SAPI_ESTABLISHED);
          }
          else if (SAPI3_event == EV_OPTIONAL_SEND)
          {
            l2_handle_optional_send_timeout(l2_data_ptr, l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_CONNECT_RECEIVED)
          {
            /* this may happen if the channel mode is modified */
            MSG_GERAN_HIGH_1("SAPI3 PH_CONNECT_IND from Layer 1: channel %d",
                     (int)l2_data_ptr->last_dcch_type);
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event:%d ",SAPI3_event);
          }

          break;
        }

      case SAPI3_TIMER_RECOVERY:
        {
          if (SAPI3_event == EV_RELEASE_REQUEST)
          {
            l2_release_link(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);

            if (l2_data_ptr->release_mode == NORMAL_RELEASE)
            {
              l2_data_ptr->SAPI3_state = SAPI3_RELEASE_PENDING;
            }
            else
            {
              l2_data_ptr->SAPI3_state = SAPI3_IDLE;
            }
          }
          else if (SAPI3_event == EV_SABM_RECEIVED)
          {
            /* re-establishment of SAPI3 - remain in this state */
            new_SAPI3_event = l2_handle_sabm_reception(l2_data_ptr,SAPI3, l2_data_ptr->current_l2_channel_type, TRUE);
          }
          else if (SAPI3_event == EV_LINK_ESTABLISHED)
          {
            l2_data_ptr->SAPI3_state = SAPI3_LINK_ESTABLISHED;
          }
          else if (SAPI3_event == EV_I_RECEIVED)
          {
            l2_handle_info_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_RR_RECEIVED)
          {
            new_SAPI3_event = l2_handle_recovery_rr_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_REJ_RECEIVED)
          {
            new_SAPI3_event = l2_handle_recovery_rej_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_CLEAR_RECOVERY_CONDITION)
          {
            l2_data_ptr->SAPI3_state = SAPI3_LINK_ESTABLISHED;
          }
          else if (SAPI3_event == EV_DATA_REQUEST)
          {
            l2_store_new_l3_message(l2_data_ptr, SAPI3, l2_data_ptr->current_l2_channel_type, SAPI_ESTABLISHED);
          }
          else if (SAPI3_event == EV_T200_TIMEOUT)
          {
            new_SAPI3_event = l2_check_pending_sequence_error(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            l2_handle_I_retransmission(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_OPTIONAL_SEND)
          {
            l2_handle_optional_send_timeout(l2_data_ptr, l2_data_ptr->current_l2_channel_type);
          }
          else if (SAPI3_event == EV_DISC_RECEIVED)
          {
            l2_handle_disc_reception(l2_data_ptr,SAPI3,l2_data_ptr->current_l2_channel_type);
            l2_data_ptr->SAPI3_state = SAPI3_UA_PENDING;
          }
          else
          {
            MSG_GERAN_HIGH_1("Ignored event:%d ",SAPI3_event);
          }
          break;
        }
    }

    if (new_SAPI3_event != EV_NO_EVENT)
    {
      SAPI3_event = new_SAPI3_event;
      new_SAPI3_event = EV_NO_EVENT;
    }
    else
    {
      SAPI3_event = EV_NO_EVENT;
    }

    if ( l2_data_ptr->old_SAPI3_state != l2_data_ptr->SAPI3_state )
    {
      l2_display_SAPI3_state( l2_data_ptr->SAPI3_state );
      l2_data_ptr->old_SAPI3_state = l2_data_ptr->SAPI3_state;
    }
  }
}



