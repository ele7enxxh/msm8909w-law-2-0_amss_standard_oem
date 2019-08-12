/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         L 2   E S T A B L I S H

GENERAL DESCRIPTION
   This module contains the procedures related to data link establishment

EXTERNALIZED FUNCTIONS

   l2_establish_link
   l2_handle_sabm_reception
   l2_handle_establish_dm_reception
   l2_retransmit_sabm
   l2_handle_establish_ua_reception

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_establish.c_v   1.6   05 Jun 2002 10:52:24   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_establish.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
10/17/12   SJW      Improved length validation for message buffer handling
29/10/10   SJW      Added support for Dual SIM
05/08/08   SJW      N200 is no longer a global variable
02/02/04   VT       N200 is now a global variable
10/04/02   JAC      Code reorg
04/07/02   JAC      Set outstanding i frame variable to false on receipt of sabm
03/28/02   JAC      Cleared the send buffer if the max retransmission
                    of a sabm is sent
03/25/02   JAC      Added diagnostic messages for SABM retransmission
12/14/01   JAC      Set retransmission variable on SAPI3 saach to false in
                    L2_establish_link
10/19/01   JAC      Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "environ.h"
#include <string.h>
#include "sys_type.h"
#include "sys_stru.h"
#include "sys_cnst.h"
#include "rex.h"
#include "timers.h"
#include "gs.h"
#include "rr_l2_l1.h"
#include "rr_l2.h"
#include "l2_l1.h"
#include "l2_l1_g.h"
#include "msg.h"
#include "err.h"
#include "l2i.h"
#include "l2_establish.h"
#include "l2_msg.h"
#include "l2_transfer.h"
#include "l2_send.h"

/*****************************************************

 CONSTANTS

******************************************************/

#define L2_N200_ESTABLISH_COUNTER 5

/*****************************************************

 FUNCTION DECLARATIONS

******************************************************/


/*
 *   Function name:  l2_establish_link
 *   ---------------------------------
 *
 *   Description: Initiate a link establishment
 *   ------------
 *
 *
 *   Parameters:  none
 *   -----------
 *
 *
 *
 *   Returns:     none
 *   --------
 *
 *
 */

void l2_establish_link(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);

  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /*************************/
  /* Initialize the sapi   */
  /*************************/
  l2_clear_sapi(l2_data_ptr, sapi, TRUE);

  if (sapi == SAPI0)
  {
    /* clear SAPI3 - must have a SAPI0 link before starting SAPI3 */
    l2_clear_sapi(l2_data_ptr, SAPI3, TRUE);
  }

  if (l2_data_ptr->establish_mode == CONTENTION_RESOLUTION)
  {
    byte index;

    /************************************************/
    /* We have to code an SABM command              */
    /* As it is a CONTENTION RESOLUTION procedure,  */
    /* the SAPI is SAPI0 and the l2_channel_type is */
    /* DCCH                                         */
    /************************************************/
    if ((sapi != SAPI0) || (channel != DCCH))
    {
      MSG_GERAN_ERROR_2("Contention resolution improper parameters (%d,%d)",sapi,channel);
    }

    /* sanity check the length */
    if (l2_data_ptr->l3_message_length <= N201_DCCH)
    {
      l2_data_ptr->contention_message.valid_data_flag = TRUE;
      l2_data_ptr->contention_message.length          = l2_data_ptr->l3_message_length;
    }
    else
    {
      l2_data_ptr->contention_message.valid_data_flag = FALSE;
      l2_data_ptr->contention_message.length          = 0;
    }

    for (index = 0; index < l2_data_ptr->contention_message.length; index++)
    {
      l2_data_ptr->contention_message.layer3_message[index] = l2_data_ptr->l3_message_to_send[index];
    }
    l2_code_sabm(sapi_info, channel_info, FALSE, 
                 l2_data_ptr->contention_message.layer3_message,
                 l2_data_ptr->contention_message.length);
  }
  else
  {
    /************************************************************/
    /* The contention message variable is only valid for SAPI0, */
    /* SAPI3 does not use contention resolution                 */
    /************************************************************/
    if (sapi == SAPI0)
    {
      l2_data_ptr->contention_message.valid_data_flag = FALSE;
    }
    l2_code_sabm(sapi_info, channel_info, FALSE, NULL, 0);
  }

  sapi_info->T200_state = TO_BE_SET;

  return;
}

