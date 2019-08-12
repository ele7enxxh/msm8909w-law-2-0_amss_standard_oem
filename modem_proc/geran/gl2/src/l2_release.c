/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         L 2   R E L E A S E

GENERAL DESCRIPTION
   This module contains functions that are related to releasing a link

EXTERNALIZED FUNCTIONS
  l2_release_link
  l2_handle_disc_reception
  l2_handle_release_ua_reception
  l2_handle_release_dm_reception
  l2_retransmit_disc_command
  l2_send_dm_response

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2_release.c_v   1.2   05 Jun 2002 10:54:48   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2_release.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
29/10/10   SJW      Added support for Dual SIM
10/17/02   JAC      Set timer state TO_BE_SET_MAX_LATENCY when sending UA
10/04/02   JAC      Code reorg
09/20/02   JAC      Added hack for retransmit disc command
12/14/01   JAC      Added F bit check in l2_handle_release_ua_reception
                    Changed l2_send_dm_response to modify correct buffer
                    when sending on SACCH sapi3
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
#include "l2i.h"
#include "l2_release.h"
#include "l2_msg.h"
#include "l2_transfer.h"
#include "l2_send.h"
#include "msg.h"

/*
 *   Function name:  l2_release_link
 *   -------------------------------
 *
 *   Description: Release a link
 *   ------------
 *
 *   Parameters: none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_release_link(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  
  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  l2_clear_sapi(l2_data_ptr, sapi, TRUE);

  if (l2_data_ptr->release_mode == NORMAL_RELEASE)
  {
    /**********************************/
    /* We have to code a DISC command */
    /**********************************/
    l2_code_disc(sapi_info, channel_info, FALSE);

    l2_clear_T200_timer(sapi_info);
    sapi_info->T200_state = TO_BE_SET;
  }
  else
  {
    l2_send_dl_release_cnf(l2_data_ptr, sapi, LOCAL_END_RELEASE);
  }

  return;
}

/*
 *   Function name:  l2_handle_disc_reception
 *   ----------------------------------------
 *
 *   Description: Receive a DISC command from the peer
 *   ------------
 *
 *   Parameters: none
 *   -----------
 *
 *   Returns:     l2_event - event for state machine
 *   --------
 */

void l2_handle_disc_reception(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  
  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  /************************************/
  /* We have to send back an UA frame */
  /************************************/
  l2_clear_sapi(l2_data_ptr, sapi, TRUE);

  l2_code_ua(l2_data_ptr->received_PF, sapi_info, channel_info);

  sapi_info->T200_state = TO_BE_SET_MAX_LATENCY;

  return;
}


/*
 *   Function name:  l2_handle_release_ua_reception
 *   ----------------------------------------------
 *
 *   Description: Receive the confirmation of release
 *   ------------ from the peer
 *
 *   Parameters: none
 *   -----------
 *
 *   Returns:     l2_event - event for state machine
 *   --------
 */

l2_event_T l2_handle_release_ua_reception(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  l2_event_T      l2_event;
   
  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return(EV_NO_EVENT);
  }

  if (l2_data_ptr->received_PF == 1)
  {
    l2_clear_sapi(l2_data_ptr, sapi, TRUE);

    l2_send_dl_release_cnf(l2_data_ptr, sapi, NORMAL_RELEASE);

    l2_event = EV_RELEASE_CONFIRMED;
  }
  else
  {
    MSG_GERAN_HIGH_0("Received UA response with F bit = 0?");
    l2_event = EV_NO_EVENT;
  }

  return(l2_event);
}


/*
 *   Function name:  l2_handle_release_dm_reception
 *   ----------------------------------------------
 *
 *   Description: Receive the confirmation of release
 *   ------------ from the peer via a DM frame
 *
 *   Parameters: none
 *   -----------
 *
 *   Returns:     l2_event - event for state machine
 *   --------
 */

l2_event_T l2_handle_release_dm_reception(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  l2_event_T      l2_event;
   
  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return(EV_NO_EVENT);
  }

  if (l2_data_ptr->received_PF == 1)
  {
    l2_clear_sapi(l2_data_ptr, sapi, TRUE);

    l2_send_dl_release_cnf(l2_data_ptr, sapi, NORMAL_RELEASE);

    l2_event = EV_RELEASE_CONFIRMED;
  }
  else
  {
    l2_event = EV_NO_EVENT;
  }

  return(l2_event);
}

/*
 *   Function name:  l2_retransmit_disc_command
 *   ------------------------------------------
 *
 *   Description: Handle T200 expiry
 *   ------------
 *
 *
 *   Parameters: none
 *   -----------
 *
 *
 *
 *   Returns:     l2_event - event for state machine
 *   --------
 *
 *
 */

l2_event_T l2_retransmit_disc_command(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  l2_event_T      l2_event;
   
  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return(EV_NO_EVENT);
  }

  /* if the disconnect has been retransmitted twice,
   * just release the link and send the RELEASE_CNF
   */
  if (sapi_info->retransmission_counter == 2)
  {
    l2_clear_sapi(l2_data_ptr, sapi, TRUE);

    /* MDL ERROR can probably be dropped here, since DL_RELEASE_CNF is sent */
    L2_DROP_MDL_ERROR_IND(T200_EXPIRY);

    l2_send_dl_release_cnf(l2_data_ptr, sapi, LOCAL_END_RELEASE);

    l2_event = EV_RELEASE_CONFIRMED;
  }
  else
  {
    channel_info->transmit_buffer.valid_data_flag = TRUE;
    channel_info->transmit_buffer.retransmission = TRUE;

    sapi_info->retransmission_counter = sapi_info->retransmission_counter + 1;

    sapi_info->T200_state = TO_BE_SET;

    l2_event = EV_NO_EVENT;
  }

  return(l2_event);
}

/*
 *   Function name:  l2_send_dm_response
 *   -----------------------------------
 *
 *   Description: Send a DM frame to the peer
 *   ------------
 *
 *   Parameters:  none
 *   -----------
 *
 *   Returns:     none
 *   --------
 */

void l2_send_dm_response(l2_store_T *l2_data_ptr, SAPI_T sapi, l2_channel_type_T channel)
{
  channel_info_T *channel_info;
  sapi_info_T    *sapi_info = l2_get_sapi_info(l2_data_ptr, sapi);
  
  channel_info = l2_get_channel_info(sapi_info, channel);
  if ((sapi_info == NULL) || (channel_info == NULL))
  {
    return;
  }

  l2_code_dm(l2_data_ptr->received_PF, sapi_info, channel_info);

  return;
}