/*
 *   Function name:  l2_handle_sabm_reception
 *   ----------------------------------------
 *
 *   Description: Receive a SABM
 *   ------------
 *
 *
 *   Parameters:  SAPI, channel, for_reestablish
 *   -----------
 *
 *
 *
 *   Returns:     L2_event - event for state machine
 *   --------
 *
 *
 */

l2_event_T l2_handle_sabm_reception(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel, boolean for_reestablish)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  l2_event_T      l2_event;

  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return(EV_NO_EVENT);
  }

  /* SABMs received on the downlink must not have a payload */
  if (l2_data_ptr->l2_segment_length == 0)
  {
    if ((sapi == SAPI0) && (for_reestablish == TRUE))
    {
      /* SAPI0 is being re-established, so reset and reconnect SAPI3 as well */
      l2_clear_sapi(l2_data_ptr, SAPI3, TRUE);
      l2_SAPI3_control(l2_data_ptr, EV_RESET_L2);
      l2_SAPI3_control(l2_data_ptr, EV_CONNECT_RECEIVED);
    }

    /* tell RR that the SAPI has been (re)established */
    l2_send_dl_establish_ind(l2_data_ptr, sapi, channel, for_reestablish);

    /* clear the SAPI: all queued messages are abandoned */
    l2_clear_sapi(l2_data_ptr, sapi, TRUE);

    /********************************/
    /* L2 must send back a UA frame */
    /********************************/
    l2_code_ua(l2_data_ptr->received_PF, sapi_info, channel_info);

    l2_event = EV_LINK_ESTABLISHED;
  }
  else
  {
    /* 04.06 5.4.2.1 */
    L2_SEND_MDL_ERROR_IND(l2_data_ptr, SABM_COMMAND_INFO_FIELD_NOT_ALLOWED);
    l2_event = EV_NO_EVENT;
  }

  return(l2_event);
}


/*
 *   Function name:  l2_handle_establish_ua_reception
 *   ------------------------------------------------
 *
 *   Description: Receive a UA while in an establishment
 *   ------------ procedure
 *
 *
 *   Parameters:  none
 *   -----------
 *
 *
 *
 *   Returns:     l2_event = event for state machine
 *   --------
 *
 *
 */

l2_event_T l2_handle_establish_ua_reception(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  l2_event_T      l2_event  = EV_NO_EVENT;

  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return(EV_NO_EVENT);
  }

  if (l2_data_ptr->received_PF == 1)
  {
    /*********************************************************/
    /* The Poll / Final bit, in that case the Final bit is 1 */
    /*********************************************************/
    /*
     * if we've just received an UA for a SABM we are
     * about to retransmit, then kill the retransmission
     */
    if (channel_info->transmit_buffer.retransmission == TRUE)
    {
      channel_info->transmit_buffer.valid_data_flag = FALSE;
      channel_info->transmit_buffer.retransmission  = FALSE;
    }

    l2_clear_T200_timer(sapi_info);

    if (sapi == SAPI0)
    {
      /* On SAPI0 we need to do contention resolution on initial establishment */
      if (l2_data_ptr->contention_message.valid_data_flag == TRUE)
      {
        if ((l2_data_ptr->contention_message.length == l2_data_ptr->l2_segment_length) &&
            (memcmp(l2_data_ptr->contention_message.layer3_message, l2_data_ptr->received_l2_segment,
                    l2_data_ptr->l2_segment_length) == 0))
        {
          l2_data_ptr->contention_message.valid_data_flag = FALSE;

          l2_send_dl_establish_cnf(l2_data_ptr, CONTENTION_RESOLUTION);

          l2_event = EV_LINK_ESTABLISHED;
        }
        else
        {
          l2_data_ptr->contention_message.valid_data_flag = FALSE;

          l2_send_dl_release_ind(l2_data_ptr, SAPI0, NORMAL_RELEASE);

          l2_event = EV_CONTENTION_FAILED;
        }
      }
      else
      {
        MSG_GERAN_ERROR_0("No contention on sapi0 establishment");
      }
    }
    else
    {
      l2_send_dl_establish_cnf(l2_data_ptr, NORMAL_ESTABLISHMENT);

      l2_event = EV_LINK_ESTABLISHED;
    }

    /*
     * clear out establish mode, so as not to confuse things later
     */
    GS_SET_UNDEFINED(l2_data_ptr->establish_mode);
  }

  return(l2_event);
}

/*
 *   Function name:  l2_handle_establish_dm_reception
 *   ------------------------------------------------
 *
 *   Description: Receive a DM while in an establishment
 *   ------------ procedure
 *
 *
 *   Parameters:  none
 *   -----------
 *
 *
 *
 *   Returns:      l2_event - event for state machine
 *   --------
 *
 *
 */

l2_event_T l2_handle_establish_dm_reception(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  l2_event_T      l2_event = EV_NO_EVENT;

  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return(EV_NO_EVENT);
  }

  if (l2_data_ptr->received_PF == 1)
  {
    /*
     * this is sent rather than dl_release in order to avoid a
     * retry on the link
     */
    L2_SEND_MDL_ERROR_IND(l2_data_ptr, UNSOLICITED_DM_RESPONSE_PAR);

    /*****************************************************************************************/
    /* Stop transmitting the sabm, clear our send buffer in case we have the assign/handover */
    /* complete msg still there                                                              */
    /*****************************************************************************************/
    l2_clear_send_buffer(sapi_info, channel);

    l2_clear_T200_timer(sapi_info);

    l2_event = EV_ABORT_ESTABLISHMENT;
  }

  return(l2_event);
}

/*
 *   Function name:  l2_retransmit_sabm
 *   ----------------------------------
 *
 *   Description: Retransmit the SABM, as the UA wasn't received
 *   ------------
 *
 *
 *   Parameters: none
 *   -----------
 *
 *
 *
 *   Returns:    l2_event - event for state machine
 *   --------
 *
 *
 */

l2_event_T l2_retransmit_sabm(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  l2_event_T      l2_event  = EV_NO_EVENT;

  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return(EV_NO_EVENT);
  }

  /************************************************/
  /* The timer T200 has expired during the link   */
  /* establishment procedure: retransmit the SABM */
  /* up to 5 times, as per 44.006 section 5.8.2.1 */
  /************************************************/
  if (sapi_info->retransmission_counter == L2_N200_ESTABLISH_COUNTER)
  {
    /* send MDL ERROR so that RR knows NOT to repeat the assignment procedure */
    L2_SEND_MDL_ERROR_IND(l2_data_ptr, T200_EXPIRY);

    l2_send_dl_release_ind(l2_data_ptr, sapi, LOCAL_END_RELEASE);

    /* Cleanup send buffer for tidyness */
    l2_clear_send_buffer(sapi_info, channel);

    l2_event = EV_ABORT_ESTABLISHMENT;

    MSG_GERAN_HIGH_0("Aborting establishment");
  }
  else
  {
    sapi_info->retransmission_counter = sapi_info->retransmission_counter + 1;

    MSG_GERAN_HIGH_1("SABM Retransmission= %d",sapi_info->retransmission_counter);

    /**********************************************************************/
    /* On DCCH the transmit_buffer.layer_2_message will still contain the */
    /* SABM frame, sapi3 will also contain it, meas report are written    */
    /* into a different buffer                                            */
    /**********************************************************************/

    channel_info->transmit_buffer.valid_data_flag = TRUE;
    channel_info->transmit_buffer.retransmission = TRUE;
    sapi_info->T200_state = TO_BE_SET;
  }

  return(l2_event);
}
